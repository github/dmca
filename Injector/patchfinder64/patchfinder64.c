//
//  patchfinder64.c
//  extra_recipe
//
//  Created by xerub on 06/06/2017.
//  Copyright © 2017 xerub. All rights reserved.
//

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>
#include "mac_policy.h"
#include "patchfinder64.h"

bool auth_ptrs = false;
typedef unsigned long long addr_t;
static addr_t kerndumpbase = -1;
static addr_t xnucore_base = 0;
static addr_t xnucore_size = 0;
static addr_t ppl_base = 0;
static addr_t ppl_size = 0;
static addr_t prelink_base = 0;
static addr_t prelink_size = 0;
static addr_t cstring_base = 0;
static addr_t cstring_size = 0;
static addr_t pstring_base = 0;
static addr_t pstring_size = 0;
static addr_t oslstring_base = 0;
static addr_t oslstring_size = 0;
static addr_t data_base = 0;
static addr_t data_size = 0;
static addr_t data_const_base = 0;
static addr_t data_const_size = 0;
static addr_t const_base = 0;
static addr_t const_size = 0;
static addr_t kernel_entry = 0;
static void *kernel_mh = 0;
static addr_t kernel_delta = 0;
bool monolithic_kernel = false;


#define IS64(image) (*(uint8_t *)(image) & 1)

#define MACHO(p) ((*(unsigned int *)(p) & ~1) == 0xfeedface)

/* generic stuff *************************************************************/

#define UCHAR_MAX 255

/* these operate on VA ******************************************************/

#define INSN_RET  0xD65F03C0, 0xFFFFFFFF
#define INSN_CALL 0x94000000, 0xFC000000
#define INSN_B    0x14000000, 0xFC000000
#define INSN_CBZ  0x34000000, 0x7F000000
#define INSN_ADRP 0x90000000, 0x9F000000

static unsigned char *
boyermoore_horspool_memmem(const unsigned char* haystack, size_t hlen,
                           const unsigned char* needle,   size_t nlen)
{
    size_t last, scan = 0;
    size_t bad_char_skip[UCHAR_MAX + 1]; /* Officially called:
                                          * bad character shift */

    /* Sanity checks on the parameters */
    if (nlen <= 0 || !haystack || !needle)
        return NULL;

    /* ---- Preprocess ---- */
    /* Initialize the table to default value */
    /* When a character is encountered that does not occur
     * in the needle, we can safely skip ahead for the whole
     * length of the needle.
     */
    for (scan = 0; scan <= UCHAR_MAX; scan = scan + 1)
        bad_char_skip[scan] = nlen;

    /* C arrays have the first byte at [0], therefore:
     * [nlen - 1] is the last byte of the array. */
    last = nlen - 1;

    /* Then populate it with the analysis of the needle */
    for (scan = 0; scan < last; scan = scan + 1)
        bad_char_skip[needle[scan]] = last - scan;

    /* ---- Do the matching ---- */

    /* Search the haystack, while the needle can still be within it. */
    while (hlen >= nlen)
    {
        /* scan from the end of the needle */
        for (scan = last; haystack[scan] == needle[scan]; scan = scan - 1)
            if (scan == 0) /* If the first byte matches, we've found it. */
                return (void *)haystack;

        /* otherwise, we need to skip some bytes and start again.
           Note that here we are getting the skip value based on the last byte
           of needle, no matter where we didn't match. So if needle is: "abcd"
           then we are skipping based on 'd' and that value will be 4, and
           for "abcdd" we again skip on 'd' but the value will be only 1.
           The alternative of pretending that the mismatched character was
           the last character is slower in the normal case (E.g. finding
           "abcd" in "...azcd..." gives 4 by using 'd' but only
           4-2==2 using 'z'. */
        hlen     -= bad_char_skip[haystack[last]];
        haystack += bad_char_skip[haystack[last]];
    }

    return NULL;
}

/* disassembler **************************************************************/

static int HighestSetBit(int N, uint32_t imm)
{
	int i;
	for (i = N - 1; i >= 0; i--) {
		if (imm & (1 << i)) {
			return i;
		}
	}
	return -1;
}

static uint64_t ZeroExtendOnes(unsigned M, unsigned N)	// zero extend M ones to N width
{
	(void)N;
	return ((uint64_t)1 << M) - 1;
}

static uint64_t RORZeroExtendOnes(unsigned M, unsigned N, unsigned R)
{
	uint64_t val = ZeroExtendOnes(M, N);
	if (R == 0) {
		return val;
	}
	return ((val >> R) & (((uint64_t)1 << (N - R)) - 1)) | ((val & (((uint64_t)1 << R) - 1)) << (N - R));
}

static uint64_t Replicate(uint64_t val, unsigned bits)
{
	uint64_t ret = val;
	unsigned shift;
	for (shift = bits; shift < 64; shift += bits) {	// XXX actually, it is either 32 or 64
		ret |= (val << shift);
	}
	return ret;
}

static int DecodeBitMasks(unsigned immN, unsigned imms, unsigned immr, int immediate, uint64_t *newval)
{
	unsigned levels, S, R, esize;
	int len = HighestSetBit(7, (immN << 6) | (~imms & 0x3F));
	if (len < 1) {
		return -1;
	}
	levels = (unsigned int)ZeroExtendOnes(len, 6);
	if (immediate && (imms & levels) == levels) {
		return -1;
	}
	S = imms & levels;
	R = immr & levels;
	esize = 1 << len;
	*newval = Replicate(RORZeroExtendOnes(S + 1, esize, R), esize);
	return 0;
}

static int DecodeMov(uint32_t opcode, uint64_t total, int first, uint64_t *newval)
{
	unsigned o = (opcode >> 29) & 3;
	unsigned k = (opcode >> 23) & 0x3F;
	unsigned rn, rd;
	uint64_t i;

	if (k == 0x24 && o == 1) {			// MOV (bitmask imm) <=> ORR (immediate)
		unsigned s = (opcode >> 31) & 1;
		unsigned N = (opcode >> 22) & 1;
		if (s == 0 && N != 0) {
			return -1;
		}
		rn = (opcode >> 5) & 0x1F;
		if (rn == 31) {
			unsigned imms = (opcode >> 10) & 0x3F;
			unsigned immr = (opcode >> 16) & 0x3F;
			return DecodeBitMasks(N, imms, immr, 1, newval);
		}
	} else if (k == 0x25) {				// MOVN/MOVZ/MOVK
		unsigned s = (opcode >> 31) & 1;
		unsigned h = (opcode >> 21) & 3;
		if (s == 0 && h > 1) {
			return -1;
		}
		i = (opcode >> 5) & 0xFFFF;
		h *= 16;
		i <<= h;
		if (o == 0) {				// MOVN
			*newval = ~i;
			return 0;
		} else if (o == 2) {			// MOVZ
			*newval = i;
			return 0;
		} else if (o == 3 && !first) {		// MOVK
			*newval = (total & ~((uint64_t)0xFFFF << h)) | i;
			return 0;
		}
	} else if ((k | 1) == 0x23 && !first) {		// ADD (immediate)
		unsigned h = (opcode >> 22) & 3;
		if (h > 1) {
			return -1;
		}
		rd = opcode & 0x1F;
		rn = (opcode >> 5) & 0x1F;
		if (rd != rn) {
			return -1;
		}
		i = (opcode >> 10) & 0xFFF;
		h *= 12;
		i <<= h;
		if (o & 2) {				// SUB
			*newval = total - i;
			return 0;
		} else {				// ADD
			*newval = total + i;
			return 0;
		}
	}

	return -1;
}

/* patchfinder ***************************************************************/

static addr_t
step64(const uint8_t *buf, addr_t start, size_t length, uint32_t what, uint32_t mask)
{
    addr_t end = start + length;
    while (start < end) {
        uint32_t x = *(uint32_t *)(buf + start);
        if ((x & mask) == what) {
            return start;
        }
        start += 4;
    }
    return 0;
}

static addr_t
step64_back(const uint8_t *buf, addr_t start, size_t length, uint32_t what, uint32_t mask)
{
    addr_t end = start - length;
    while (start >= end) {
        uint32_t x = *(uint32_t *)(buf + start);
        if ((x & mask) == what) {
            return start;
        }
        start -= 4;
    }
    return 0;
}

static addr_t
step_adrp_to_reg(const uint8_t *buf, addr_t start, size_t length, int reg)
{
    return step64(buf, start, length, 0x90000000 | (reg&0x1F), 0x9F00001F);
}

static addr_t
bof64(const uint8_t *buf, addr_t start, addr_t where)
{
    if (auth_ptrs) {
        for (; where >= start; where -= 4) {
            uint32_t op = *(uint32_t *)(buf + where);
            if (op == 0xD503237F) {
                return where;
            }
        }
        return 0;
    }
    for (; where >= start; where -= 4) {
        uint32_t op = *(uint32_t *)(buf + where);
        if ((op & 0xFFC003FF) == 0x910003FD) {
            unsigned delta = (op >> 10) & 0xFFF;
            //printf("0x%llx: ADD X29, SP, #0x%x\n", where + kerndumpbase, delta);
            if ((delta & 0xF) == 0) {
                addr_t prev = where - ((delta >> 4) + 1) * 4;
                uint32_t au = *(uint32_t *)(buf + prev);
                //printf("0x%llx: (%llx & %llx) == %llx\n", prev + kerndumpbase, au, 0x3BC003E0, au & 0x3BC003E0);
                if ((au & 0x3BC003E0) == 0x298003E0) {
                    //printf("%x: STP x, y, [SP,#-imm]!\n", prev);
                    return prev;
                } else if ((au & 0x7F8003FF) == 0x510003FF) {
                    //printf("%x: SUB SP, SP, #imm\n", prev);
                    return prev;
                }
                for (addr_t diff = 4; diff < delta/4+4; diff+=4) {
                    uint32_t ai = *(uint32_t *)(buf + where - diff);
                    // SUB SP, SP, #imm
                    //printf("0x%llx: (%llx & %llx) == %llx\n", where - diff + kerndumpbase, ai, 0x3BC003E0, ai & 0x3BC003E0);
                    if ((ai & 0x7F8003FF) == 0x510003FF) {
                        return where - diff;
                    }
                    // Not stp and not str
                    if (((ai & 0xFFC003E0) != 0xA90003E0) && (ai&0xFFC001F0) != 0xF90001E0) {
                        break;
                    }
                }
                // try something else
                while (where > start) {
                    where -= 4;
                    au = *(uint32_t *)(buf + where);
                    // SUB SP, SP, #imm
                    if ((au & 0xFFC003FF) == 0xD10003FF && ((au >> 10) & 0xFFF) == delta + 0x10) {
                        return where;
                    }
                    // STP x, y, [SP,#imm]
                    if ((au & 0xFFC003E0) != 0xA90003E0) {
                        where += 4;
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

static addr_t
xref64(const uint8_t *buf, addr_t start, addr_t end, addr_t what)
{
    addr_t i;
    uint64_t value[32];

    memset(value, 0, sizeof(value));

    end &= ~3;
    for (i = start & ~3; i < end; i += 4) {
        uint32_t op = *(uint32_t *)(buf + i);
        unsigned reg = op & 0x1F;

        if ((op & 0x9F000000) == 0x90000000) {
            signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
            //printf("%llx: ADRP X%d, 0x%llx\n", i, reg, ((long long)adr << 1) + (i & ~0xFFF));
            value[reg] = ((long long)adr << 1) + (i & ~0xFFF);
            continue;				// XXX should not XREF on its own?
        /*} else if ((op & 0xFFE0FFE0) == 0xAA0003E0) {
            unsigned rd = op & 0x1F;
            unsigned rm = (op >> 16) & 0x1F;
            //printf("%llx: MOV X%d, X%d\n", i, rd, rm);
            value[rd] = value[rm];*/
        } else if ((op & 0xFF000000) == 0x91000000) {
            unsigned rn = (op >> 5) & 0x1F;
            if (rn == 0x1f) {
                // ignore ADD Xn, SP
                value[reg] = 0;
                continue;
            }

            unsigned shift = (op >> 22) & 3;
            unsigned imm = (op >> 10) & 0xFFF;
            if (shift == 1) {
                imm <<= 12;
            } else {
                //assert(shift == 0);
                if (shift > 1) continue;
            }
            //printf("%llx: ADD X%d, X%d, 0x%x\n", i, reg, rn, imm);
            value[reg] = value[rn] + imm;
        } else if ((op & 0xF9C00000) == 0xF9400000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: LDR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            if (!imm) continue;			// XXX not counted as true xref
            value[reg] = value[rn] + imm;	// XXX address, not actual value
        /*} else if ((op & 0xF9C00000) == 0xF9000000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: STR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            if (!imm) continue;			// XXX not counted as true xref
            value[rn] = value[rn] + imm;	// XXX address, not actual value*/
        } else if ((op & 0x9F000000) == 0x10000000) {
            signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
            //printf("%llx: ADR X%d, 0x%llx\n", i, reg, ((long long)adr >> 11) + i);
            value[reg] = ((long long)adr >> 11) + i;
        } else if ((op & 0xFF000000) == 0x58000000) {
            unsigned adr = (op & 0xFFFFE0) >> 3;
            //printf("%llx: LDR X%d, =0x%llx\n", i, reg, adr + i);
            value[reg] = adr + i;		// XXX address, not actual value
        } else if ((op & 0xFC000000) == 0x94000000) {
            // BL addr
            signed imm = (op & 0x3FFFFFF) << 2;
            if (op & 0x2000000) {
                imm |= 0xf << 28;
            }
            unsigned adr = (unsigned)(i + imm);
            if (adr == what) {
                return i;
            }
        }
        // Don't match SP as an offset
        if (value[reg] == what && reg != 0x1f) {
            return i;
        }
    }
    return 0;
}

static addr_t
calc64(const uint8_t *buf, addr_t start, addr_t end, int which)
{
    addr_t i;
    uint64_t value[32];

    memset(value, 0, sizeof(value));

    end &= ~3;
    for (i = start & ~3; i < end; i += 4) {
        uint32_t op = *(uint32_t *)(buf + i);
        unsigned reg = op & 0x1F;
        if ((op & 0x9F000000) == 0x90000000) {
            signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
            //printf("%llx: ADRP X%d, 0x%llx\n", i, reg, ((long long)adr << 1) + (i & ~0xFFF));
            value[reg] = ((long long)adr << 1) + (i & ~0xFFF);
        /*} else if ((op & 0xFFE0FFE0) == 0xAA0003E0) {
            unsigned rd = op & 0x1F;
            unsigned rm = (op >> 16) & 0x1F;
            //printf("%llx: MOV X%d, X%d\n", i, rd, rm);
            value[rd] = value[rm];*/
        } else if ((op & 0xFF000000) == 0x91000000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned shift = (op >> 22) & 3;
            unsigned imm = (op >> 10) & 0xFFF;
            if (shift == 1) {
                imm <<= 12;
            } else {
                //assert(shift == 0);
                if (shift > 1) continue;
            }
            //printf("%llx: ADD X%d, X%d, 0x%x\n", i, reg, rn, imm);
            value[reg] = value[rn] + imm;
        } else if ((op & 0xF9C00000) == 0xF9400000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: LDR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            if (!imm) continue;			// XXX not counted as true xref
            value[reg] = value[rn] + imm;	// XXX address, not actual value
        } else if ((op & 0xF9C00000) == 0xF9000000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: STR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            if (!imm) continue;			// XXX not counted as true xref
            value[rn] = value[rn] + imm;	// XXX address, not actual value
        } else if ((op & 0x9F000000) == 0x10000000) {
            signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
            //printf("%llx: ADR X%d, 0x%llx\n", i, reg, ((long long)adr >> 11) + i);
            value[reg] = ((long long)adr >> 11) + i;
        } else if ((op & 0xFF000000) == 0x58000000) {
            unsigned adr = (op & 0xFFFFE0) >> 3;
            //printf("%llx: LDR X%d, =0x%llx\n", i, reg, adr + i);
            value[reg] = adr + i;		// XXX address, not actual value
        } else if ((op & 0xF9C00000) == 0xb9400000) { // 32bit
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 2;
            if (!imm) continue;            // XXX not counted as true xref
            value[reg] = value[rn] + imm;    // XXX address, not actual value
        }
    }
    return value[which];
}

static addr_t
calc64mov(const uint8_t *buf, addr_t start, addr_t end, int which)
{
    addr_t i;
    uint64_t value[32];

    memset(value, 0, sizeof(value));

    end &= ~3;
    for (i = start & ~3; i < end; i += 4) {
        uint32_t op = *(uint32_t *)(buf + i);
        unsigned reg = op & 0x1F;
        uint64_t newval;
        int rv = DecodeMov(op, value[reg], 0, &newval);
        if (rv == 0) {
            if (((op >> 31) & 1) == 0) {
                newval &= 0xFFFFFFFF;
            }
            value[reg] = newval;
        }
    }
    return value[which];
}

static addr_t
find_call64(const uint8_t *buf, addr_t start, size_t length)
{
    return step64(buf, start, length, INSN_CALL);
}

static addr_t
follow_call64(const uint8_t *buf, addr_t call)
{
    long long w;
    w = *(uint32_t *)(buf + call) & 0x3FFFFFF;
    w <<= 64 - 26;
    w >>= 64 - 26 - 2;
    return call + w;
}

static addr_t
follow_stub(const uint8_t *buf, addr_t call)
{
    addr_t stub = follow_call64(buf, call);
    if (!stub) return 0;

    if (monolithic_kernel) {
        return stub + kerndumpbase;
    }
    addr_t target_function_offset = calc64(buf, stub, stub+4*3, 16);
    if (!target_function_offset) return 0;

    return *(addr_t*)(buf + target_function_offset);
}

static addr_t
follow_cbz(const uint8_t *buf, addr_t cbz)
{
    return cbz + ((*(int *)(buf + cbz) & 0x3FFFFE0) << 10 >> 13);
}

static addr_t
remove_pac(addr_t addr)
{
    if (addr >= kerndumpbase) return addr;
    if (addr >> 56 == 0x80) {
        return (addr&0xffffffff) + kerndumpbase;
    }
    return addr |= 0xfffffff000000000;
}

/* kernel iOS10 **************************************************************/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef NOT_DARWIN
#include <mach-o/loader.h>
#else
#include "mach-o_loader.h"
#endif

#ifdef VFS_H_included
#define INVALID_HANDLE NULL
static FHANDLE
OPEN(const char *filename, int oflag)
{
    // XXX use sub_reopen() to handle FAT
    return img4_reopen(file_open(filename, oflag), NULL, 0);
}
#define CLOSE(fd) (fd)->close(fd)
#define READ(fd, buf, sz) (fd)->read(fd, buf, sz)
static ssize_t
PREAD(FHANDLE fd, void *buf, size_t count, off_t offset)
{
    ssize_t rv;
    //off_t pos = fd->lseek(FHANDLE fd, 0, SEEK_CUR);
    fd->lseek(fd, offset, SEEK_SET);
    rv = fd->read(fd, buf, count);
    //fd->lseek(FHANDLE fd, pos, SEEK_SET);
    return rv;
}
#else
#define FHANDLE int
#define INVALID_HANDLE -1
#define OPEN open
#define CLOSE close
#define READ read
#define PREAD pread
#endif

static uint8_t *kernel = NULL;
static size_t kernel_size = 0;

int
init_kernel(size_t (*kread)(uint64_t, void *, size_t), addr_t kernel_base, const char *filename)
{
    size_t rv;
    uint8_t buf[0x4000];
    unsigned i, j;
    const struct mach_header *hdr = (struct mach_header *)buf;
    FHANDLE fd = INVALID_HANDLE;
    const uint8_t *q;
    addr_t min = -1;
    addr_t max = 0;
    int is64 = 0;

    if (filename == NULL) {
        if (!kread || !kernel_base) {
            return -1;
        }
        rv = kread(kernel_base, buf, sizeof(buf));
        if (rv != sizeof(buf) || !MACHO(buf)) {
            return -1;
        }
    } else {
        fd = OPEN(filename, O_RDONLY);
        if (fd == INVALID_HANDLE) {
            return -1;
        }
        rv = READ(fd, buf, sizeof(buf));
        if (rv != sizeof(buf) || !MACHO(buf)) {
            CLOSE(fd);
            return -1;
        }
    }

    if (IS64(buf)) {
        is64 = 4;
    }

    q = buf + sizeof(struct mach_header) + is64;
    for (i = 0; i < hdr->ncmds; i++) {
        const struct load_command *cmd = (struct load_command *)q;
        if (cmd->cmd == LC_SEGMENT_64) {
            const struct segment_command_64 *seg = (struct segment_command_64 *)q;
            if (min > seg->vmaddr && seg->vmsize > 0) {
                min = seg->vmaddr;
            }
            if (max < seg->vmaddr + seg->vmsize && seg->vmsize > 0) {
                max = seg->vmaddr + seg->vmsize;
            }
            if (!strcmp(seg->segname, "__TEXT_EXEC")) {
                xnucore_base = seg->vmaddr;
                xnucore_size = seg->filesize;
            } else if (!strcmp(seg->segname, "__PPLTEXT")) {
                ppl_base = seg->vmaddr;
                ppl_size = seg->filesize;
            } else if (!strcmp(seg->segname, "__PLK_TEXT_EXEC")) {
                prelink_base = seg->vmaddr;
                prelink_size = seg->filesize;
            } else if (!strcmp(seg->segname, "__TEXT")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__cstring")) {
                        cstring_base = sec[j].addr;
                        cstring_size = sec[j].size;
                    } else if (!strcmp(sec[j].sectname, "__os_log")) {
                        oslstring_base = sec[j].addr;
                        oslstring_size = sec[j].size;
                    } else if (!strcmp(sec[j].sectname, "__const")) {
                        const_base = sec[j].addr;
                        const_size = sec[j].size;
                    }
                }
            } else if (!strcmp(seg->segname, "__PRELINK_TEXT")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__text")) {
                        pstring_base = sec[j].addr;
                        pstring_size = sec[j].size;
                    }
                }
            } else if (!strcmp(seg->segname, "__DATA_CONST")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__const")) {
                        data_const_base = sec[j].addr;
                        data_const_size = sec[j].size;
                    }
                }
            } else if (!strcmp(seg->segname, "__DATA")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__data")) {
                        data_base = sec[j].addr;
                        data_size = sec[j].size;
                    }
                }
            }
        } else if (cmd->cmd == LC_UNIXTHREAD) {
            uint32_t *ptr = (uint32_t *)(cmd + 1);
            uint32_t flavor = ptr[0];
            struct {
                uint64_t x[29];	/* General purpose registers x0-x28 */
                uint64_t fp;	/* Frame pointer x29 */
                uint64_t lr;	/* Link register x30 */
                uint64_t sp;	/* Stack pointer x31 */
                uint64_t pc; 	/* Program counter */
                uint32_t cpsr;	/* Current program status register */
            } *thread = (void *)(ptr + 2);
            if (flavor == 6) {
                kernel_entry = thread->pc;
            }
        }
        q = q + cmd->cmdsize;
    }

    if (prelink_size == 0) {
        monolithic_kernel = true;
        prelink_base = xnucore_base;
        prelink_size = xnucore_size;
        pstring_base = cstring_base;
        pstring_size = cstring_size;
    }

    kerndumpbase = min;
    xnucore_base -= kerndumpbase;
    prelink_base -= kerndumpbase;
    cstring_base -= kerndumpbase;
    ppl_base -= kerndumpbase;
    pstring_base -= kerndumpbase;
    oslstring_base -= kerndumpbase;
    data_const_base -= kerndumpbase;
    data_base -= kerndumpbase;
    const_base -= kerndumpbase;
    kernel_size = max - min;

    if (filename == NULL) {
        kernel = malloc(kernel_size);
        if (!kernel) {
            return -1;
        }
        rv = kread(kerndumpbase, kernel, kernel_size);
        if (rv != kernel_size) {
            free(kernel);
            kernel = NULL;
            return -1;
        }

        kernel_mh = kernel + kernel_base - min;
    } else {
        kernel = calloc(1, kernel_size);
        if (!kernel) {
            CLOSE(fd);
            return -1;
        }

        q = buf + sizeof(struct mach_header) + is64;
        for (i = 0; i < hdr->ncmds; i++) {
            const struct load_command *cmd = (struct load_command *)q;
            if (cmd->cmd == LC_SEGMENT_64) {
                const struct segment_command_64 *seg = (struct segment_command_64 *)q;
                size_t sz = PREAD(fd, kernel + seg->vmaddr - min, seg->filesize, seg->fileoff);
                if (sz != seg->filesize) {
                    CLOSE(fd);
                    free(kernel);
                    kernel = NULL;
                    return -1;
                }
                if (!kernel_mh) {
                    kernel_mh = kernel + seg->vmaddr - min;
                }
                if (!strcmp(seg->segname, "__PPLDATA")) {
                    auth_ptrs = true;
                } else if (!strcmp(seg->segname, "__LINKEDIT")) {
                    kernel_delta = seg->vmaddr - min - seg->fileoff;
                }
            }
            q = q + cmd->cmdsize;
        }

        CLOSE(fd);
    }
    return 0;
}

void
term_kernel(void)
{
    if (kernel != NULL) {
        free(kernel);
        kernel = NULL;
    }
}

addr_t
find_register_value(addr_t where, int reg)
{
    addr_t val;
    addr_t bof = 0;
    where -= kerndumpbase;
    if (where > xnucore_base) {
        bof = bof64(kernel, xnucore_base, where);
        if (!bof) {
            bof = xnucore_base;
        }
    } else if (where > prelink_base) {
        bof = bof64(kernel, prelink_base, where);
        if (!bof) {
            bof = prelink_base;
        }
    }
    val = calc64(kernel, bof, where, reg);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_reference(addr_t to, int n, enum text_bases text_base)
{
    addr_t ref, end;
    addr_t base = xnucore_base;
    addr_t size = xnucore_size;
    switch (text_base) {
        case text_xnucore_base:
            break;
        case text_prelink_base:
            if (prelink_base) {
                base = prelink_base;
                size = prelink_size;
            }
            break;
        case text_ppl_base:
            if (ppl_base != 0-kerndumpbase) {
                base = ppl_base;
                size = ppl_size;
            }
            break;
        default:
            printf("Unknown base %d\n", text_base);
            return 0;
            break;
    }
    if (n <= 0) {
        n = 1;
    }
    end = base + size;
    to -= kerndumpbase;
    do {
        ref = xref64(kernel, base, end, to);
        if (!ref) {
            return 0;
        }
        base = ref + 4;
    } while (--n > 0);
    return ref + kerndumpbase;
}

addr_t
find_strref(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base)
{
    uint8_t *str;
    addr_t base;
    addr_t size;
    enum text_bases text_base = ppl_base?text_ppl_base:text_xnucore_base;

    switch (string_base) {
        case string_base_const:
            base = const_base;
            size = const_size;
            break;
        case string_base_data:
            base = data_base;
            size = data_size;
            break;
        case string_base_oslstring:
            base = oslstring_base;
            size = oslstring_size;
            break;
        case string_base_pstring:
            base = pstring_base;
            size = pstring_size;
            text_base = text_prelink_base;
            break;
        case string_base_cstring:
        default:
            base = cstring_base;
            size = cstring_size;
            break;
    }
    addr_t off = 0;
    while ((str = boyermoore_horspool_memmem(kernel + base + off, size - off, (uint8_t *)string, strlen(string)))) {
        // Only match the beginning of strings
        if ((str == kernel + base || *(str-1) == '\0') && (!full_match || strcmp((char *)str, string) == 0))
            break;
        off = str - (kernel + base) + 1;
    }
    if (!str) {
        return 0;
    }
    return find_reference(str - kernel + kerndumpbase, n, text_base);
}

addr_t
find_gPhysBase(void)
{
    addr_t ret, val;
    addr_t ref = find_strref("\"pmap_map_high_window_bd: insufficient pages", 1, string_base_cstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    ret = step64(kernel, ref, 64, INSN_RET);
    if (!ret) {
        // iOS 11
        ref = step64(kernel, ref, 1024, INSN_RET);
        if (!ref) {
            return 0;
        }
        ret = step64(kernel, ref + 4, 64, INSN_RET);
        if (!ret) {
            return 0;
        }
    }
    val = calc64(kernel, ref, ret, 8);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_kernel_pmap(void)
{
    addr_t call, bof, val;
    addr_t ref = find_strref("\"pmap_map_bd\"", 1, string_base_cstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    call = step64_back(kernel, ref, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    bof = bof64(kernel, xnucore_base, call);
    if (!bof) {
        return 0;
    }
    val = calc64(kernel, bof, call, 2);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_amfiret(void)
{
    addr_t ret;
    addr_t ref = find_strref("AMFI: hook..execve() killing pid %u: %s\n", 1, string_base_pstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    ret = step64(kernel, ref, 512, INSN_RET);
    if (!ret) {
        return 0;
    }
    return ret + kerndumpbase;
}

addr_t
find_ret_0(void)
{
    addr_t off;
    uint32_t *k;
    k = (uint32_t *)(kernel + xnucore_base);
    for (off = 0; off < xnucore_size - 4; off += 4, k++) {
        if (k[0] == 0xAA1F03E0 && k[1] == 0xD65F03C0) {
            return off + xnucore_base + kerndumpbase;
        }
    }
    k = (uint32_t *)(kernel + prelink_base);
    for (off = 0; off < prelink_size - 4; off += 4, k++) {
        if (k[0] == 0xAA1F03E0 && k[1] == 0xD65F03C0) {
            return off + prelink_base + kerndumpbase;
        }
    }
    return 0;
}

addr_t
find_amfi_memcmpstub(void)
{
    addr_t call, dest, reg;
    addr_t ref = find_strref("%s: Possible race detected. Rejecting.", 1, string_base_pstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    call = step64_back(kernel, ref, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    dest = follow_call64(kernel, call);
    if (!dest) {
        return 0;
    }
    reg = calc64(kernel, dest, dest + 8, 16);
    if (!reg) {
        return 0;
    }
    return reg + kerndumpbase;
}

addr_t
find_sbops(void)
{
    addr_t off, what;
    uint8_t *str = boyermoore_horspool_memmem(kernel + pstring_base, pstring_size, (uint8_t *)"Seatbelt sandbox policy", sizeof("Seatbelt sandbox policy") - 1);
    if (!str) {
        return 0;
    }
    what = str - kernel + kerndumpbase;
    for (off = 0; off < kernel_size - prelink_base; off += 8) {
        if (*(uint64_t *)(kernel + prelink_base + off) == what) {
            return *(uint64_t *)(kernel + prelink_base + off + 24);
        }
    }
    return 0;
}

addr_t
find_lwvm_mapio_patch(void)
{
    addr_t call, dest, reg;
    addr_t ref = find_strref("_mapForIO", 1, string_base_pstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    call = step64(kernel, ref, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    call = step64(kernel, call + 4, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    dest = follow_call64(kernel, call);
    if (!dest) {
        return 0;
    }
    reg = calc64(kernel, dest, dest + 8, 16);
    if (!reg) {
        return 0;
    }
    return reg + kerndumpbase;
}

addr_t
find_lwvm_mapio_newj(void)
{
    addr_t call;
    addr_t ref = find_strref("_mapForIO", 1, string_base_pstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    call = step64(kernel, ref, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    call = step64(kernel, call + 4, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    call = step64(kernel, call + 4, 64, INSN_CALL);
    if (!call) {
        return 0;
    }
    call = step64_back(kernel, call, 64, INSN_B);
    if (!call) {
        return 0;
    }
    return call + 4 + kerndumpbase;
}

addr_t
find_cpacr_write(void)
{
    addr_t off;
    uint32_t *k;
    k = (uint32_t *)(kernel + xnucore_base);
    for (off = 0; off < xnucore_size - 4; off += 4, k++) {
        if (k[0] == 0xd5181040) {
            return off + xnucore_base + kerndumpbase;
        }
    }
    return 0;
}

addr_t
find_str(const char *string)
{
    uint8_t *str = boyermoore_horspool_memmem(kernel, kernel_size, (uint8_t *)string, strlen(string));
    if (!str) {
        return 0;
    }
    return str - kernel + kerndumpbase;
}

addr_t
find_entry(void)
{
    /* XXX returns an unslid address */
    return kernel_entry;
}

const unsigned char *
find_mh(void)
{
    return kernel_mh;
}

addr_t
find_amfiops(void)
{
    addr_t off, what;
    uint8_t *str = boyermoore_horspool_memmem(kernel + pstring_base, pstring_size, (uint8_t *)"Apple Mobile File Integrity", sizeof("Apple Mobile File Integrity") - 1);
    if (!str) {
        return 0;
    }
    what = str - kernel + kerndumpbase;
    /* XXX will only work on a dumped kernel */
    for (off = 0; off < kernel_size - prelink_base; off += 8) {
        if (*(uint64_t *)(kernel + prelink_base + off) == what) {
            return *(uint64_t *)(kernel + prelink_base + off + 0x18);
        }
    }
    return 0;
}

addr_t
find_sysbootnonce(void)
{
    addr_t off, what;
    uint8_t *str = boyermoore_horspool_memmem(kernel + cstring_base, cstring_size, (uint8_t *)"com.apple.System.boot-nonce", sizeof("com.apple.System.boot-nonce") - 1);
    if (!str) {
        return 0;
    }
    what = str - kernel + kerndumpbase;
    for (off = 0; off < kernel_size - xnucore_base; off += 8) {
        if (*(uint64_t *)(kernel + xnucore_base + off) == what) {
            return xnucore_base + off + 8 + 4 + kerndumpbase;
        }
    }
    return 0;
}

addr_t
find_trustcache(void)
{
    if (auth_ptrs) {
        addr_t ref = find_strref("\"loadable trust cache buffer too small (%ld) for entries claimed (%d)\"", 1, string_base_cstring, false, true);
        if (!ref) return 0;
    
        ref -= kerndumpbase;

        addr_t val = calc64(kernel, ref-32*4, ref-24*4, 8);
        if (!val) return 0;

        return val + kerndumpbase;
    }

    addr_t cbz, call, func, val, adrp;
    int reg;
    uint32_t op;

    addr_t ref = find_strref("%s: only allowed process can check the trust cache", 1, string_base_pstring, false, false); // Trying to find AppleMobileFileIntegrityUserClient::isCdhashInTrustCache
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    call = step64_back(kernel, ref, 11 * 4, INSN_CALL);
    if (!call) {
        return 0;
    }
    func = follow_call64(kernel, call);
    if (!func) {
        return 0;
    }
    call = step64(kernel, func, 8 * 4, INSN_CALL);
    if (!call) {
        return 0;
    }
    func = follow_call64(kernel, call);
    if (!func) {
        return 0;
    }
    call = step64(kernel, func, 8 * 4, INSN_CALL);
    if (!call) {
        return 0;
    }
    call = step64(kernel, call + 4, 8 * 4, INSN_CALL);
    if (!call) {
        return 0;
    }
    func = follow_call64(kernel, call);
    if (!func) {
        return 0;
    }

    call = step64(kernel, func, 12 * 4, INSN_CALL);
    if (!call) {
        return 0;
    }

    val = calc64(kernel, call, call + 6 * 4, 21);
    if (!val) {
        func = follow_stub(kernel, call);
        if (!func) return 0;
        func -=  kerndumpbase;
        addr_t movw = step64(kernel, func, 0x300, 0x52800280, 0xffffffe0);
        if (!movw) return 0;
        adrp = step64_back(kernel, movw, 0x10, INSN_ADRP);
        if (!adrp) return 0;
        op = *(uint32_t*)(kernel + adrp + 4);
        reg = op&0x1F;
        val = calc64(kernel, adrp, movw, reg);
        if (!val) return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_amficache(void)
{
    addr_t cbz, call, func, val;
    addr_t ref = find_strref("amfi_prevent_old_entitled_platform_binaries", 1, string_base_pstring, false, false);
    if (!ref) {
        // iOS 11
        ref = find_strref("com.apple.MobileFileIntegrity", 1, string_base_pstring, false, false);
        if (!ref) {
            return 0;
        }
        ref -= kerndumpbase;
        call = step64(kernel, ref, 64, INSN_CALL);
        if (!call) {
            return 0;
        }
        call = step64(kernel, call + 4, 64, INSN_CALL);
        goto okay;
    }
    ref -= kerndumpbase;
    cbz = step64(kernel, ref, 32, INSN_CBZ);
    if (!cbz) {
        return 0;
    }
    call = step64(kernel, follow_cbz(kernel, cbz), 4, INSN_CALL);
okay:
    if (!call) {
        return 0;
    }
    func = follow_call64(kernel, call);
    if (!func) {
        return 0;
    }
    val = calc64(kernel, func, func + 16, 8);
    if (!val) {
        ref = find_strref("%s: only allowed process can check the trust cache", 1, string_base_pstring, false, false); // Trying to find AppleMobileFileIntegrityUserClient::isCdhashInTrustCache
        if (!ref) {
            return 0;
        }
        ref -= kerndumpbase;
        call = step64_back(kernel, ref, 11 * 4, INSN_CALL);
        if (!call) {
            return 0;
        }
        func = follow_call64(kernel, call);
        if (!func) {
            return 0;
        }
        call = step64(kernel, func, 8 * 4, INSN_CALL);
        if (!call) {
            return 0;
        }
        func = follow_call64(kernel, call);
        if (!func) {
            return 0;
        }
        call = step64(kernel, func, 8 * 4, INSN_CALL);
        if (!call) {
            return 0;
        }
        call = step64(kernel, call + 4, 8 * 4, INSN_CALL);
        if (!call) {
            return 0;
        }
        func = follow_call64(kernel, call);
        if (!func) {
            return 0;
        }
        call = step64(kernel, func, 12 * 4, INSN_CALL);
        if (!call) {
            return 0;
        }
        
        val = calc64(kernel, call, call + 6 * 4, 21);
    }
    return val + kerndumpbase;
}

/* extra_recipe **************************************************************/

#define INSN_STR8 0xF9000000 | 8, 0xFFC00000 | 0x1F
#define INSN_POPS 0xA9407BFD, 0xFFC07FFF

addr_t
find_AGXCommandQueue_vtable(void)
{
    addr_t val, str8;
    addr_t ref = find_strref("AGXCommandQueue", 1, string_base_pstring, false, false);
    if (!ref) {
        return 0;
    }
    val = find_register_value(ref, 0);
    if (!val) {
        return 0;
    }
    ref = find_reference(val, 1, 1);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    str8 = step64(kernel, ref, 32, INSN_STR8);
    if (!str8) {
        return 0;
    }
    val = calc64(kernel, ref, str8, 8);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_allproc(void)
{
    addr_t val, bof, str8;
    addr_t ref = find_strref("\"pgrp_add : pgrp is dead adding process\"", 1, string_base_cstring, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    bof = bof64(kernel, xnucore_base, ref);
    if (!bof) {
        return 0;
    }
    str8 = step64_back(kernel, ref, ref - bof, INSN_STR8);
    if (!str8) {
        // iOS 11
        addr_t ldp = step64(kernel, ref, 1024, INSN_POPS);
        if (!ldp) {
            return 0;
        }
        str8 = step64_back(kernel, ldp, ldp - bof, INSN_STR8);
        if (!str8) {
            return 0;
        }
    }
    val = calc64(kernel, bof, str8, 8);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

addr_t
find_call5(void)
{
    addr_t bof;
    uint8_t gadget[] = { 0x95, 0x5A, 0x40, 0xF9, 0x68, 0x02, 0x40, 0xF9, 0x88, 0x5A, 0x00, 0xF9, 0x60, 0xA2, 0x40, 0xA9 };
    uint8_t *str = boyermoore_horspool_memmem(kernel + prelink_base, prelink_size, gadget, sizeof(gadget));
    if (!str) {
        return 0;
    }
    bof = bof64(kernel, prelink_base, str - kernel);
    if (!bof) {
        return 0;
    }
    return bof + kerndumpbase;
}

addr_t
find_realhost(addr_t priv)
{
    addr_t val;
    if (!priv) {
        return 0;
    }
    priv -= kerndumpbase;
    val = calc64(kernel, priv, priv + 12, 0);
    if (!val) {
        return 0;
    }
    return val + kerndumpbase;
}

/*
 *
 * @ninjaprawn's patches
 *
 */ 

addr_t find_vfs_context_current(void) {
    addr_t str = find_strref("/private/var/tmp/wav%u_%uchans.wav", 1, string_base_pstring, false, false);
    if (!str) return 0;
    str -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, str);
    if (!func) return 0;

    addr_t call = step64(kernel, func, 0x100, INSN_CALL);
    if (!call) return 0;
        
    return follow_stub(kernel, call);
}

addr_t find_vnode_lookup(void) {
    addr_t hfs_str = find_strref("hfs: journal open cb: error %d looking up device %s (dev uuid %s)\n", 1, string_base_pstring, false, false);
    if (!hfs_str) return 0;
    
    hfs_str -= kerndumpbase;

    addr_t call_to_stub = step64_back(kernel, hfs_str, 10*4, INSN_CALL);
    if (!call_to_stub) return 0;
    
    return follow_stub(kernel, call_to_stub);
}

addr_t find_vnode_put(void) {
    addr_t hook_mount_check_snapshot_revert = find_hook_mount_check_snapshot_revert();
    if (!hook_mount_check_snapshot_revert) return 0;
    hook_mount_check_snapshot_revert -= kerndumpbase;

    addr_t call = hook_mount_check_snapshot_revert;
    for (int i=0; i<4; i++) {
        call = step64(kernel, call+4, 0x50, INSN_CALL);
        if (!call) return 0;
    }

    return follow_stub(kernel, call);
}

addr_t find_vnode_getfromfd(void) {
    addr_t syscall_check_sandbox = find_syscall_check_sandbox();
    if (!syscall_check_sandbox) return 0;
    syscall_check_sandbox -= kerndumpbase;

    addr_t ldr = step64(kernel, syscall_check_sandbox+4, 0x1000, 0xB94003E1, 0xFFE003FF);
    if (!ldr) return 0;

    addr_t call = step64(kernel, ldr+4, 0x10, INSN_CALL);
    if (!call) return 0;
    return follow_stub(kernel, call);
}

addr_t find_vnode_getattr(void) {
    addr_t error_str = find_strref("\"add_fsevent: you can't pass me a NULL vnode ptr (type %d)!\\n\"", 1, string_base_cstring, false, false);
    
    if (!error_str) {
        return 0;
    }
    
    error_str -= kerndumpbase;
    error_str += 12; // Jump over the panic call

    addr_t call_to_target = step64(kernel, error_str, 30*4, INSN_CALL);
    
    if (!call_to_target) {
        return 0;
    }
    
    addr_t offset_to_target = follow_call64(kernel, call_to_target);
    
    if (!offset_to_target) {
        return 0;
    }

    return offset_to_target + kerndumpbase;
}

addr_t find_SHA1Init(void) {
    addr_t id_str = find_strref("CrashReporter-ID", 1, string_base_pstring, false, false);
    
    if (!id_str) {
        return 0;
    }
    
    id_str -= kerndumpbase;

    addr_t call_to_hash_function = step64(kernel, id_str, 10*4, INSN_CALL);
    
    if (!call_to_hash_function) {
        return 0;
    }
    
    addr_t hash_function = follow_call64(kernel, call_to_hash_function);
    
    if (!hash_function) {
        return 0;
    }
    
    addr_t call_to_stub = step64(kernel, hash_function, 20*4, INSN_CALL);
    if (!call_to_stub) return 0;

    return follow_stub(kernel, call_to_stub);
}

addr_t find_SHA1Update(void) {
    addr_t id_str = find_strref("CrashReporter-ID", 1, string_base_pstring, false, false);
    if (!id_str) {
        return 0;
    }

    id_str -= kerndumpbase;

    addr_t call_to_hash_function = step64(kernel, id_str, 10*4, INSN_CALL);
    
    if (!call_to_hash_function) {
        return 0;
    }
    
    addr_t hash_function = follow_call64(kernel, call_to_hash_function);
    
    if (!hash_function) {
        return 0;
    }
    
    addr_t call_to_sha1init = step64(kernel, hash_function, 20*4, INSN_CALL);
    
    if (!call_to_sha1init) {
        return 0;
    }
    
    addr_t call_to_stub = step64(kernel, call_to_sha1init + 4, 20*4, INSN_CALL);
    if (!call_to_stub) return 0;

    return follow_stub(kernel, call_to_stub);
}


addr_t find_SHA1Final(void) {
    addr_t id_str = find_strref("CrashReporter-ID", 1, string_base_pstring, false, false);
    
    if (!id_str) {
        return 0;
    }
    
    id_str -= kerndumpbase;

    addr_t call_to_hash_function = step64(kernel, id_str, 10*4, INSN_CALL);
    
    if (!call_to_hash_function) {
        return 0;
    }
    
    addr_t hash_function = follow_call64(kernel, call_to_hash_function);
    
    if (!hash_function) {
        return 0;
    }
    
    addr_t call_to_sha1init = step64(kernel, hash_function, 20*4, INSN_CALL);
    
    if (!call_to_sha1init) {
        return 0;
    }
    
    addr_t call_to_sha1update = step64(kernel, call_to_sha1init + 4, 20*4, INSN_CALL);
    
    if (!call_to_sha1update) {
        return 0;
    }
    
    addr_t call_to_stub = step64(kernel, call_to_sha1update + 4, 20*4, INSN_CALL);
    if (!call_to_stub) return 0;

    return follow_stub(kernel, call_to_stub);
}

addr_t find_csblob_entitlements_dictionary_set(void) {
    addr_t ent_str = find_strref("entitlements are not a dictionary", 1, string_base_pstring, false, false);
    
    if (!ent_str) {
        return 0;
    }
    
    ent_str -= kerndumpbase;

    addr_t call_to_lck_mtx_lock = step64(kernel, ent_str, 20*4, INSN_CALL);
    
    if (!call_to_lck_mtx_lock) {
        return 0;
    }
    
    addr_t call_to_csblob_entitlements_dictionary_copy = step64(kernel, call_to_lck_mtx_lock + 4, 20*4, INSN_CALL);
    
    if (!call_to_csblob_entitlements_dictionary_copy) {
        return 0;
    }
    
    addr_t call_to_stub = step64(kernel, call_to_csblob_entitlements_dictionary_copy + 4, 20*4, INSN_CALL);
    if (!call_to_stub) return 0;

    return follow_stub(kernel, call_to_stub);
}

addr_t find_kernel_task(void) {
    if (monolithic_kernel) {
        addr_t str = find_strref("\"shouldn't be applying exception notification", 2, string_base_cstring, false, false);
        if (!str) return 0;
        str -= kerndumpbase;

        addr_t call = step64_back(kernel, str, 0x10, INSN_CALL);
        if (!call) return 0;

        addr_t task_suspend = follow_call64(kernel, call);
        if (!task_suspend) return 0;

        addr_t adrp = step64(kernel, task_suspend, 20*4, INSN_ADRP);
        if (!adrp) return 0;

        addr_t kern_task = calc64(kernel, adrp, adrp + 0x8, 8);
        if (!kern_task) return 0;

        return kern_task + kerndumpbase;
    }

    addr_t term_str = find_strref("\"thread_terminate\"", 1, string_base_cstring, false, false);
    
    if (!term_str) {
        return 0;
    }
    
    term_str -= kerndumpbase;

    addr_t thread_terminate = bof64(kernel, xnucore_base, term_str);
    
    if (!thread_terminate) {
        return 0;
    }
    
    addr_t call_to_unk1 = step64(kernel, thread_terminate, 20*4, INSN_CALL);
    
    if (!call_to_unk1) {
        return 0;
    }

    addr_t kern_task = calc64(kernel, thread_terminate, call_to_unk1, 9);
    
    if (!kern_task) {
        return 0;
    }
    
    return kern_task + kerndumpbase;
}


addr_t find_kernproc(void) {
    addr_t ret_str = find_strref("\"returning child proc which is not cur_act\"", 1, string_base_cstring, false, false);
    
    if (!ret_str) {
        return 0;
    }
    
    ret_str -= kerndumpbase;

    addr_t end;
    int reg = 0;
    if (monolithic_kernel) {
        addr_t adrp = step64(kernel, ret_str, 20*4, INSN_ADRP);
        if (!adrp) return 0;
        uint32_t op = *(uint32_t*)(kernel + adrp + 4);
        reg = op & 0x1f;

        end = step64(kernel, adrp, 20*4, INSN_CALL);
        if (!end) return 0;
    } else {
        reg = 19;
        end = step64(kernel, ret_str, 20*4, INSN_RET);

        if (!end) {
            return 0;
        }
    }

    addr_t kernproc = calc64(kernel, ret_str, end, reg);
    
    if (!kernproc) {
        return 0;
    }
    
    return kernproc + kerndumpbase;
}

addr_t find_vnode_recycle(void) {
    addr_t error_str = find_strref("\"vnode_put(%p): iocount < 1\"", 1, string_base_cstring, false, false);
    
    if (!error_str) {
        return 0;
    }
    
    error_str -= kerndumpbase;
    
    if (monolithic_kernel) {
        addr_t tbnz = step64(kernel, error_str, 0x400, 0x37100000, 0xFFF80000);
        if (!tbnz) return 0;

        addr_t call_to_target = step64(kernel, tbnz + 4, 40*4, INSN_CALL);
        if (!call_to_target) return 0;

        addr_t func = follow_call64(kernel, call_to_target);
        if (!func) return 0;
        return func + kerndumpbase;
    }

    addr_t call_to_lck_mtx_unlock = step64(kernel, error_str + 4, 40*4, INSN_CALL);
    
    if (!call_to_lck_mtx_unlock) {
        return 0;
    }
    
    addr_t call_to_unknown1 = step64(kernel, call_to_lck_mtx_unlock + 4, 40*4, INSN_CALL);
    
    if (!call_to_unknown1) {
        return 0;
    }
    
    addr_t offset_to_unknown1 = follow_call64(kernel, call_to_unknown1);
    
    if (!offset_to_unknown1) {
        return 0;
    }
    
    addr_t call_to_target = step64(kernel, offset_to_unknown1 + 4, 40*4, INSN_CALL);
    
    if (!call_to_target) {
        return 0;
    }
    
    addr_t offset_to_target = follow_call64(kernel, call_to_target);
    
    if (!offset_to_target) {
        return 0;
    }

    return offset_to_target + kerndumpbase;
}

addr_t find_lck_mtx_lock(void) {
    addr_t strref = find_strref("nxprov_detacher", 1, string_base_cstring, false, false);
    if (!strref) return 0;
    
    strref -= kerndumpbase;

    addr_t call_to_target = step64_back(kernel, strref - 4, 0x10, INSN_CALL);
    if (!call_to_target) return 0;

    addr_t offset_to_target = follow_call64(kernel, call_to_target);
    if (!offset_to_target) return 0;

    // Did we really find lck_mtx_lock_contended?
    call_to_target = step64_back(kernel, offset_to_target, 0x4, INSN_B);
    if (call_to_target) {
        addr_t target = follow_call64(kernel, call_to_target);
        if (target == offset_to_target) {
            // Nope
            offset_to_target = bof64(kernel, xnucore_base, call_to_target);
            if (!offset_to_target) return 0;
        }
    }


    return offset_to_target + kerndumpbase;
}

addr_t find_lck_mtx_unlock(void) {
    addr_t strref = find_strref("nxprov_detacher", 1, string_base_cstring, false, false);
    if (!strref) return 0;
    
    strref -= kerndumpbase;

    addr_t call = step64(kernel, strref + 4, 0x100, INSN_CALL);
    if (!call) return 0;

    addr_t call_to_target = step64(kernel, call + 4, 0x10, INSN_CALL);
    if (!call_to_target) return 0;

    addr_t offset_to_target = follow_call64(kernel, call_to_target);
    if (!offset_to_target) return 0;

    return offset_to_target + kerndumpbase;
}

addr_t find_strlen(void) {
    addr_t xnu_str = find_strref("AP-xnu", 1, string_base_cstring, false, false);
    
    if (!xnu_str) {
        return 0;
    }
    
    xnu_str -= kerndumpbase;

    addr_t call_to_target = step64(kernel, xnu_str, 40*4, INSN_CALL);
    
    if (!call_to_target) {
        return 0;
    }
    
    addr_t offset_to_target = follow_call64(kernel, call_to_target);
    
    if (!offset_to_target) {
        return 0;
    }

    return offset_to_target + kerndumpbase;
}

addr_t find_add_x0_x0_0x40_ret(void)
{
    addr_t off;
    uint32_t* k;
    k = (uint32_t*)(kernel + xnucore_base);
    for (off = 0; off < xnucore_size - 4; off += 4, k++) {
        if (k[0] == 0x91010000 && k[1] == 0xD65F03C0) {
            return off + xnucore_base + kerndumpbase;
        }
    }
    k = (uint32_t*)(kernel + prelink_base);
    for (off = 0; off < prelink_size - 4; off += 4, k++) {
        if (k[0] == 0x91010000 && k[1] == 0xD65F03C0) {
            return off + prelink_base + kerndumpbase;
        }
    }
    return 0;
}

/*
 *
 * 
 *
 */

/*
 *
 * @Cryptiiiic's patches
 *
 */ 

addr_t find_boottime(void) {
    addr_t ref = find_strref("%s WARNING: PMU offset is less then sys PMU", 1, string_base_oslstring, false, false);
    
    if (!ref) {
        ref = find_strref("%s WARNING: UTC time is less then sys time, (%lu s %d u) UTC (%lu s %d u) sys\n", 1, string_base_oslstring, false, false);
        if (!ref) {
            return 0;
        }
    }
    
    ref -= kerndumpbase;

    // ADRP Xm, #_boottime@PAGE
    ref = step64(kernel, ref, 0x4D, INSN_ADRP);
    
    if (!ref) {
        return 0;
    }

    // pc base
    uint64_t val = kerndumpbase;

    uint32_t* insn = (uint32_t*)(kernel + ref);
    // add pc (well, offset)
    val += ((uint8_t*)(insn)-kernel) & ~0xfff;
    uint8_t xm = *insn & 0x1f;

    // add imm: immhi(bits 23-5)|immlo(bits 30-29)
    val += (*insn << 9 & 0x1ffffc000) | (*insn >> 17 & 0x3000);

    ++insn;
    // STR Xn, [Xm,_boottime@PAGEOFF]
    if ((*insn & 0xF9090000) != 0xF9000000) {
        return 0;
    }
    if (((*insn >> 5) & 0x1f) != xm) {
        return 0;
    }
    // add pageoff
    val += ((*insn >> 10) & 0xFFF) << 3;
    uint8_t xn = (*insn & 0x1f);

    return val;
}

/*
 *
 * 
 *
 */

/*
 *
 * @stek29's patches
 *
 */

addr_t find_zone_map_ref(void)
{
    // \"Nothing being freed to the zone_map. start = end = %p\\n\"
    uint64_t val = kerndumpbase;
    
    addr_t ref = find_strref("\"Nothing being freed to the zone_map. start = end = %p\\n\"", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    // skip add & adrp for panic str
    ref -= 8;
    
    // adrp xX, #_zone_map@PAGE
    ref = step64_back(kernel, ref, 30, INSN_ADRP);
    
    if (!ref) {
        return 0;
    }
    
    uint32_t* insn = (uint32_t*)(kernel + ref);
    // get pc
    val += ((uint8_t*)(insn)-kernel) & ~0xfff;
    uint8_t xm = *insn & 0x1f;
    
    // don't ask, I wrote this at 5am
    val += (*insn << 9 & 0x1ffffc000) | (*insn >> 17 & 0x3000);
    
    // ldr x, [xX, #_zone_map@PAGEOFF]
    ++insn;
    if ((*insn & 0xF9C00000) != 0xF9400000) {
        return 0;
    }
    
    // xd == xX, xn == xX,
    if ((*insn & 0x1f) != xm || ((*insn >> 5) & 0x1f) != xm) {
        return 0;
    }
    
    val += ((*insn >> 10) & 0xFFF) << 3;
    
    return val;
}

addr_t find_OSBoolean_True(void)
{
    addr_t val;
    addr_t ref = find_strref("Delay Autounload", 2, string_base_cstring, false, false);
    if (!ref) ref = find_strref("Delay Autounload", 1, string_base_cstring, false, false);

    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    
    addr_t weird_instruction = 0;
    for (int i = 4; i < 4 * 0x100; i += 4) {
        uint32_t op = *(uint32_t*)(kernel + ref + i);
        if (op == 0x320003E0) {
            weird_instruction = ref + i;
            break;
        }
    }
    if (!weird_instruction) {
        return 0;
    }
    
    val = calc64(kernel, ref, weird_instruction, 8);
    if (!val) {
        return 0;
    }
    
    return val + kerndumpbase;
}

addr_t find_osunserializexml(void)
{
    addr_t ref = find_strref("OSUnserializeXML: %s near line %d\n", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, xnucore_base, ref);
    
    if (!start) return 0;

    if (monolithic_kernel) {
        ref = find_reference(start + kerndumpbase, 1, false);
        if (!ref) return 0;
        ref -= kerndumpbase;

        start = bof64(kernel, xnucore_base, ref);
        if (!start) return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_smalloc(void)
{
    static uint64_t start=0;
    if (start) return start + kerndumpbase;

    addr_t ref = find_strref("sandbox memory allocation failure", 1, string_base_pstring, false, false);
    if (!ref) ref = find_strref("sandbox memory allocation failure", 1, string_base_oslstring, false, false);
    
    if (!ref) return 0;
    
    ref -= kerndumpbase;
    
    start = bof64(kernel, prelink_base, ref);
    
    if (!start) {
        return 0;
    }

    return start + kerndumpbase;
}

addr_t find_shenanigans(void)
{
    addr_t ref = find_strref("\"shenanigans!", 1, string_base_pstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    // find sb_evaluate
    ref = bof64(kernel, prelink_base, ref);
    //printf("sb_evaluate: 0x%llx\n", ref + kerndumpbase);
    
    if (!ref) {
        return 0;
    }
    
    // ADRP Xm, #_is_kernel_cred_kerncred@PAGE
    ref = step64(kernel, ref, 0x100, INSN_ADRP);
    
    if (!ref) {
        return 0;
    }
    
    // pc base
    uint64_t val = kerndumpbase;
    
    uint32_t* insn = (uint32_t*)(kernel + ref);
    // add pc (well, offset)
    val += ((uint8_t*)(insn)-kernel) & ~0xfff;
    uint8_t xm = *insn & 0x1f;
    
    // add imm: immhi(bits 23-5)|immlo(bits 30-29)
    val += (*insn << 9 & 0x1ffffc000) | (*insn >> 17 & 0x3000);
    
    ++insn;
    // LDR Xn, [Xm,#_is_kernel_cred_kerncred@PAGEOFF]
    if ((*insn & 0xF9C00000) != 0xF9400000) {
        return 0;
    }
    if (((*insn >> 5) & 0x1f) != xm) {
        return 0;
    }
    // add pageoff
    val += ((*insn >> 10) & 0xFFF) << 3;
    uint8_t xn = (*insn & 0x1f);
    
    ++insn;
    // CBNZ Xn, ...
    if ((*insn & 0xFC000000) != 0xB4000000) {
        return 0;
    }
    if ((*insn & 0x1f) != xn) {
        return 0;
    }
    
    return val;
}

/*
 *
 *
 *
 */

addr_t find_unix_syscall(void)
{
    addr_t ref = find_strref("SW_STEP_DEBUG exception thread DebugData is NULL", 1, string_base_cstring, false, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    // Find LDR             X2, [XN,#0x3B0|#0x3B8]
    ref = step64_back(kernel, ref, 0x20, 0xF941D802, 0xFFFFF81F);
    if (!ref) return 0;

    uint64_t call = step64(kernel, ref, 0x20, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

addr_t find_pthread_kext_register(void)
{
    addr_t ref = find_strref("\"Re-initialisation of pthread kext callbacks.\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    uint64_t start = bof64(kernel, xnucore_base, ref);
    if (!start) return 0;
    return start + kerndumpbase;
}

// This can be used to find any functions referenced by struct pthread_callbacks_s
addr_t find_pthread_callbacks(void)
{
    // Cache this one //
    static addr_t addr = 0;
    if (addr) {
        return addr + kerndumpbase;
    }

    addr_t ref = find_pthread_kext_register();
    if (!ref) return 0;
    ref -= kerndumpbase;

    for (int i=0; i<3; i++) {
        ref = step64(kernel, ref+4, 0x30, INSN_ADRP);
        if (!ref) return 0;
    }

    addr = calc64(kernel, ref, ref + 8, 8);
    if (!addr) return 0;
    return addr + kerndumpbase;
}

addr_t find_unix_syscall_return(void)
{
    addr_t pthread_callbacks = find_pthread_callbacks();
    if (!pthread_callbacks) return 0;
    pthread_callbacks -= kerndumpbase;

    addr_t addr = *(uint64_t*)(kernel + pthread_callbacks + 8 + 0x31 * 8);
    if (!addr) return 0;
    return remove_pac(addr);
}

addr_t find_kmod_start(void)
{
    addr_t ref = find_strref("kern_return_t kmod_start(kmod_info_t *, void *)", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_handler_map(void)
{
    static addr_t addr = 0;
    if (addr) return addr + kerndumpbase;

    addr_t kmod_start = find_kmod_start();
    if (!kmod_start) return 0;
    kmod_start -= kerndumpbase;
    addr_t func = kmod_start;

    addr_t call = step64(kernel, kmod_start, 0x20, INSN_CALL);
    if (call) {
        func = follow_call64(kernel, call);
        if (!func) return 0;
    }

    // ADD Xn, Xy, #0x10
    addr_t add = step64(kernel, func, 0x50, 0x91004000, 0xFFFFFC00);
    if (!add) return 0;

    uint32_t insn = *(uint32_t*)(kernel + add);
    int rn = (insn>>5)&0x1f;

    addr = calc64(kernel, add-0x10, add, rn);
    if (!addr) return 0;
    return addr + kerndumpbase;
}

addr_t find_policy_conf(void)
{
    addr_t kmod_start = find_kmod_start();
    if (!kmod_start) return 0;
    kmod_start -= kerndumpbase;

    addr_t eof = step64(kernel, kmod_start, 0x500, 0x910003FF, 0xFF0003FF);
    if (!eof) return 0;

    addr_t insn = step64(kernel, kmod_start, eof-kmod_start, 0xD2800002, 0xFFFFFFFF);
    if (!insn) return 0;

    addr_t ref = step64_back(kernel, insn, 0x10, 0x90000000, 0x9F00001F);
    if (!ref) return 0;
    
    addr_t addr = calc64(kernel, ref, ref+8, 0);
    if (!addr) return 0;
    return addr + kerndumpbase;
}

addr_t find_sandbox_handler(const char *name)
{
    addr_t handler_map = find_handler_map();
    if (!handler_map) return 0;
    handler_map -= kerndumpbase;

    struct {
        const char *name;
        uint64_t func;
        uint64_t unk;
    } *handler = kernel + handler_map;

    addr_t func = 0;
    while (handler->name && handler->func) {
        const char *hname = (char*)remove_pac(handler->name) - (char*)kerndumpbase + (char*)kernel;
        if (strcmp(hname, name) == 0) {
            func = remove_pac(handler->func);
            break;
        }
        handler++;
    }
    return func;
}

addr_t find_issue_extension_for_mach_service(void)
{
    return find_sandbox_handler("com.apple.security.exception.mach-lookup.global-name");
}

addr_t find_issue_extension_for_absolute_path(void)
{
    return find_sandbox_handler("com.apple.security.exception.files.absolute-path.read-only");
}

addr_t find_copy_path_for_vp(void)
{
    addr_t ref = find_strref("unknown path", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;

    return func + kerndumpbase;
}

addr_t find_vn_getpath(void)
{
    addr_t copy_path_for_vp = find_copy_path_for_vp();
    if (!copy_path_for_vp) return 0;
    copy_path_for_vp -= kerndumpbase;

    addr_t call = copy_path_for_vp;
    for (int i=0; i<2; i++) {
        call = step64(kernel, call + 4, 0x100, INSN_CALL);
        if (!call) return 0;
    }

    return follow_stub(kernel, call);
}

addr_t find_IOMalloc(void)
{
    addr_t start = find_smalloc();
    if (!start) return 0;
    start -= kerndumpbase;

    addr_t call = step64(kernel, start + 4, 0x50, INSN_CALL);
    if (!call) return 0;

    return follow_stub(kernel, call);
}

addr_t find_IOFree(void)
{
    addr_t start = find_sfree();
    if (!start) return 0;
    start -= kerndumpbase;

    addr_t call=start;
    for (int i=0; i<2; i++) {
        call = step64(kernel, call + 4, 0x100, INSN_CALL);
        if (!call) return 0;
    }

    return follow_stub(kernel, call);
}

addr_t find_policy_ops(void)
{
    static struct mac_policy_conf *conf = NULL;
    if (!conf) {
        addr_t policy_conf_ref = find_policy_conf();
        if (!policy_conf_ref) return 0;
        conf = (struct mac_policy_conf *)(policy_conf_ref - kerndumpbase + kernel);
    }

    addr_t ops = conf->mpc_ops;
    if (!ops) return 0;
    return remove_pac(ops);
}

addr_t find_mpo_entry(addr_t offset)
{
    addr_t ops = find_policy_ops();
    if (!ops) return 0;
    ops -= kerndumpbase;
    
    addr_t opref = *(addr_t *)(ops + kernel + offset);
    if (!opref) return 0;
    return remove_pac(opref);
}

addr_t find_hook_mount_check_snapshot_revert()
{
    return find_mpo(mount_check_snapshot_revert);
}

addr_t find_hook_policy_syscall(int n)
{
    addr_t policy_syscall = find_mpo(policy_syscall);
    if (!policy_syscall) return 0;
    policy_syscall -= kerndumpbase;

    //uint32_t insn = *(uint32_t *)(kernel + policy_syscall);
    uint32_t insn = step64(kernel, policy_syscall, 0x100, 0x7100003F, 0xFFC003FF);
    if (!insn) return 0;
    int len = (*(int*)(kernel+insn)>>10) & 0xFFF;
    if (n > len) return 0;

    addr_t ref = step64(kernel, insn, 0x20, 0x10000000, 0x9F000000);
    if (!ref) ref = step64(kernel, insn, 0x20, INSN_ADRP);
    if (!ref) return 0;

    int reg = *(uint32_t *)(kernel + ref) & 0x1f;
    addr_t jptbl = calc64(kernel, ref, ref+8, reg);
    if (!jptbl) return 0;

    ref = jptbl + *(int *)(kernel + jptbl + (n<<2));
    addr_t call = step64(kernel, ref, 0x50, INSN_B);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_syscall_set_profile(void)
{
    return find_hook_policy_syscall(0);
}

addr_t find_syscall_check_sandbox(void)
{
    return find_hook_policy_syscall(2);
}

addr_t find_sandbox_set_container_copyin(void)
{
    addr_t syscall_set_profile = find_syscall_set_profile();
    if (!syscall_set_profile) return 0;
    syscall_set_profile -= kerndumpbase;

    // SUB SP, SP #imm
    addr_t next_func = step64(kernel, syscall_set_profile+8, 0x1000, 0x510003FF, 0x7F8003FF);
    if (!next_func) return 0;

    addr_t call = next_func;
    for (int i=0; i<3; i++) {
        call = step64_back(kernel, call-4, call-syscall_set_profile, INSN_CALL);
        if (!call) return 0;
    }

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_platform_set_container(void)
{
    addr_t sandbox_set_container_copyin = find_sandbox_set_container_copyin();
    if (!sandbox_set_container_copyin) return 0;
    sandbox_set_container_copyin -= kerndumpbase;

    addr_t call = sandbox_set_container_copyin;
    for (int i=0; i<3; i++) {
        call = step64(kernel, call+4, 0x100, INSN_CALL);
        if (!call) return 0;
    }

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_extension_create_file(void)
{
    addr_t platform_set_container = find_platform_set_container();
    if (!platform_set_container) return 0;
    platform_set_container -= kerndumpbase;

    addr_t call = platform_set_container;
    for (int i=0; i<2; i++) {
        call = step64(kernel, call+8, 0x100, INSN_CALL);
        if (!call) return 0;
    }
    
    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_extension_add(void)
{
    addr_t platform_set_container = find_platform_set_container();
    if (!platform_set_container) return 0;
    platform_set_container -= kerndumpbase;

    addr_t call = platform_set_container;
    for (int i=0; i<3; i++) {
        call = step64(kernel, call+8, 0x100, INSN_CALL);
        if (!call) return 0;
    }
    
    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_extension_release(void)
{
    addr_t platform_set_container = find_platform_set_container();
    if (!platform_set_container) return 0;
    platform_set_container -= kerndumpbase;

    addr_t call = platform_set_container;
    for (int i=0; i<4; i++) {
        call = step64(kernel, call+8, 0x100, INSN_CALL);
        if (!call) return 0;
    }
    
    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_sfree(void)
{
    static addr_t func=0;
    if (func) return func + kerndumpbase;

    addr_t extension_release = find_extension_release();
    if (!extension_release) return 0;
    extension_release -= kerndumpbase;

    addr_t call = step64(kernel, extension_release+4, 0x100, INSN_CALL);
    if (!call) return 0;

    func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_sb_ustate_create(void)
{
    addr_t ref = find_strref("sb_ustate_create", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_sstrdup(void)
{
    addr_t sb_ustate_create = find_sb_ustate_create();
    if (!sb_ustate_create) return 0;
    sb_ustate_create -= kerndumpbase;

    addr_t call = sb_ustate_create;
    for (int i=0; i<2; i++) {
        call = step64(kernel, call+4, 0x50, INSN_CALL);
        if (!call) return 0;
    }

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_sysent(void)
{
    static addr_t sysent = 0;
    if (sysent) return sysent + kerndumpbase;

    addr_t unix_syscall_return = find_unix_syscall_return();
    if (!unix_syscall_return) return 0;
    unix_syscall_return -= kerndumpbase;

    addr_t csel = step64(kernel, unix_syscall_return, 0x200, 0x1A800000, 0x7FE00C00);
    if (!csel) return 0;

    int reg = (*(uint64_t *)(kernel + csel) >> 16) & 0x1F;

    sysent = calc64(kernel, unix_syscall_return, csel-12, reg);
    if (!sysent) return 0;

    return sysent + kerndumpbase;
}

addr_t find_syscall(int n)
{
    addr_t sysent = find_sysent();
    if (!sysent) return 0;
    sysent -= kerndumpbase;

    addr_t syscall = *(addr_t*)(kernel + sysent + 3 * sizeof(addr_t) * n);
    if (!syscall) return 0;
    return remove_pac(syscall);
}

addr_t find_proc_find(void) {
    addr_t getpgid = find_syscall(SYS_getpgid);
    if (!getpgid) return 0;
    getpgid -= kerndumpbase;

    addr_t call = step64(kernel, getpgid, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_proc_rele(void) {
    addr_t getpgid = find_syscall(SYS_getpgid);
    if (!getpgid) return 0;
    getpgid -= kerndumpbase;

    addr_t call = step64(kernel, getpgid, 0x50, INSN_CALL);
    if (!call) return 0;

    call = step64(kernel, call+4, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

/*
 *
 * @pwn20wnd's patches
 *
 */

addr_t find_move_snapshot_to_purgatory(void)
{
    addr_t ref = find_strref("move_snapshot_to_purgatory", 1, string_base_pstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, prelink_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_chgproccnt(void)
{
    addr_t ref = find_strref("\"chgproccnt: lost user\"", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, xnucore_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_kauth_cred_ref(void)
{
    addr_t ref = find_strref("\"kauth_cred_ref: trying to take a reference on a cred with no references\"", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, xnucore_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_apfs_jhash_getvnode(void)
{
    addr_t ref = find_strref("apfs_jhash_getvnode", 1, string_base_pstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, prelink_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_fs_lookup_snapshot_metadata_by_name() {
    uint64_t ref = find_strref("%s:%d: fs_rename_snapshot('%s', %u, '%s', %u) returned %d", 1, string_base_pstring, false, false), func = 0, call = 0;
    if (!ref) return 0;

    ref -= kerndumpbase;

    for (int i = 0; i < 11; i++) {
        call = step64_back(kernel, ref, 256, INSN_CALL);
        if (!call) return 0;

        ref = call - 4;
    }

    func = follow_call64(kernel, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

addr_t find_fs_lookup_snapshot_metadata_by_name_and_return_name() {
    uint64_t ref = find_strref("%s:%d: fs_rename_snapshot('%s', %u, '%s', %u) returned %d", 1, string_base_pstring, false, false), func = 0, call = 0;
    if (!ref) return 0;
   
    ref -= kerndumpbase;
   
    for (int i = 0; i < 7; i++) {
        call = step64_back(kernel, ref, 256, INSN_CALL);
        if (!call) return 0;
       
        ref = call - 4;
    }
   
    func = follow_call64(kernel, call);
    if (!func) return 0;

#ifdef HAVE_MAIN
    // Verify we got the right function
    uint64_t sub = find_fs_lookup_snapshot_metadata_by_name();
    if (!sub) return 0;

    call = step64(kernel, ref, 256, INSN_CALL);
    if (!call) return 0;

    if (follow_call64(kernel, call) != func) return 0;
#endif

    return func + kerndumpbase;
}

addr_t find_mount_common() {
    uint64_t ref = find_strref("\"mount_common():", 1, string_base_cstring, false, false);
    if (!ref) return 0;
    ref -= kerndumpbase;
    uint64_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}


addr_t find_fs_snapshot() {
    uint64_t mount_common = find_mount_common();
    if (!mount_common) return 0;

    uint64_t ref = find_reference(mount_common, 5, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    uint64_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

addr_t find_vnode_get_snapshot() {
    uint64_t fs_snapshot = find_fs_snapshot();
    if (!fs_snapshot) return 0;
    fs_snapshot -= kerndumpbase;

    uint64_t call = step64(kernel, fs_snapshot+4, 0x400, 0xAA0003E6, 0xFFE0FFFF);
    if (!call) {
        return 0;
    }
    call += 4;
    uint64_t func = follow_call64(kernel, call);
    if (!func) return 0;

#ifdef HAVE_MAIN
    // Verification

    int i=0;
    uint64_t ref;
    while ((ref = find_reference(func + kerndumpbase, i+1, false))) {
        if (bof64(kernel, xnucore_base, ref - kerndumpbase) != fs_snapshot) {
            return 0;
        }
        i++;
    }
    if (i==0) return 0;
#endif

    return func + kerndumpbase;
}

addr_t find_pmap_load_trust_cache() {
    if (auth_ptrs) {
        addr_t ref = find_strref("%s: trust cache already loaded, ignoring", 2, 0, false, false);
        if (!ref) ref = find_strref("%s: trust cache already loaded, ignoring", 1, 0, false, false);
        if (!ref) return 0;
        
        ref -= kerndumpbase;
        
        addr_t func = step64_back(kernel, ref, 200, INSN_CALL);
        if (!func) return 0;
        
        func -= 4;
        
        func = step64_back(kernel, func, 200, INSN_CALL);
        if (!func) return 0;
        
        func = follow_call64(kernel, func);
        if (!func) return 0;
        
        return func + kerndumpbase;
    }
    addr_t ref = find_strref("\"loadable trust cache buffer too small (%ld) for entries claimed (%d)\"", 1, string_base_cstring, false, true);
    if (!ref) return 0;
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, prelink_base, ref);
    
    if (!start) return 0;
    
    return start + kerndumpbase;
}

addr_t find_paciza_pointer__l2tp_domain_module_start() {
    uint64_t string = (uint64_t)boyermoore_horspool_memmem(kernel + data_base, data_size, (const unsigned char *)"com.apple.driver.AppleSynopsysOTGDevice", strlen("com.apple.driver.AppleSynopsysOTGDevice")) - (uint64_t)kernel;
    
    if (!string) {
        return 0;
    }
    
    return string + kerndumpbase - 0x20;
}

addr_t find_paciza_pointer__l2tp_domain_module_stop() {
    uint64_t string = (uint64_t)boyermoore_horspool_memmem(kernel + data_base, data_size, (const unsigned char *)"com.apple.driver.AppleSynopsysOTGDevice", strlen("com.apple.driver.AppleSynopsysOTGDevice")) - (uint64_t)kernel;
    
    if (!string) {
        return 0;
    }
    
    return string + kerndumpbase - 0x18;
}

uint64_t find_l2tp_domain_inited() {
    uint64_t ref = find_strref("L2TP domain init\n", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    
    uint64_t addr = calc64(kernel, ref, ref + 32, 8);
    
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

uint64_t find_sysctl__net_ppp_l2tp() {
    uint64_t ref = find_strref("L2TP domain terminate : PF_PPP domain does not exist...\n", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    ref += 4;
    
    uint64_t addr = calc64(kernel, ref, ref + 28, 0);
    
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

uint64_t find_sysctl_unregister_oid() {
    uint64_t ref = find_strref("L2TP domain terminate : PF_PPP domain does not exist...\n", 1, string_base_cstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t addr = step64(kernel, ref, 28, INSN_CALL);
    
    if (!addr) {
        return 0;
    }
    
    addr += 4;
    addr = step64(kernel, addr, 28, INSN_CALL);
    
    if (!addr) {
        return 0;
    }
    
    uint64_t call = follow_call64(kernel, addr);
    if (!call) {
        return 0;
    }
    return call + kerndumpbase;
}

uint64_t find_mov_x0_x4__br_x5() {
    uint32_t bytes[] = {
        0xaa0403e0, // mov x0, x4
        0xd61f00a0  // br x5
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    return addr - (uint64_t)kernel + kerndumpbase;
}

uint64_t find_mov_x9_x0__br_x1() {
    uint32_t bytes[] = {
        0xaa0003e9, // mov x9, x0
        0xd61f0020  // br x1
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    return addr - (uint64_t)kernel + kerndumpbase;
}

uint64_t find_mov_x10_x3__br_x6() {
    uint32_t bytes[] = {
        0xaa0303ea, // mov x10, x3
        0xd61f00c0  // br x6
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    return addr - (uint64_t)kernel + kerndumpbase;
}

uint64_t find_kernel_forge_pacia_gadget() {
    uint32_t bytes[] = {
        0xdac10149, // paci
        0xf9007849  // str x9, [x2, #240]
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    return addr - (uint64_t)kernel + kerndumpbase;
}

uint64_t find_kernel_forge_pacda_gadget() {
    uint32_t bytes[] = {
        0xdac10949, // pacd x9
        0xf9007449  // str x9, [x2, #232]
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    return addr - (uint64_t)kernel + kerndumpbase;
}

uint64_t find_IOUserClient__vtable() {
    uint64_t ref1 = find_strref("IOUserClient", 2, string_base_cstring, true, false);
    
    if (!ref1) {
        return 0;
    }
    
    ref1 -= kerndumpbase;
    
    uint64_t ref2 = find_strref("IOUserClient", 3, string_base_cstring, true, false);
    
    if (!ref2) {
        return 0;
    }
    
    ref2 -= kerndumpbase;
    
    uint64_t func2 = bof64(kernel, xnucore_base, ref2);
    
    if (!func2) {
        return 0;
    }
    
    uint64_t vtable = calc64(kernel, ref1, func2, 8);
    
    if (!vtable) {
        return 0;
    }
    
    return vtable + kerndumpbase;
}

uint64_t find_IORegistryEntry__getRegistryEntryID() {
    uint32_t bytes[] = {
        0xf9400808, // ldr x8, [x0, #0x10]
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    
    if (!addr) {
        return 0;
    }
    
    // basically just look the instructions
    // can't find a better way
    // this was not done like the previous gadgets because an address is being used, which won't be the same between devices so can't be hardcoded and i gotta use masks
    
    // cbz x8, SOME_ADDRESS <= where we do masking (((*(uint32_t *)(addr + 4)) & 0xFC000000) != 0xb4000000)
    // ldr x0, [x8, #8]     <= 2nd part of 0xd65f03c0f9400500
    // ret                  <= 1st part of 0xd65f03c0f9400500
    
    while ((((*(uint32_t *)(addr + 4)) & 0xFC000000) != 0xb4000000) || (*(uint64_t*)(addr + 8) != 0xd65f03c0f9400500)) {
        addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)(addr + 4), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    }
    
    return addr + kerndumpbase - (uint64_t)kernel;;
}

addr_t find_cs_blob_generation_count() {
    uint64_t ref = find_strref("CS Platform Exec Logging: Executing platform signed binary '%s'", 1, 2, false, false);
    if (!ref) {
        return 0;
    }
    ref -= kerndumpbase;
    
    uint64_t addr = step64(kernel, ref, 200, INSN_ADRP);
    if (!addr) {
        addr = step64(kernel, ref, 200, INSN_CALL);
        if (!addr) {
            return 0;
        }
        
        addr = step64(kernel, addr + 4, 200, INSN_CALL);
        if (!addr) {
            return 0;
        }
        
        addr = step64(kernel, addr + 4, 200, INSN_CALL);
        if (!addr) {
            return 0;
        }
        
        addr = step64(kernel, addr + 4, 200, INSN_CALL);
        if (!addr) {
            return 0;
        }
        
        addr = follow_call64(kernel, addr);
        if (!addr) {
            return 0;
        }
        
        addr = step64(kernel, addr, 200, INSN_ADRP);
        if (!addr) {
            return 0;
        }
        
        addr = calc64(kernel, addr - 4, addr + 8, 9);
        if (!addr) {
            return 0;
        }
        
        return addr + kerndumpbase;
    }
    
    addr = calc64(kernel, addr, addr + 12, 25);
    if (!addr) {
        addr = step64(kernel, ref, 200, INSN_ADRP);
        addr = calc64(kernel, addr, addr + 12, 9);
    }
    
    return addr + kerndumpbase;
}

addr_t find_cs_find_md() {
    
    uint32_t bytes[] = {
        0xb9400008, // ldr w8, [x0]
        0x529bdf49, // mov w9, #0xdefa
        0x72a04189, // movk w9, #0x20c, lsl#16
        0x6b09011f  // cmp w8, w9
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    if (!addr) {
        return 0;
    }
    
    addr -= (uint64_t)kernel;
    
    uint64_t adrp = step64(kernel, addr, 200, INSN_ADRP);
    if (!adrp) {
        return 0;
    }
    
    adrp += 4;
    
    uint64_t adrp2 = step64(kernel, adrp, 200, INSN_ADRP);
    if (adrp2) {
        adrp = adrp2; // non-A12
    }
    
    addr = calc64(kernel, adrp - 4, adrp + 8, 9);
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}


addr_t find_cs_validate_csblob() {
    
    uint32_t bytes[] = {
        0x52818049, // mov w9, #0xC02
        0x72bf5bc9, // movk w9, #0xfade, lsl#16
        0x6b09011f  // cmp w8, w9
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    if (!addr) {
        return 0;
    }
    
    addr -= (uint64_t)kernel;
    addr = bof64(kernel, xnucore_base, addr);
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

addr_t find_kalloc_canblock() {
    
    uint32_t bytes[] = {
        0xaa0003f3, // mov x19, x0
        0xf9400274, // ldr x20, [x19]
        0xf11fbe9f  // cmp x20, #0x7ef
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    if (!addr) {
        return 0;
    }
    addr -= (uint64_t)kernel;
    
    addr = bof64(kernel, xnucore_base, addr);
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

addr_t find_ubc_cs_blob_allocate_site() {
    uint32_t bytes[] = {
        0xd2adfc0d,
        0xf2c0002d
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    if (!addr) {
        return 0;
    }
    addr -= (uint64_t)kernel;
    
    uint64_t kalloc = find_kalloc_canblock();
    if (!kalloc) {
        return 0;
    }
    
    do {
        addr = step64(kernel, addr + 4, 1000, INSN_CALL);
        if (!addr) {
            return 0;
        }
    } while (follow_call64(kernel, addr) + kerndumpbase != kalloc);
    
    addr = step64_back(kernel, addr, 20, INSN_ADRP);
    if (!addr) {
        return 0;
    }
    
    addr = calc64(kernel, addr, addr + 8, 2);
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

addr_t find_kfree() {
    
    uint32_t bytes[] = {
        0xaa0103f3,
        0xaa0003f4,
        0xf11fbe7f
    };
    
    uint64_t addr = (uint64_t)boyermoore_horspool_memmem((unsigned char *)((uint64_t)kernel + xnucore_base), xnucore_size, (const unsigned char *)bytes, sizeof(bytes));
    if (!addr) {
        return 0;
    }
    addr -= (uint64_t)kernel;
    
    addr = bof64(kernel, xnucore_base, addr);
    if (!addr) {
        return 0;
    }
    
    return addr + kerndumpbase;
}

addr_t find_hook_cred_label_update_execve() {
    addr_t ref = find_strref("only launchd is allowed to spawn untrusted binaries", 1, string_base_pstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, prelink_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}

addr_t find_flow_divert_connect_out() {
    addr_t ref = find_strref("(%u): Sending saved connect packet\n", 1, string_base_oslstring, false, false);
    
    if (!ref) {
        return 0;
    }
    
    ref -= kerndumpbase;
    
    uint64_t start = bof64(kernel, xnucore_base, ref);
    
    if (!start) {
        return 0;
    }
    
    return start + kerndumpbase;
}
/*
 *
 *
 *
 */

#ifndef NOT_DARWIN
#include <mach-o/nlist.h>
#else
#include "mach-o_nlist.h"
#endif


addr_t
find_symbol(const char *symbol)
{
    if (!symbol) {
        return 0;
    }
    
    unsigned i;
    const struct mach_header *hdr = kernel_mh;
    const uint8_t *q;
    int is64 = 0;

    if (IS64(hdr)) {
        is64 = 4;
    }

/* XXX will only work on a decrypted kernel */
    if (!kernel_delta) {
        return 0;
    }

    /* XXX I should cache these.  ohwell... */
    q = (uint8_t *)(hdr + 1) + is64;
    for (i = 0; i < hdr->ncmds; i++) {
        const struct load_command *cmd = (struct load_command *)q;
        if (cmd->cmd == LC_SYMTAB) {
            const struct symtab_command *sym = (struct symtab_command *)q;
            const char *stroff = (const char *)kernel + sym->stroff + kernel_delta;
            if (is64) {
                uint32_t k;
                const struct nlist_64 *s = (struct nlist_64 *)(kernel + sym->symoff + kernel_delta);
                for (k = 0; k < sym->nsyms; k++) {
                    if (s[k].n_type & N_STAB) {
                        continue;
                    }
                    if (s[k].n_value && (s[k].n_type & N_TYPE) != N_INDR) {
                        if (!strcmp(symbol, stroff + s[k].n_un.n_strx)) {
                            /* XXX this is an unslid address */
                            return s[k].n_value;
                        }
                    }
                }
            }
        }
        q = q + cmd->cmdsize;
    }
    return 0;
}

#ifdef HAVE_MAIN

int
main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: patchfinder64 _decompressed_kernel_image_\n");
        printf("iOS ARM64 kernel patchfinder\n");
        exit(EXIT_FAILURE);
    }
    if (access(argv[1], F_OK) != 0) {
        printf("%s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }
    int rv;
    addr_t kernel_base = 0;
    const addr_t vm_kernel_slide = 0;
    if (init_kernel(NULL, kernel_base, argv[1]) != 0) {
        printf("Failed to prepare kernel\n");
        exit(EXIT_FAILURE);
    }

#define FIND(name) do { \
    addr_t patchfinder_offset = find_ ##name (); \
    printf("%s: PF=0x%llx - %s\n", #name, patchfinder_offset, (patchfinder_offset != 0 && patchfinder_offset != kerndumpbase)? "PASS" : "FAIL"); \
} while(false)
#define CHECK(name) do { \
    addr_t actual_offset = find_symbol("_" #name); \
    if (actual_offset == 0) { \
        FIND(name); \
    } else { \
        addr_t patchfinder_offset = find_ ##name (); \
        printf("%s: PF=0x%llx - AS=0x%llx - %s\n", #name, patchfinder_offset, actual_offset, ((actual_offset==0?patchfinder_offset!=0:patchfinder_offset == actual_offset) ? "PASS" : "FAIL")); \
    } \
} while(false)
    
    CHECK(syscall_check_sandbox);
    CHECK(IOMalloc);
    CHECK(IOFree);
    CHECK(copy_path_for_vp);
    CHECK(vn_getpath);
    CHECK(kmod_start);
    CHECK(handler_map);
    CHECK(issue_extension_for_mach_service);
    CHECK(issue_extension_for_absolute_path);
    CHECK(policy_conf);
    CHECK(policy_ops);
    CHECK(syscall_set_profile);
    CHECK(sandbox_set_container_copyin);
    CHECK(platform_set_container);
    CHECK(extension_create_file);
    CHECK(extension_add);
    CHECK(extension_release);
    CHECK(unix_syscall_return);
    CHECK(sfree);
    CHECK(sstrdup);
    CHECK(pthread_kext_register);
    CHECK(pthread_callbacks);
    CHECK(sysent);
    CHECK(proc_find);
    CHECK(proc_rele);
    CHECK(vfs_context_current);
    CHECK(vnode_lookup);
    CHECK(vnode_put);
    CHECK(vnode_getfromfd);
    CHECK(vnode_getattr);
    CHECK(SHA1Init);
    CHECK(SHA1Update);
    CHECK(SHA1Final);
    CHECK(csblob_entitlements_dictionary_set);
    CHECK(kernel_task);
    CHECK(kernproc);
    CHECK(vnode_recycle);
    CHECK(lck_mtx_lock);
    CHECK(lck_mtx_unlock);
    CHECK(strlen);
    CHECK(add_x0_x0_0x40_ret);
    CHECK(trustcache);
    CHECK(move_snapshot_to_purgatory);
    CHECK(apfs_jhash_getvnode);
    CHECK(zone_map_ref);
    CHECK(OSBoolean_True);
    CHECK(osunserializexml);
    CHECK(smalloc);
    CHECK(shenanigans);
    CHECK(fs_lookup_snapshot_metadata_by_name_and_return_name);
    CHECK(mount_common);
    CHECK(fs_snapshot);
    CHECK(vnode_get_snapshot);
    if (auth_ptrs) {
        CHECK(paciza_pointer__l2tp_domain_module_start);
        CHECK(paciza_pointer__l2tp_domain_module_stop);
        CHECK(l2tp_domain_inited);
        CHECK(sysctl__net_ppp_l2tp);
        CHECK(sysctl_unregister_oid);
        CHECK(mov_x0_x4__br_x5);
        CHECK(mov_x9_x0__br_x1);
        CHECK(mov_x10_x3__br_x6);
        CHECK(kernel_forge_pacia_gadget);
        CHECK(kernel_forge_pacda_gadget);
        CHECK(pmap_load_trust_cache);
    }
    CHECK(IOUserClient__vtable);
    CHECK(IORegistryEntry__getRegistryEntryID);
    CHECK(cs_blob_generation_count);
    CHECK(cs_find_md);
    CHECK(cs_validate_csblob);
    CHECK(kalloc_canblock);
    CHECK(ubc_cs_blob_allocate_site);
    CHECK(kfree);
    CHECK(hook_cred_label_update_execve);
    CHECK(flow_divert_connect_out);
    
    term_kernel();
    return EXIT_SUCCESS;
}

#endif	/* HAVE_MAIN */

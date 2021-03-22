//
//  patchfinder64.c
//  extra_recipe
//
//  Created by xerub on 06/06/2017.
//  Copyright © 2017 xerub. All rights reserved.
//

#define KERNEL
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>
#include <mach/mig.h>
#include "mac_policy.h"
#include "sysctl.h"
#include "patchfinder64.h"

bool auth_ptrs = false;
bool arm64e = false;
typedef unsigned long long addr_t;
static addr_t kerndumpbase = -1;
static size_t kernel_size = 0;
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
static addr_t sysctl_set_base = 0;
static addr_t sysctl_set_size = 0;
static addr_t data_const_base = 0;
static addr_t data_const_size = 0;
static addr_t pdata_base = 0;
static addr_t pdata_size = 0;
static addr_t const_base = 0;
static addr_t const_size = 0;
static addr_t kernel_entry = 0;
static struct sysctl_oid_list *sysctl__children; /* root list */
static void *kernel_mh = 0;
static addr_t kernel_delta = 0;
bool monolithic_kernel = false;
uint32_t cmdline_offset = 0;
const char *kernel_version_string = NULL;
uint32_t kernel_major = 0;
uint32_t kernel_minor = 0;
uint32_t kernel_revision = 0;
uint32_t xnu_major = 0;

struct {
    size_t p_uid;
    size_t p_gid;
    size_t p_ruid;
    size_t p_rgid;
    size_t p_svuid;
    size_t p_svgid;
} proc_offsets = {0};

#define IS64(image) (*(uint8_t *)(image) & 1)

#define MACHO(p) ((*(unsigned int *)(p) & ~1) == 0xfeedface)
#define INSN(p) (*(uint32_t*)(kernel + p))
// Immediate value from INSN "p" consisting of "bits" bits ending "start" bits from the end left shifted "shift" bits
#define IMM(p, start, bits, shift) (((INSN(p)>>start)&((1<<bits)-1))<<shift)
#define KADDR_TO_BUF(buf, addr) ((addr)?(addr_t)(addr) - (addr_t)kerndumpbase + (addr_t)(buf):0)

static addr_t _remove_pac(addr_t addr);
#define remove_pac(x) _remove_pac((addr_t)(x))

/* generic stuff *************************************************************/

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

/* these operate on VA ******************************************************/

#define INSN_RET  0xD65F03C0, 0xFFFFF3C0
#define INSN_CALL 0x94000000, 0xFC000000
#define INSN_B    0x14000000, 0xFC000000
#define INSN_CBZ  0x34000000, 0x7F000000
#define INSN_ADRP 0x90000000, 0x9F000000

_pf64_internal unsigned char *
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

_pf64_internal int HighestSetBit(int N, uint32_t imm)
{
	int i;
	for (i = N - 1; i >= 0; i--) {
		if (imm & (1 << i)) {
			return i;
		}
	}
	return -1;
}

_pf64_internal uint64_t ZeroExtendOnes(unsigned M, unsigned N)	// zero extend M ones to N width
{
	(void)N;
	return ((uint64_t)1 << M) - 1;
}

_pf64_internal uint64_t RORZeroExtendOnes(unsigned M, unsigned N, unsigned R)
{
	uint64_t val = ZeroExtendOnes(M, N);
	if (R == 0) {
		return val;
	}
	return ((val >> R) & (((uint64_t)1 << (N - R)) - 1)) | ((val & (((uint64_t)1 << R) - 1)) << (N - R));
}

_pf64_internal uint64_t Replicate(uint64_t val, unsigned bits)
{
	uint64_t ret = val;
	unsigned shift;
	for (shift = bits; shift < 64; shift += bits) {	// XXX actually, it is either 32 or 64
		ret |= (val << shift);
	}
	return ret;
}

_pf64_internal int DecodeBitMasks(unsigned immN, unsigned imms, unsigned immr, int immediate, uint64_t *newval)
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

_pf64_internal int DecodeMov(uint32_t opcode, uint64_t total, int first, uint64_t *newval)
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

_pf64_internal uint64_t ROR64(uint64_t x, int shift)
{
    if (shift == 0) return x;
    int m = shift % 64;
    return (x >> m) | x<<(64-m);
}

_pf64_internal uint32_t ROR32(uint64_t x, int shift)
{
    if (shift == 0) return x;
    int m = shift % 32;
    return (x >> m) | x<<(32-m);
}

/* patchfinder ***************************************************************/

_pf64_internal addr_t
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

_pf64_internal addr_t
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

_pf64_internal addr_t
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

_pf64_internal addr_t
xref64(const uint8_t *buf, addr_t start, addr_t end, addr_t what)
{
    addr_t i;
    uint64_t value[32];

    memset(value, 0, sizeof(value));

    end &= ~3;
    for (i = start & ~3; i < end; i += 4) {
        uint32_t op = *(uint32_t *)(buf + i);
        unsigned reg = op & 0x1f;

        switch (buf[i + 3]) {
            case 0x90:
            case 0xb0:
            case 0xd0:
            case 0xf0:
                {
                    //if ((op & 0x9F000000) == 0x90000000) {
                    signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
                    //printf("%llx: ADRP X%d, 0x%llx\n", i, reg, ((long long)adr << 1) + (i & ~0xFFF));
                    value[reg] = ((long long)adr << 1) + (i & ~0xFFF);
                    continue;				// XXX should not XREF on its own?
                }
            case 0xb9:
            case 0xf9:
                {
                    if ((op & 0xBFC00000) == 0xB9000000) {
                        // STR Rt, [Rn, #imm]
                        unsigned rn = (op >> 5) & 0x1F;
                        unsigned variant = (op>>30)&1;
                        unsigned imm = ((op >> 10) & 0xFFF) << (2+variant);
                        if (!imm) continue;
                        if (value[rn]+imm == what) {
                            return i;
                        }
                        break;
                    }
                }
            case 0xfb:
            case 0xfd:
            case 0xff:
                {
                    if ((op & 0xF9C00000) == 0xF9400000) {
                        unsigned rn = (op >> 5) & 0x1F;
                        unsigned imm = ((op >> 10) & 0xFFF) << 3;
                        //printf("%llx: LDR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
                        if (!imm) {
                            value[reg] = 0;
                            continue;			// XXX not counted as true xref
                        }
                        value[reg] = value[rn] + imm;	// XXX address, not actual value
                    }
                    break;
                }
                /*
            case 0xaa:
                //if ((op & 0xFFE0FFE0) == 0xAA0003E0)
                unsigned rd = op & 0x1F;
                unsigned rm = (op >> 16) & 0x1F;
                //printf("%llx: MOV X%d, X%d\n", i, rd, rm);
                value[rd] = value[rm];
                break; */
            case 0x91:
                {
                    //if ((op & 0xFF000000) == 0x91000000) {
                    unsigned rn = (op >> 5) & 0x1F;
                    if (rn == 0x1f) {
                        // ignore ADD Xn, SP
                        if (!auth_ptrs && reg == 29) {
                            // This looks like the beginning of a func - reset registers
                            memset(value, 0, sizeof(value));
                        } else {
                            value[reg] = 0;
                        }
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
                    break;
                }
        /*} else if ((op & 0xF9C00000) == 0xF9000000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: STR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            if (!imm) continue;			// XXX not counted as true xref
            value[rn] = value[rn] + imm;	// XXX address, not actual value*/
            case 0x10:
            case 0x30:
            case 0x50:
            case 0x70:
                {
                    //if ((op & 0x9F000000) == 0x10000000) {
                    signed adr = ((op & 0x60000000) >> 18) | ((op & 0xFFFFE0) << 8);
                    //printf("%llx: ADR X%d, 0x%llx\n", i, reg, ((long long)adr >> 11) + i);
                    value[reg] = ((long long)adr >> 11) + i;
                    break;
                }
            case 0x58:
                {
                    //if ((op & 0xFF000000) == 0x58000000) {
                    unsigned adr = (op & 0xFFFFE0) >> 3;
                    //printf("%llx: LDR X%d, =0x%llx\n", i, reg, adr + i);
                    value[reg] = adr + i;		// XXX address, not actual value
                    break;
                }
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                    // if ((op & 0xfc000000) == 0x14000000)
                    // B addr
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
                    //if ((op & 0xFC000000) == 0x94000000) {
                    // BL addr
                {
                    signed imm = (op & 0x3FFFFFF) << 2;
                    if (op & 0x2000000) {
                        imm |= 0xf << 28;
                    }
                    unsigned adr = (unsigned)(i + imm);
                    if (adr == what) {
                        return i;
                    }
                    break;
                }
            case 0xd5:
                {
                    if (op == 0xD503237F) {
                        // PACIBSP - reset register values
                        memset(value, 0, sizeof(value));
                    } else if ((op & 0xFFF00000) == 0xD5300000) {
                        // MRS Rn
                        value[reg] = 0;
                    }
                    break;
                }
            case 0xd6:
                {
                    if ((op & 0xFFFFFC1F) == 0xD65F0000) {
                        // RET - reset register values
                        memset(value, 0, sizeof(value));
                    }
                    break;
                }

            default:
                continue;
        }
        // Don't match SP as an offset
        if (value[reg] == what && reg != 0x1f) {
            return i;
        }
    }
    return 0;
}

_pf64_internal addr_t
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
        } else if ((op & 0xFFE0FFE0) == 0xAA0003E0) {
            unsigned rd = op & 0x1F;
            unsigned rm = (op >> 16) & 0x1F;
            //printf("%llx: MOV X%d, X%d\n", i, rd, rm);
            value[rd] = value[rm];
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
        } else if ((op & 0x7F200000) == 0x0B000000) {
            // ADD (shifted register)
            unsigned rn = (op >> 5) & 0x1F;
            unsigned rm = (op >> 16) & 0x1F;
            unsigned shift = (op>>22) & 0x3;
            unsigned imm = (op>>10) & 0x3F;
            if ((op>>31) == 0) imm &= 0x1F;
            if (imm == 0) {
                value[reg] = value[rn] + value[rm];
            } else {
                switch (shift) {
                    case 0:
                        // LSL
                        value[reg] = value[rn] + (value[rm]<<imm);
                        break;
                    case 1:
                        // LSR
                        value[reg] = value[rn] + (value[rm]>>imm);
                        break;
                    case 2:
                        // ASR
                        value[reg] = value[rn] + (int64_t)((value[rm]>>imm) | 0-(1<<imm));
                        break;
                    default:
                        // Invalid
                        break;
                }
            }
        } else if ((op & 0xF9C00000) == 0xF9400000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: LDR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
            value[reg] = value[rn] + imm;	// XXX address, not actual value
        } else if ((op & 0xF9C00000) == 0xF9000000) {
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imm = ((op >> 10) & 0xFFF) << 3;
            //printf("%llx: STR X%d, [X%d, 0x%x]\n", i, reg, rn, imm);
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
        } else if ((op & 0x1F800000) == 0x12800000 && ((op >> 29)&3) != 1) {
            uint64_t imm = 0;
            if (DecodeMov(op, value[reg], 0, &imm) == 0) {
                value[reg] = imm;
            }
        } else if ((op & 0x7F800000) == 0x53000000) {
            // UBFM
            unsigned rn = (op >> 5) & 0x1F;
            unsigned imms = (op >> 10) & 0x3F;
            unsigned immr = (op >> 16) & 0x3F;
            unsigned N = (op >> 22) & 1;
            unsigned sf = (op >> 31) & 1;

            if (sf != N) continue;

            if ( sf==1 && (((immr>>1)&1) || ((imms>>1)&1) )) continue;

            uint64_t wmask = 0;
            if (DecodeBitMasks(N, imms, immr, false, &wmask) == 0)  {
                //printf("%llx: UBFM X%d, X%d, #0x%x, #0x%x = 0x%llx\n", i, reg, rn, imms, immr, value[reg]);
                value[reg] = (sf?ROR64(value[rn], immr):ROR32(value[rn], immr)) & wmask;
            }
        }
    }
    return value[which];
}

_pf64_internal addr_t
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

_pf64_internal addr_t
find_call64(const uint8_t *buf, addr_t start, size_t length)
{
    return step64(buf, start, length, INSN_CALL);
}

_pf64_internal addr_t
follow_call64(const uint8_t *buf, addr_t call)
{
    long long w;
    w = *(uint32_t *)(buf + call) & 0x3FFFFFF;
    w <<= 64 - 26;
    w >>= 64 - 26 - 2;
    return call + w;
}

_pf64_internal addr_t
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

_pf64_internal addr_t
find_parent_of_stub(const uint8_t *buf, addr_t stub, enum text_bases text_base)
{
    addr_t call;
    for (int i=1; (call = find_reference(stub, i, text_base)); i++) {
        uint32_t insn = *(uint32_t*)(buf+(call-kerndumpbase));
        if ((insn&0xfc000000) != 0x14000000) break;

        addr_t parent = find_parent_of_stub(buf, call, text_base);
        if (parent) return parent;
    }
    if (!call) return 0;
    call -= kerndumpbase;

    addr_t func = bof64(buf, xnucore_base, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_internal addr_t
follow_cbz(const uint8_t *buf, addr_t cbz)
{
    return cbz + ((*(int *)(buf + cbz) & 0x3FFFFE0) << 10 >> 13);
}

_pf64_internal addr_t
_remove_pac(addr_t addr)
{
    if (!addr || addr >= kerndumpbase) return addr;
    if (addr >> 56 == 0x80) {
        return (addr&0xffffffff) + kerndumpbase;
    }
    addr |= 0xfffffff000000000;
    if (addr < kerndumpbase || addr > kerndumpbase + kernel_size) return 0;
    return addr;
}

_pf64_internal void rebase_sysctl_oid(uint8_t *buf, struct sysctl_oid *sysctl) {
    sysctl->oid_parent = (struct sysctl_oid_list*)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_parent));
    if (SLIST_NEXT(sysctl, oid_link)) SLIST_NEXT(sysctl, oid_link) = (struct sysctl_oid *)KADDR_TO_BUF(buf, remove_pac(SLIST_NEXT(sysctl, oid_link)));
    sysctl->oid_arg1 = (void *)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_arg1));
    if (sysctl->oid_name) sysctl->oid_name = (const char *)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_name));
    sysctl->oid_handler = (int (*)SYSCTL_HANDLER_ARGS)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_handler));
    sysctl->oid_fmt = (const char *)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_fmt));
    sysctl->oid_descr = (const char *)KADDR_TO_BUF(buf, remove_pac(sysctl->oid_descr));
}

_pf64_internal bool register_sysctl_oid(uint8_t *buf, struct sysctl_oid *sysctl) {
    rebase_sysctl_oid(buf, sysctl);
    if (sysctl->oid_version != SYSCTL_OID_VERSION) {
        fprintf(stderr, "encountered unknown sysctl version %d\n", sysctl->oid_number);
        return false;
    }
    if ((sysctl->oid_kind & CTLTYPE) == CTLTYPE_NODE && sysctl->oid_name &&  strcmp(sysctl->oid_name, "sysctl") == 0) {
        sysctl__children = sysctl->oid_parent;
    }

    struct sysctl_oid *p = NULL;
    if (sysctl->oid_number == OID_AUTO) {
        int n = OID_AUTO_START;
        SLIST_FOREACH(p, sysctl->oid_parent, oid_link) {
            if ((addr_t)p > kerndumpbase) {
                p = (struct sysctl_oid *)KADDR_TO_BUF(buf, p);
                rebase_sysctl_oid(buf, p);
            }
            if (p->oid_number > n) n = p->oid_number;
        }
        sysctl->oid_number = n+1;
    }
    struct sysctl_oid *q = NULL;
    if (!sysctl->oid_parent) return false; //bail
    SLIST_FOREACH(p, sysctl->oid_parent, oid_link) {
        //fprintf(stderr, "base %p sysctl %p p %p\n", kerndumpbase, sysctl, p);
        if ((addr_t)p > kerndumpbase) {
            p = (struct sysctl_oid *)KADDR_TO_BUF(buf, p);
            rebase_sysctl_oid(buf, p);
        }
        if (sysctl->oid_number < p->oid_number) break;
        q = p;
    }
    if (q) {
        SLIST_INSERT_AFTER(q, sysctl, oid_link);
    } else {
        SLIST_INSERT_HEAD(sysctl->oid_parent, sysctl, oid_link);
    }
    return true;
}

// TODO: follow default case
_pf64_internal addr_t follow_switch(const uint8_t *buf, addr_t entry, int branch) {
    uint64_t ref = (uint64_t)step64(buf, entry, 0x100, 0x7100001F, 0xFFC0001F);
    if (!ref) return 0;
    uint32_t insn = *(uint32_t*)(buf+ref);
    int len = (insn>>10) & 0xFFF;

    addr_t sub_insn = step64_back(buf, ref-4, 0x8, 0x51000000|((insn>>5)&0x1f), 0xFFC0001F);
    int skipped = 0;
    if (sub_insn) {
        skipped = ((*(uint32_t*)(buf+sub_insn)>>10)&0xFFF);
        // Could zeroextend here but it should be positive... soooo
        if (skipped>>11) return 0;
        branch -= skipped;
    }
    if (branch > len || branch < 0) return 0;

    addr_t adrp = step64(buf, ref, 0x28, INSN_ADRP);
    addr_t adr = step64(buf, ref, 0x28, 0x10000000, 0x9F000000);
    fprintf(stderr, "switch adrp %llx adr %llx\n", adrp + kerndumpbase, adr + kerndumpbase);
    if (!adrp && !adr) return 0;

    addr_t jpref = adrp?adrp:adr;

    int reg = *(uint32_t *)(buf + jpref) & 0x1f;
    addr_t jptbl = calc64(buf, jpref, jpref+8, reg);
    fprintf(stderr, "jptbl at %llx\n", jptbl + kerndumpbase);
    if (!jptbl) return 0;
    uint16_t offset = *(uint16_t*)(buf + jptbl);
    if ((*(uint32_t*)(buf + jptbl) >> 16) != 0xffff) {
        if (jpref == adr) return 0;
        reg = *(uint32_t *)(buf + adr) & 0x1f;
        addr_t jumpbase = calc64(buf, adr, adr+4, reg);
        uint16_t offset = *(uint16_t*)(buf + jptbl + (branch<<1));
        jpref = jumpbase + (offset<<2);
    } else {
        jpref = jptbl + *(int *)(buf + jptbl + (branch<<2));
    }
    return jpref;
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

_pf64_external int
init_kernel(kread_t kread, addr_t kernel_base, const char *filename)
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
                    } else if (!strcmp(sec[j].sectname, "__sysctl_set")) {
                        sysctl_set_base = sec[j].addr;
                        sysctl_set_size = sec[j].size;
                    }
                }
            } else if (!strcmp(seg->segname, "__PRELINK_DATA")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__data")) {
                        pdata_base = sec[j].addr;
                        pdata_size = sec[j].size;
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
    sysctl_set_base -= kerndumpbase;
    const_base -= kerndumpbase;
    kernel_size = max - min;
	
#define READ_BASE(base, size) do { \
	rv = kread(kerndumpbase + base, (void *)((uintptr_t)kernel + base), size); \
	if (rv == -1) { \
		return -1; \
	} \
} while (false)

    if (filename == NULL) {
        kernel = calloc(1, kernel_size);
        if (!kernel) {
            return -1;
        }
		READ_BASE(xnucore_base, xnucore_size);
		READ_BASE(prelink_base, prelink_size);
		READ_BASE(cstring_base, cstring_size);
		READ_BASE(pstring_base, pstring_size);
		READ_BASE(oslstring_base, oslstring_size);
		READ_BASE(data_const_base, data_const_size);
		READ_BASE(data_base, data_size);
		READ_BASE(sysctl_set_base, sysctl_set_size);
		READ_BASE(const_base, const_size);

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
    if (hdr->cputype == CPU_TYPE_ARM64 && hdr->cpusubtype == CPU_SUBTYPE_ARM64E) {
        arm64e = true;
    }
    for (struct sysctl_oid **sysctlp = (struct sysctl_oid **)(kernel + sysctl_set_base); (addr_t)sysctlp <= (addr_t)kernel+sysctl_set_base+sysctl_set_size-8; sysctlp++) {
        // Clean up any PAC in the structure
        struct sysctl_oid *sysctl = (struct sysctl_oid *)KADDR_TO_BUF(kernel, remove_pac(*sysctlp));
        register_sysctl_oid(kernel, sysctl);
    }
    addr_t kernel_version_str = find_str("Darwin Kernel Version", 1, string_base_const, false, false);
    if (kernel_version_str) {
        kernel_version_string = (const char *)KADDR_TO_BUF(kernel, kernel_version_str);
        size_t version_len = strlen(kernel_version_string);
        char *version_buf[version_len];
        char *version = (char*)version_buf;
        strcpy(version, kernel_version_string + strlen("Darwin Kernel Version "));

        char *sep = strchr(version, '.');
        if (!sep) return 0;
        *sep = '\0';
        kernel_major = atoi(version);
        version = sep+1;

        sep = strchr(version, '.');
        if (!sep) {
            kernel_major = 0;
            return 0;
        }
        *sep = '\0';
        kernel_minor = atoi(version);
        version = sep+1;

        sep = strchr(version, ':');
        if (!sep) {
            kernel_minor = kernel_major = 0;
            return 0;
        }
        *sep = '\0';
        kernel_revision = atoi(version);
        version = sep+1;

        sep = strstr(version, "xnu");
        if (!sep) {
            kernel_revision = kernel_minor = kernel_major = 0;
            return 0;
        }
        sep = strchr(sep, '-') + 1;
        if (!sep) return 0;
        version = sep;
        sep = strchr(sep, '.');
        if (!sep) return 0;
        *sep = '\0';
        xnu_major = atoi(version);
    }
    return 0;
}

_pf64_external void
term_kernel(void)
{
    if (kernel != NULL) {
        free(kernel);
        kernel = NULL;
        kernel_version_string = NULL;
    }
}

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
find_str(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base)
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
    if (!str) return 0;

    return str - kernel + kerndumpbase;
}

_pf64_external addr_t
find_strref(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base)
{
    uint8_t *str = (uint8_t *)find_str(string, n, string_base, full_match, ppl_base);
    if (!str) return 0;
    str = str - (uint8_t*)kerndumpbase + kernel;

    enum text_bases text_base;
    if (ppl_base) {
        text_base = text_ppl_base;
    } else {
        text_base = (string_base==string_base_pstring)?text_prelink_base:text_xnucore_base;
    }
    return find_reference(str - kernel + kerndumpbase, n, text_base);
}

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
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
_pf64_external find_entry(void)
{
    /* XXX returns an unslid address */
    return kernel_entry;
}

_pf64_external const unsigned char *
find_mh(void)
{
    return kernel_mh;
}

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t find_amfi_is_cdhash_in_trust_cache(void)
{
    addr_t ref = find_strref("%s: only allowed process can check the trust cache\n", 1, string_base_pstring, true, false); // Trying to find AppleMobileFileIntegrityUserClient::isCdhashInTrustCache
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, prelink_base, ref);
    if (!base) return 0;

    addr_t ldr = step64(kernel, base, 0x200, 0xF9400001, 0xFFC0001F);
    if (!ldr) return 0;

    addr_t call = step64(kernel, ldr, 0x20, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;

    call = step64(kernel, func, 0x20, INSN_CALL);
    if (!call) return 0;

    // amfi_is_cdhash_in_trust_cache
    func = follow_call64(kernel, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_external addr_t find_pmap_image4_trust_caches(void)
{
    if (auth_ptrs) return 0;

    // amfi_is_cdhash_in_trust_cache
    addr_t func = find_amfi_is_cdhash_in_trust_cache();
    if (!func) return 0;
    func -= kerndumpbase;

    // pmap_lookup_in_static_trust_cache
    addr_t call = step64(kernel, func, 0x20, INSN_CALL);
    if (!call) return 0;

    // pmap_lookup_in_loaded_trust_caches
    call = step64(kernel, call+4, 0x20, INSN_CALL);
    if (!call) return 0;

    func = follow_call64(kernel, call);
    if (!func) return 0;

    call = step64(kernel, func, 0x20, INSN_CALL);
    if (!call) return 0;

    func = follow_stub(kernel, call);
    if (!func) return 0;
    func -= kerndumpbase;

    call = step64(kernel, func, 0x100, INSN_CALL);
    if (!call) return 0;

    addr_t cbz = step64(kernel, call, 0x20, INSN_CBZ);
    if (!cbz) return 0;

    int rn = *(uint32_t*)(kernel+cbz) & 0x1f;
    addr_t cache = calc64(kernel, func, cbz, rn);
    if (!cache) return 0;
    return cache + kerndumpbase;
}

_pf64_external addr_t
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

    addr_t ref = find_strref("%s: only allowed process can check the trust cache\n", 1, string_base_pstring, true, false); // Trying to find AppleMobileFileIntegrityUserClient::isCdhashInTrustCache
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

_pf64_external addr_t
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

_pf64_external addr_t
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

_pf64_external addr_t
find_allproc(void)
{
    addr_t ref = find_strref("shutdownwait", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t adrp = step64(kernel, ref, 0x20, INSN_ADRP);
    if (!adrp) return 0;

    addr_t cbz = step64(kernel, adrp, 0x10, INSN_CBZ);
    if (!cbz || cbz != adrp+8) return 0;

    int rn = *(uint32_t*)(kernel + cbz) & 0x1f;
    
    addr_t allproc = calc64(kernel, adrp, cbz, rn);
    if (!allproc) return 0;

    return allproc + kerndumpbase;
}

_pf64_external addr_t
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

_pf64_external addr_t
find_realhost_for(addr_t priv)
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

_pf64_external addr_t find_vfs_context_current(void) {
    addr_t str = find_strref("/private/var/tmp/wav%u_%uchans.wav", 1, string_base_pstring, false, false);
    if (!str) return 0;
    str -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, str);
    if (!func) return 0;

    addr_t call = step64(kernel, func, 0x100, INSN_CALL);
    if (!call) return 0;
        
    return follow_stub(kernel, call);
}

_pf64_external addr_t find_vnode_lookup(void) {
    addr_t hfs_str = find_strref("hfs: journal open cb: error %d looking up device %s (dev uuid %s)\n", 1, string_base_pstring, false, false);
    if (!hfs_str) return 0;
    
    hfs_str -= kerndumpbase;

    addr_t call_to_stub = step64_back(kernel, hfs_str, 10*4, INSN_CALL);
    if (!call_to_stub) return 0;
    
    return follow_stub(kernel, call_to_stub);
}

_pf64_external addr_t find_vnode_put(void) {
    addr_t hook_mount_check_snapshot_revert = find_hook_mount_check_snapshot_revert();
    if (!hook_mount_check_snapshot_revert) return 0;
    hook_mount_check_snapshot_revert -= kerndumpbase;

    addr_t call = step64(kernel, hook_mount_check_snapshot_revert+4, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t mov = step64(kernel, call+4, 0x12, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov) return 0;

    int rn = *(uint32_t*)(kernel + mov) & 0x1f;

    mov = step64(kernel, mov, 0x100, 0xAA0003E0 | (rn<<16), 0xFFFFFFFF);
    if (!mov) return 0;

    call = step64(kernel, mov, 0x10, INSN_CALL);
    if (!call) return 0;

    return follow_stub(kernel, call);
}

_pf64_external addr_t find_vnode_getfromfd(void) {
    addr_t syscall_check_sandbox = find_syscall_check_sandbox();
    if (!syscall_check_sandbox) return 0;
    syscall_check_sandbox -= kerndumpbase;

    addr_t ldr = step64(kernel, syscall_check_sandbox+4, 0x1000, 0xB94003E1, 0xFFE003FF);
    if (!ldr) return 0;

    addr_t call = step64(kernel, ldr+4, 0x10, INSN_CALL);
    if (!call) return 0;
    return follow_stub(kernel, call);
}

_pf64_external addr_t find_vnode_getattr(void) {
    addr_t hook_vnode_check_exec = find_hook_vnode_check_exec();
    if (!hook_vnode_check_exec) return 0;
    hook_vnode_check_exec -= kerndumpbase;

    addr_t mov = step64(kernel, hook_vnode_check_exec, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov) return 0;

    int reg = *(uint32_t*)(kernel + mov) & 0x1f;

    mov = step64(kernel, mov, 0x200, 0xAA0003E0 | (reg<<16), 0xFFFFFFFF);
    if (!mov) return 0;

    addr_t call = step64(kernel, mov, 0x12, INSN_CALL);
    if (!call) return 0;
    
    return follow_stub(kernel, call);
}

_pf64_external addr_t find_SHA1Init(void) {
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

_pf64_external addr_t find_SHA1Update(void) {
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


_pf64_external addr_t find_SHA1Final(void) {
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

_pf64_external addr_t find_csblob_entitlements_dictionary_set(void) {
    addr_t ent_str = find_strref("failed getting entitlements", 1, string_base_pstring, true, false);
    if (!ent_str) return 0;
    ent_str -= kerndumpbase;

    addr_t start = bof64(kernel, prelink_base, ent_str);
    if (!start) return 0;

    addr_t mov = step64(kernel, start, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov) return 0;

    int reg = *(uint32_t*)(kernel + mov) & 0x1f;

    while ((mov = step64(kernel, mov+4, 0x200, 0xAA0003E0 | (reg<<16), 0xFFFFFFFF))) {
        if (((*(uint32_t*)(kernel + mov + 4) >> 0) & 0x1f) == 1) break;
    }
    if (!mov) return 0;
    
    addr_t call_to_stub = step64(kernel, mov + 4, 0x12, INSN_CALL);
    if (!call_to_stub) return 0;

    return follow_stub(kernel, call_to_stub);
}

_pf64_external addr_t find_kernel_task(void) {
    addr_t convert_port_to_task = find_convert_port_to_task();
    if (!convert_port_to_task) return 0;
    convert_port_to_task -= kerndumpbase;

    addr_t mov_xn_x0 = step64(kernel, convert_port_to_task, 0x30, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return 0;
    int rn = INSN(mov_xn_x0)&0x1f;

    addr_t ldr_xn_xn_imm = step64(kernel, convert_port_to_task, 0x100, 0xF9400000|(rn<<5), 0xFFC003E0);
    if (!ldr_xn_xn_imm) return 0;
    
    addr_t adrp = step64(kernel, ldr_xn_xn_imm, 0x20, INSN_ADRP);
    if (!adrp) return 0;

    rn = INSN(adrp)&0x1f;

    ldr_xn_xn_imm = step64(kernel, adrp, 0x10, 0xF9400000|(rn<<5), 0xFFC003E0);
    if (!ldr_xn_xn_imm) return 0;

    rn = INSN(ldr_xn_xn_imm)&0x1f;

    addr_t kern_task = calc64(kernel, adrp, ldr_xn_xn_imm+4, rn);
    if (!kern_task) return 0;

    return kern_task + kerndumpbase;

}


_pf64_external addr_t find_kernproc(void) {
    addr_t pid_suspend = find_syscall(SYS_pid_suspend);
    if (!pid_suspend) return 0;
    pid_suspend -= kerndumpbase;

    addr_t call_mac_proc_check_suspend_resume = step64(kernel, pid_suspend, 0x50, INSN_CALL);
    if (!call_mac_proc_check_suspend_resume) return 0;

    addr_t mac_proc_check_suspend_resume = follow_call64(kernel, call_mac_proc_check_suspend_resume);
    if (!mac_proc_check_suspend_resume) return 0;

    addr_t cmp = step64(kernel, mac_proc_check_suspend_resume, 0x50, 0xEB00001F, 0xFFE0FC1F);
    if (!cmp) return 0;

    int rn = (*(uint32_t*)(kernel + cmp)>>5) & 0x1f;

    addr_t kernproc = calc64(kernel, mac_proc_check_suspend_resume, cmp, rn);
    if (!kernproc) return 0;

    return kernproc + kerndumpbase;
}

_pf64_external addr_t find_vnode_put_locked(void) {
    addr_t error_str = find_strref("\"vnode_put(%p): iocount < 1\"", 1, string_base_cstring, false, false);
    if (!error_str) return 0;
    error_str -= kerndumpbase;

    addr_t func = bof64(kernel, xnucore_base, error_str);
    if (!func) return 0;

    return func + kerndumpbase;
}
    
_pf64_external addr_t find_vnode_recycle(void) {
    addr_t vnode_put_locked = find_vnode_put_locked();
    if (!vnode_put_locked) return 0;
    vnode_put_locked -= kerndumpbase;

    addr_t tbnz=0, call=0;
    if (monolithic_kernel) {
        tbnz = step64(kernel, vnode_put_locked, 0x400, 0x37100000, 0xFFF80000);
        if (tbnz) call = step64(kernel, tbnz+4, 0x10, INSN_CALL);
    }
    if (!tbnz || !call) {
        addr_t mov = step64(kernel, vnode_put_locked, 0x100, 0xAA0003E1, 0xFFE0FFFF);
        if (!mov) return 0;

        call = step64(kernel, mov, 0x10, INSN_CALL);
        if (!call) return 0;

        addr_t vnop_inactive = follow_call64(kernel, call);
        if (!vnop_inactive) return 0;

        tbnz = step64(kernel, vnop_inactive, 0x100, 0x37100000, 0xFFF80000);
        if (!tbnz) return 0;

        call = step64(kernel, tbnz+4, 0x10, INSN_CALL);
        if (!call) return 0;
    }

    addr_t vnode_recycle = follow_call64(kernel, call);
    if (!vnode_recycle) return 0;
    return vnode_recycle + kerndumpbase;
}

_pf64_external addr_t find_lck_mtx_lock(void) {
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

_pf64_external addr_t find_lck_mtx_unlock(void) {
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

_pf64_external addr_t find_strlen(void) {
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

_pf64_external addr_t find_add_x0_x0_0x40_ret(void)
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

_pf64_external addr_t find_boottime(void) {
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


_pf64_external addr_t find_zfill(void)
{
    addr_t ref = find_strref("os reasons", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t mov_w1_100 = step64(kernel, ref+4, 0x20, 0x52800C81, 0xFFFFFFFF);
    if (!mov_w1_100) return 0;

    addr_t call_to_zfill = step64(kernel, mov_w1_100+4, 0x20, INSN_CALL);
    if (!call_to_zfill) call_to_zfill = step64(kernel, mov_w1_100+4, 0x20, INSN_B);
    if (!call_to_zfill) return 0;

    addr_t zfill = follow_call64(kernel, call_to_zfill);
    if (!zfill) return 0;

    return zfill + kerndumpbase;
}

_pf64_external addr_t find_zone_map(void)
{
    addr_t zfill = find_zfill();
    if (!zfill) return 0;
    zfill -= kerndumpbase;

    addr_t adrp = step64(kernel, zfill+4, 0x100, INSN_ADRP);
    if (!adrp) return 0;

    addr_t zone_map = calc64(kernel, adrp, adrp+8, 0);
    if (!zone_map) return 0;

    return zone_map + kerndumpbase;
}

_pf64_external addr_t find_OSBoolean_True(void)
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

_pf64_external addr_t find_osunserializexml(void)
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

_pf64_external addr_t find_smalloc(void)
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

_pf64_external addr_t find_shenanigans(void)
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

_pf64_external uint64_t find_boot_args(void) {
    /*
     ADRP            X8, #_PE_state@PAGE
     ADD             X8, X8, #_PE_state@PAGEOFF
     LDR             X8, [X8,#(PE_state__boot_args - 0xFFFFFFF0078BF098)]
     ADD             X8, X8, #0x6C
     STR             X8, [SP,#0x550+var_550]
     ADRP            X0, #aBsdInitCannotF@PAGE ; "\"bsd_init: cannot find root vnode: %s"...
     ADD             X0, X0, #aBsdInitCannotF@PAGEOFF ; "\"bsd_init: cannot find root vnode: %s"...
     BL              _panic
     */
    
    addr_t ref = find_strref("\"bsd_init: cannot find root vnode: %s\"", text_prelink_base, string_base_cstring, false, false);
    
    if (ref == 0) {
        return 0;
    }
    
    ref -= kerndumpbase;
    // skip add & adrp for panic str
    ref -= 8;
    uint32_t *insn = (uint32_t*)(kernel+ref);
    
    // skip str
    --insn;
    // add xX, xX, #cmdline_offset
    uint8_t xm = *insn&0x1f;
    if (((*insn>>5)&0x1f) != xm || ((*insn>>22)&3) != 0) {
        return 0;
    }
    
    cmdline_offset = (*insn>>10) & 0xfff;
    
    uint64_t val = kerndumpbase;
    
    --insn;
    // ldr xX, [xX, #(PE_state__boot_args - PE_state)]
    if ((*insn & 0xF9C00000) != 0xF9400000) {
        return 0;
    }
    // xd == xX, xn == xX,
    if ((*insn&0x1f) != xm || ((*insn>>5)&0x1f) != xm) {
        return 0;
    }
    
    val += ((*insn >> 10) & 0xFFF) << 3;
    
    --insn;
    // add xX, xX, #_PE_state@PAGEOFF
    if ((*insn&0x1f) != xm || ((*insn>>5)&0x1f) != xm || ((*insn>>22)&3) != 0) {
        return 0;
    }
    
    val += (*insn>>10) & 0xfff;
    
    --insn;
    if ((*insn & 0x1f) != xm) {
        return 0;
    }
    
    // pc
    val += ((uint8_t*)(insn) - kernel) & ~0xfff;
    
    // don't ask, I wrote this at 5am
    val += (*insn<<9 & 0x1ffffc000) | (*insn>>17 & 0x3000);
    
    return val;
}


/*
 *
 *
 *
 */

_pf64_external addr_t find_unix_syscall(void)
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

_pf64_external addr_t find_pthread_kext_register(void)
{
    addr_t ref = find_strref("\"Re-initialisation of pthread kext callbacks.\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    uint64_t start = bof64(kernel, xnucore_base, ref);
    if (!start) return 0;
    return start + kerndumpbase;
}

// This can be used to find any functions referenced by struct pthread_callbacks_s
_pf64_external addr_t find_pthread_callbacks(void)
{
    // Cache this one //
    static addr_t addr = 0;
    if (addr) {
        return addr + kerndumpbase;
    }

    addr_t ref = find_pthread_kext_register();
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t str = step64(kernel, ref, 0x50, 0xF9000020, 0xFFFFFFE0);
    if (!str) {
        addr_t mov = step64(kernel, ref, 0x20, 0xAA0103E0, 0xFFFFFFE0);
        if (!mov) return 0;

        int rn = *(uint32_t*)(kernel+mov) & 0x1f;
        str = step64(kernel, ref, 0x50, 0xF9000000 | (rn << 5), 0xFFFFFFE0);
    }

    if (!str) return 0;
    int rn = *(uint32_t*)(kernel+str) & 0x1f;

    addr = calc64(kernel, str-0x20, str, rn);
    if (!addr) return 0;
    return addr + kerndumpbase;
}

_pf64_external addr_t find_unix_syscall_return(void)
{
    addr_t pthread_callbacks = find_pthread_callbacks();
    if (!pthread_callbacks) return 0;
    pthread_callbacks -= kerndumpbase;

    addr_t addr = *(uint64_t*)(kernel + pthread_callbacks + 8 + 0x31 * 8);
    if (!addr) return 0;
    return remove_pac(addr);
}

_pf64_external addr_t find_kmod_start(void)
{
    addr_t ref = find_strref("kern_return_t kmod_start(kmod_info_t *, void *)", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_handler_map(void)
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

    addr_t cbz = step64(kernel, func, 0x30, INSN_CBZ);
    if (!cbz) return 0;

    int rn = *(uint32_t*)(kernel + cbz) & 0x1f;

    addr = calc64(kernel, cbz-0x8, cbz, rn);
    if (!addr) return 0;
    return addr + kerndumpbase;
}

_pf64_external addr_t find_policy_conf(void)
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

_pf64_external addr_t find_sandbox_handler(const char *name)
{
    addr_t handler_map = find_handler_map();
    if (!handler_map) return 0;
    handler_map -= kerndumpbase;

    struct {
        const char *name;
        uint64_t func;
        uint64_t unk;
    } *handler = (void *)(kernel + handler_map);

    addr_t func = 0;
    while (handler->name && handler->func) {
        const char *hname = (char*)remove_pac((addr_t)handler->name) - (char*)kerndumpbase + (char*)kernel;
        if (strcmp(hname, name) == 0) {
            func = remove_pac(handler->func);
            break;
        }
        handler++;
    }
    return func;
}

_pf64_external addr_t find_issue_extension_for_mach_service(void)
{
    return find_sandbox_handler("com.apple.security.exception.mach-lookup.global-name");
}

_pf64_external addr_t find_issue_extension_for_absolute_path(void)
{
    return find_sandbox_handler("com.apple.security.exception.files.absolute-path.read-only");
}

_pf64_external addr_t find_copy_path_for_vp(void)
{
    addr_t ref = find_strref("unknown path", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_external addr_t find_vn_getpath(void)
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

_pf64_external addr_t find_IOMalloc(void)
{
    addr_t start = find_smalloc();
    if (!start) return 0;
    start -= kerndumpbase;

    addr_t call = step64(kernel, start + 4, 0x50, INSN_CALL);
    if (!call) return 0;

    return follow_stub(kernel, call);
}

_pf64_external addr_t find_IOFree(void)
{
    addr_t start = find_sfree();
    if (!start) return 0;
    start -= kerndumpbase;

    addr_t insn = step64(kernel, start+4, 0x100, 0xAA0003E1, 0xFFE0FFFF);
    if (!insn) return 0;

    addr_t call = step64(kernel, insn+4, 0x10, INSN_CALL);
    if (!call) return 0;

    return follow_stub(kernel, call);
}

_pf64_external addr_t find_policy_ops(void)
{
    static struct mac_policy_conf *conf = NULL;
    if (!conf) {
        addr_t policy_conf_ref = find_policy_conf();
        if (!policy_conf_ref) return 0;
        conf = (struct mac_policy_conf *)KADDR_TO_BUF(kernel, policy_conf_ref);
    }

    addr_t ops = conf->mpc_ops;
    if (!ops) return 0;
    return remove_pac(ops);
}

_pf64_external addr_t find_mpo_entry(addr_t offset)
{
    addr_t ops = find_policy_ops();
    if (!ops) return 0;
    ops -= kerndumpbase;
    
    addr_t opref = *(addr_t *)(ops + kernel + offset);
    if (!opref) return 0;
    return remove_pac(opref);
}

_pf64_external addr_t find_hook_mount_check_snapshot_revert()
{
    return find_mpo(mount_check_snapshot_revert);
}

_pf64_external addr_t find_hook_vnode_check_exec()
{
    return find_mpo(vnode_check_exec);
}

_pf64_external addr_t find_hook_policy_init(void)
{
    return find_mpo(policy_init);
}

_pf64_external addr_t find_platform_profile(void)
{
    addr_t hook_policy_init = find_hook_policy_init();
    if (!hook_policy_init) return 0;
    hook_policy_init -= kerndumpbase;

    addr_t mov_x2_0 = step64(kernel, hook_policy_init, 0x400, 0xD2800002, 0xFFFFFFFF);
    if (!mov_x2_0) return 0;

    mov_x2_0 = step64(kernel, mov_x2_0+4, 0x200, 0xD2800002, 0xFFFFFFFF);
    if (!mov_x2_0) return 0;

    addr_t adrp_x1 = step64_back(kernel, mov_x2_0, 0x10, 0x90000001, 0x9F00001F);
    if (!adrp_x1) return 0;

    addr_t adrp_x0 = step64_back(kernel, adrp_x1, 0x10, 0x90000000, 0x9F00001F);
    if (!adrp_x0) return 0;

    addr_t platform_profile = calc64(kernel, adrp_x0, adrp_x1, 0);
    if (!platform_profile) return 0;

    return platform_profile + kerndumpbase;
}

_pf64_external addr_t find_hook_policy_syscall(int n)
{
    addr_t policy_syscall = find_mpo(policy_syscall);
    if (!policy_syscall) return 0;
    policy_syscall -= kerndumpbase;

    //uint32_t insn = *(uint32_t *)(kernel + policy_syscall);
    uint64_t insn = (uint64_t)step64(kernel, policy_syscall, 0x100, 0x7100003F, 0xFFC003FF);
    if (!insn) return 0;
    int len = (*(int*)(kernel+insn)>>10) & 0xFFF;
    if (n > len) return 0;

    addr_t adrp = step64(kernel, insn, 0x20, INSN_ADRP);
    addr_t adr = step64(kernel, insn, 0x20, 0x10000000, 0x9F000000);
    if (!adrp && !adr) return 0;

    addr_t ref = adrp?adrp:adr;

    int reg = *(uint32_t *)(kernel + ref) & 0x1f;
    addr_t jptbl = calc64(kernel, ref, ref+8, reg);
    if (!jptbl) return 0;
    uint16_t offset = *(uint16_t*)(kernel + jptbl);
    if ((*(uint32_t*)(kernel + jptbl) >> 16) != 0xffff) {
        if (ref == adr) return 0;
        reg = *(uint32_t *)(kernel + adr) & 0x1f;
        addr_t jumpbase = calc64(kernel, adr, adr+4, reg);
        uint16_t offset = *(uint16_t*)(kernel + jptbl + (n<<1));
        ref = jumpbase + (offset<<2);
    } else {
        ref = jptbl + *(int *)(kernel + jptbl + (n<<2));
    }

    addr_t call = step64(kernel, ref, 0x50, INSN_B);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_syscall_set_profile(void)
{
    return find_hook_policy_syscall(0);
}

_pf64_external addr_t find_syscall_check_sandbox(void)
{
    return find_hook_policy_syscall(2);
}

_pf64_external addr_t find_sandbox_set_container_copyin(void)
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

_pf64_external addr_t find_platform_set_container(void)
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

_pf64_external addr_t find_extension_create_file(void)
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

_pf64_external addr_t find_extension_add(void)
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

_pf64_external addr_t find_extension_release(void)
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

_pf64_external addr_t find_sfree(void)
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

_pf64_external addr_t find_sb_ustate_create(void)
{
    addr_t ref = find_strref("sb_ustate_create", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_sstrdup(void)
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

_pf64_external addr_t find_sysent(void)
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

_pf64_external addr_t find_syscall(int n)
{
    addr_t sysent = find_sysent();
    if (!sysent) return 0;
    sysent -= kerndumpbase;

    addr_t syscall = *(addr_t*)(kernel + sysent + 3 * sizeof(addr_t) * n);
    if (!syscall) return 0;
    return remove_pac(syscall);
}

_pf64_external addr_t find_proc_find(void) {
    addr_t getpgid = find_syscall(SYS_getpgid);
    if (!getpgid) return 0;
    getpgid -= kerndumpbase;

    addr_t call = step64(kernel, getpgid, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_proc_rele(void) {
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

_pf64_external addr_t find_move_snapshot_to_purgatory(void)
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

_pf64_external addr_t find_chgproccnt(void)
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

_pf64_external addr_t find_kauth_cred_ref(void)
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

_pf64_external addr_t find_apfs_jhash_getvnode(void)
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

_pf64_external addr_t find_fs_lookup_snapshot_metadata_by_name() {
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

_pf64_external addr_t find_fs_lookup_snapshot_metadata_by_name_and_return_name() {
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

_pf64_external addr_t find_mount_common() {
    uint64_t ref = find_strref("\"mount_common():", 1, string_base_cstring, false, false);
    if (!ref) return 0;
    ref -= kerndumpbase;
    uint64_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}


_pf64_external addr_t find_fs_snapshot() {
    uint64_t mount_common = find_mount_common();
    if (!mount_common) return 0;

    uint64_t ref = find_reference(mount_common, 5, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    uint64_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_vnode_get_snapshot() {
    uint64_t fs_snapshot = find_fs_snapshot();
    if (!fs_snapshot) return 0;
    fs_snapshot -= kerndumpbase;

    uint64_t mov = step64(kernel, fs_snapshot+4, 0x400, 0x320003E5, 0xFFFFFFFF);
    if (!mov) return 0;
    
    uint64_t call = step64(kernel, mov+4, 0x20, INSN_CALL);
    if (!call) return 0;

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

_pf64_external addr_t find_pmap_load_trust_cache() {
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

_pf64_external addr_t find_ppl_stubs() {
    if (!auth_ptrs) return 0;
    static addr_t ppl_stubs=0;
    if (ppl_stubs) return ppl_stubs + kerndumpbase;

    addr_t pmap_load_trust_cache = find_pmap_load_trust_cache();
    if (!pmap_load_trust_cache) return 0;
    pmap_load_trust_cache -= kerndumpbase;

    addr_t ref = follow_call64(kernel, pmap_load_trust_cache);
    if (!ref) return 0;

    addr_t start=0;
    while ((ref = step64_back(kernel, ref-4, 4, 0xD280000F, 0xFFE0001F))) {
        start = ref;
    }
    if (!start) return 0;
    return start + kerndumpbase;
}

_pf64_external addr_t find_ppl_stub(uint16_t id) {
    addr_t ppl_stubs = find_ppl_stubs();
    if (!ppl_stubs) return 0;
    ppl_stubs -= kerndumpbase;

    addr_t func=0;

    for (addr_t ref=ppl_stubs, insn=0;
            ((insn=*(uint32_t*)(kernel + ref))&0xFFE0001F) == 0xD280000F;
            ref += 8) {
        uint16_t value = (insn>>5)&0xffff;
        if (value == id) {
            func = ref;
            break;
        }
    }
    if (!func) return 0;
    return func + kerndumpbase;
}

_pf64_external addr_t find_pmap_cs_cd_register() {
    unsigned idx = 0x2a;
    if (find_ppl_stub(0x2f)) idx++;
    return find_ppl_stub(idx);
}

_pf64_external addr_t find_pmap_cs_cd_unregister() {
    unsigned idx = 0x2b;
    if (find_ppl_stub(0x2f)) idx++;
    return find_ppl_stub(idx);
}

_pf64_external addr_t find_paciza_pointer__l2tp_domain_module_start() {
    uint64_t string = (uint64_t)boyermoore_horspool_memmem(kernel + data_base, data_size, (const unsigned char *)"com.apple.driver.AppleSynopsysOTGDevice", strlen("com.apple.driver.AppleSynopsysOTGDevice")) - (uint64_t)kernel;
    
    if (!string) {
        return 0;
    }
    
    return string + kerndumpbase - 0x20;
}

_pf64_external addr_t find_paciza_pointer__l2tp_domain_module_stop() {
    uint64_t string = (uint64_t)boyermoore_horspool_memmem(kernel + data_base, data_size, (const unsigned char *)"com.apple.driver.AppleSynopsysOTGDevice", strlen("com.apple.driver.AppleSynopsysOTGDevice")) - (uint64_t)kernel;
    
    if (!string) {
        return 0;
    }
    
    return string + kerndumpbase - 0x18;
}

_pf64_external uint64_t find_l2tp_domain_inited() {
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

_pf64_external uint64_t find_sysctl__net_ppp_l2tp() {
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

_pf64_external uint64_t find_sysctl_unregister_oid() {
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

_pf64_external uint64_t find_mov_x0_x4__br_x5() {
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

_pf64_external uint64_t find_mov_x9_x0__br_x1() {
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

_pf64_external uint64_t find_mov_x10_x3__br_x6() {
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

_pf64_external uint64_t find_kernel_forge_pacia_gadget() {
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

_pf64_external uint64_t find_kernel_forge_pacda_gadget() {
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

_pf64_external uint64_t find_IOUserClient__vtable() {
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

_pf64_external uint64_t find_IORegistryEntry__getRegistryEntryID() {
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

_pf64_external addr_t find_check_for_signature() {
    addr_t ref = find_strref("CS Platform Exec Logging: Executing platform signed binary '%s' with cdhash %s\n", 1, string_base_oslstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, prelink_base, ref);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_internal struct sysctl_oid *find_sysctl_by_name(char *name, struct sysctl_oid_list* parent) {
    char localname[strlen(name)+1];
    strcpy(localname, name);
    if (!parent) parent = sysctl__children;
    if ((addr_t)parent > kerndumpbase) {
        parent = (struct sysctl_oid_list *)KADDR_TO_BUF(kernel, parent);
        rebase_sysctl_oid(kernel, (struct sysctl_oid *)parent);
    }

    char *next_level = strchr(localname, '.');
    if (next_level) {
        *next_level++ = '\0';
        struct sysctl_oid* new_parent_oid = find_sysctl_by_name(localname, parent);
        if (!new_parent_oid) return NULL;
        struct sysctl_oid_list* new_parent = new_parent_oid->oid_arg1;
        if (!new_parent) return NULL;
        return find_sysctl_by_name(next_level, new_parent);
    } else if (parent) {
        struct sysctl_oid *p;
        SLIST_FOREACH(p, parent, oid_link) {
            if ((addr_t)p > kerndumpbase) {
                p = (struct sysctl_oid *)KADDR_TO_BUF(kernel, p);
                rebase_sysctl_oid(kernel, p);
            }

            if (p && !strcmp(p->oid_name, localname)) {
                return p;
                break;
            }
        }
    }
    return NULL;
}

_pf64_external addr_t find_sysctl__vm_cs_blob_size_max() {
    struct sysctl_oid *vm_cs_blob_size_max = find_sysctl_by_name("vm.cs_blob_size_max", NULL);
    if (!vm_cs_blob_size_max) return 0;

    return (addr_t)vm_cs_blob_size_max - (addr_t)kernel + kerndumpbase;
}

_pf64_external addr_t find_cs_blob_generation_count() {
    addr_t sysctl__vm_cs_blob_size_max = find_sysctl__vm_cs_blob_size_max();
    if (!sysctl__vm_cs_blob_size_max) return 0;

    return sysctl__vm_cs_blob_size_max + 0x80;
}

_pf64_external addr_t find_ubc_cs_blob_add_site() {
    addr_t sysctl__vm_cs_blob_size_max = find_sysctl__vm_cs_blob_size_max();
    if (!sysctl__vm_cs_blob_size_max) return 0;

    return sysctl__vm_cs_blob_size_max + 0x68;
}

_pf64_external addr_t find_ubc_cs_blob_allocate_site() {
    addr_t sysctl__vm_cs_blob_size_max = find_sysctl__vm_cs_blob_size_max();
    if (!sysctl__vm_cs_blob_size_max) return 0;

    return sysctl__vm_cs_blob_size_max + 0x50;
}

_pf64_external addr_t find_ubc_cs_blob_add(void) {
    addr_t ref = find_strref("\"validated CodeDirectory but unsupported type\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;

    addr_t mov_xn_x7 = step64(kernel, func, 0x50, 0xAA0703E0, 0xFFFFFFE0);
    if (!mov_xn_x7) {
        // we actually found cs_blob_create_validated
        ref = find_reference(func + kerndumpbase, 1, text_xnucore_base);
        if (!ref) return 0;
        ref -= kerndumpbase;

        func = bof64(kernel, xnucore_base, ref);
        if (!func) return 0;
    }

    return func + kerndumpbase;
}

_pf64_external addr_t find_cs_blob_create_validated(void) {
    addr_t ref = find_strref("\"validated CodeDirectory but unsupported type\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t func = bof64(kernel, xnucore_base, ref);
    if (!func) return 0;

    addr_t mov_xn_x7 = step64(kernel, func, 0x50, 0xAA0703E0, 0xFFFFFFE0);
    if (mov_xn_x7) return 0; // we actually found ubc_cs_blob_add

    return func + kerndumpbase;
}

struct cs_hash {
    uint8_t     cs_type;    /* type code as per code signing */
    size_t      cs_size;    /* size of effective hash (may be truncated) */
    size_t      cs_digest_size; /* size of native hash */
    void*      cs_init;
    void*    cs_update;
    void*     cs_final;
};

uint64_t cs_hash_sizes[4] = {0x14, 0x20, 0x20, 0x30};

_pf64_external addr_t find_cs_find_md() {
    addr_t parent_func = find_cs_blob_create_validated();
    if (!parent_func) parent_func = find_ubc_cs_blob_add();
    if (!parent_func) return 0;
    parent_func -= kerndumpbase;

    // CMP Xn, #0x31
    addr_t insn = step64(kernel, parent_func+4, 0xc00, 0xF100C41F, 0xFFFFFC1F);
    if (!insn) return 0;

    addr_t ref = step64_back(kernel, insn-4, 0x20, INSN_ADRP);
    if (!ref) return 0;

    int reg = (*(uint32_t*)(kernel+ref+4)>>5) & 0x1f;
    addr_t addr = calc64(kernel, ref, ref+8, reg);
    if (!addr) return 0;

    // Validate
    for (int i=0; i<4; i++) {
        addr_t hashref = *(uint64_t*)(kernel+addr+i*8);
        struct cs_hash *hash = (struct cs_hash *)((hashref&0xffffffff) - (kerndumpbase&0xffffffff) + kernel);
        if (hash->cs_digest_size != cs_hash_sizes[i]) return 0;
    }

    return addr + kerndumpbase;
}


_pf64_external addr_t find_cs_validate_csblob() {
    addr_t parent_func = find_cs_blob_create_validated();
    if (!parent_func) parent_func = find_ubc_cs_blob_add();
    if (!parent_func) return 0;
    parent_func -= kerndumpbase;
    
    addr_t insn_x3 = step64(kernel, parent_func, 0x200, 0x3, 0x1f);
    if (!insn_x3) return 0;

    addr_t call = step64(kernel, insn_x3, 0x14, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_external addr_t find_kalloc_canblock() {
    addr_t pipe = find_syscall(SYS_pipe);
    if (!pipe) return 0;
    pipe -= kerndumpbase;

    addr_t call = step64(kernel, pipe, 0x50, INSN_CALL);
    if (!call) return 0;

    return follow_call64(kernel, call) + kerndumpbase;
}

_pf64_external addr_t find_backtrace_sysctl(void) {
    struct sysctl_oid *backtrace_user = find_sysctl_by_name("kern.backtrace.user", NULL);
    if (!backtrace_user) return 0;

    return (addr_t)backtrace_user->oid_handler - (addr_t)kernel + kerndumpbase;
}

_pf64_external addr_t find_kfree() {
    //kalloc_canblock
    uint64_t ref = find_strref("\"kfree: size %lu > kalloc_largest_allocated %lu\"", 1, string_base_cstring, true, false);
    if (ref) {
        ref -= kerndumpbase;

        addr_t start = bof64(kernel, xnucore_base, ref);
        if (!start) return 0;

        return start + kerndumpbase;
    }

    ref = find_strref("\"validated CodeDirectory but unsupported type\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t call = step64_back(kernel, ref, 0x100, INSN_CALL);
    if (!call) return 0;

    addr_t func = follow_call64(kernel, call);
    if (!func) return 0;

    return func + kerndumpbase;
}

_pf64_external addr_t find_hook_cred_label_update_execve() {
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

_pf64_external addr_t find_flow_divert_connect_out() {
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

_pf64_external addr_t find_kernel_memory_allocate() {
    addr_t ref = find_strref("\"kernel_memory_allocate: VM is not ready\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    uint64_t start = bof64(kernel, xnucore_base, ref);
    if (!start) return 0;

    return start + kerndumpbase;
}

_pf64_external addr_t find_kernel_map() {
    addr_t ipc_init_ref = find_strref("ipc_init", 1, string_base_cstring, true, false);
    if (!ipc_init_ref) return 0;
    ipc_init_ref -= kerndumpbase;

    addr_t ref = find_strref("\"ipc_init: kmem_suballoc of ipc_kernel_map failed\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base, call_to_kmem_suballoc;
    if (ref < ipc_init_ref || (ref-ipc_init_ref) > 0x200) {
        base = bof64(kernel, xnucore_base, ref);
        if (!base) return 0;

        call_to_kmem_suballoc = step64(kernel, base, 0x50, INSN_CALL);
    } else {
        base = bof64(kernel, xnucore_base, ipc_init_ref);
        if (!base) return 0;

        call_to_kmem_suballoc = step64_back(kernel, ref, 0x50, INSN_CALL);
    }
    if (!call_to_kmem_suballoc) return 0;

    addr_t kernel_map = calc64(kernel, base, call_to_kmem_suballoc, 0);
    if (kernel_map < 0x4000) return 0;

    return kernel_map + kerndumpbase;
}

_pf64_external addr_t find_ipc_space_kernel() {
    addr_t ref = find_strref("ipc kmsgs", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t insn = step64(kernel, ref, 0x100, 0x12800000, 0xffffffe0);
    if (!insn) return 0;

    insn = step64(kernel, insn+4, 0x10, INSN_ADRP);
    if (!insn) return 0;

    char reg = *(uint32_t*)(kernel + insn) & 0x1f;
    addr_t pageoff = calc64(kernel, insn, insn+4, reg);
    if (!pageoff) return 0;

    uint32_t str = *(uint32_t*)(kernel + insn + 4);
    if ( (str&0xFFC003E0) != (0xF9000000|(reg<<5)) ) return 0;
    ref = pageoff + (((str>>10)&0xfff)<<3);
    return ref + kerndumpbase;
}

_pf64_external addr_t find_socketops(void) {
    addr_t socketpair = find_syscall(SYS_socketpair);
    if (!socketpair) return 0;
    socketpair -= kerndumpbase;

    addr_t str = step64(kernel, socketpair, 0x300, 0xF9001400, 0xFFFFFC00);
    if (!str) return 0;

    int rn = *(uint32_t*)(kernel+str) & 0x1f;

    addr_t ref = calc64(kernel, str-0x10, str, rn);
    if (!ref) return 0;

    return ref + kerndumpbase;
}

_pf64_external addr_t find_copyin(void) {
    addr_t setrlimit = find_syscall(SYS_setrlimit);
    if (!setrlimit) return 0;
    setrlimit -= kerndumpbase;

    addr_t ref = step64(kernel, setrlimit, 0x50, INSN_CALL);
    if (!ref) return 0;

    addr_t copyin = follow_call64(kernel, ref);
    if (!copyin) return 0;

    return copyin + kerndumpbase;
}

_pf64_external addr_t find_copyout(void) {
    addr_t sigpending = find_syscall(SYS_sigpending);
    if (!sigpending) return 0;
    sigpending -= kerndumpbase;

    addr_t ref = step64(kernel, sigpending, 0x50, INSN_CALL);
    if (!ref) return 0;

    addr_t copyout = follow_call64(kernel, ref);
    if (!copyout) return 0;

    return copyout + kerndumpbase;
}

_pf64_external addr_t find_realhost(void) {
    addr_t ref = find_strref("Stackshot triggered using keycombo", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t call = step64(kernel, ref, 0x10, INSN_CALL);
    if (!call) return 0;

    call = step64(kernel, call+4, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t sysdiagnose_notify_user = follow_stub(kernel, call);
    if (!sysdiagnose_notify_user) return 0;
    sysdiagnose_notify_user -= kerndumpbase;

    call = step64(kernel, sysdiagnose_notify_user, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t addr = calc64(kernel, sysdiagnose_notify_user, call, 0);
    if (!addr) return 0;

    return addr + kerndumpbase;
}

_pf64_external addr_t find_proc_pid(void) {
    addr_t ref = find_strref("com.apple.wlan.authentication", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, prelink_base, ref);
    if (!base) return 0;

    // CMP W0, #0
    addr_t cmp_w0 = step64(kernel, base, 0x100, 0x7100001F, 0xFFFFFFFF);
    if (!cmp_w0) return 0;

    addr_t ref_to_proc_pid = step64_back(kernel, cmp_w0, 0x8, INSN_CALL);
    if (!ref_to_proc_pid) return 0;

    return follow_stub(kernel, ref_to_proc_pid);
}

_pf64_external addr_t find_mach_trap_table(void) {
    static addr_t mach_trap_table = 0;
    if (mach_trap_table) return mach_trap_table + kerndumpbase;

    addr_t ref = find_strref("\"Returned from exception_triage()?\\n\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, xnucore_base, ref);
    if (!base) return 0;

    // LDR Xn, [Xy, #8]
    addr_t ldr = step64_back(kernel, ref, ref-base, 0xF9400000|((8>>3)<<10), 0xFFFFFC00);
    if (!ldr) return 0;

    ref = step64_back(kernel, ldr, 0x10, INSN_ADRP);
    if (!ref) return 0;

    int rn = INSN(ref+4)&0x1f;

    mach_trap_table = calc64(kernel, ref, ref+8, rn);
    if (!mach_trap_table) return 0;

    return mach_trap_table + kerndumpbase;
}

typedef struct {
    uint64_t mach_trap_arg_count;
    uint64_t mach_trap_function;
    uint64_t mach_trap_arg_munge32;
    uint64_t mach_trap_u32_words;
    uint64_t mach_trap_name;
} mach_trap_t;

_pf64_external addr_t find_mach_trap(int n) {
    if (n>=0 || n < -127) return 0;
    n *= -1;

    uint64_t mach_trap_table = find_mach_trap_table();
    if (!mach_trap_table) return 0;
    mach_trap_table -= kerndumpbase;

    static size_t trap_size=0;
    if (!trap_size) {
        trap_size = sizeof(mach_trap_t) - sizeof(uint64_t);
        mach_trap_t *first_trap = (mach_trap_t *)(kernel + mach_trap_table);
        mach_trap_t *second_trap = (mach_trap_t *)(kernel + mach_trap_table + trap_size);
        if (first_trap->mach_trap_function != second_trap->mach_trap_function) {
            second_trap = (mach_trap_t *)((uint8_t*)second_trap + sizeof(uint64_t));
            if (first_trap->mach_trap_function != second_trap->mach_trap_function) return 0;
            trap_size += sizeof(uint64_t);
        }
    }

    mach_trap_t *trap = (mach_trap_t *)(kernel + mach_trap_table + n * trap_size);
    return remove_pac(trap->mach_trap_function);
}

_pf64_external addr_t find_task_for_pid(void) {
    return find_mach_trap(-45);
}

_pf64_external addr_t find_mach_reply_port(void) {
    return find_mach_trap(-26);
}

_pf64_external addr_t find__kernelrpc_mach_port_insert_right_trap(void) {
    return find_mach_trap(-21);
}

_pf64_external addr_t find_csops_internal(void) {
    addr_t csops = find_syscall(SYS_csops);
    if (!csops) return 0;
    csops -= kerndumpbase;

    addr_t call = step64(kernel, csops, 0x30, INSN_B);
    if (!call) return 0;

    addr_t csops_internal = follow_call64(kernel, call);
    if (!csops_internal) return 0;

    return csops_internal + kerndumpbase;
}

_pf64_external addr_t find_install_apfs_vfs_fs(void) {
    addr_t ref = find_strref("install_apfs_vfs_fs", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, prelink_base, ref);
    if (!base) return 0;

    return base + kerndumpbase;
}

_pf64_external addr_t find_apfs_sysctl_register(void) {
    addr_t ref = find_strref("apfs_sysctl_register", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t apfs_sysctl_register_addr = bof64(kernel, prelink_base, ref);
    if (!apfs_sysctl_register_addr) return 0;

    ref = find_strref("install_apfs_vfs_fs", 1, string_base_pstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t install_apfs_vfs_fs = bof64(kernel, prelink_base, ref);
    if (!install_apfs_vfs_fs || install_apfs_vfs_fs == apfs_sysctl_register_addr) return 0;

    return apfs_sysctl_register_addr + kerndumpbase;
}

struct apfs_sysctl_list_entry {
    struct sysctl_oid *entry;
    SLIST_ENTRY(apfs_sysctl_list_entry) next;
};

SLIST_HEAD(apfs_sysctl_list, apfs_sysctl_list_entry);

_pf64_internal bool apfs_sysctl_register(void) {
    addr_t sysctl_register_oid_call, adrp;
    addr_t apfs_sysctl_register_addr = find_apfs_sysctl_register();
    if (!apfs_sysctl_register_addr) { // inlined? :(
        addr_t ref = find_strref("apfs_sysctl_register", 1, string_base_pstring, true, false);
        if (!ref) return false;
        ref -= kerndumpbase;

        sysctl_register_oid_call = step64_back(kernel, ref, 0x20, INSN_CALL);
        if (!sysctl_register_oid_call) return false;

        adrp = step64_back(kernel, sysctl_register_oid_call, 0x20, INSN_ADRP);
        if (!adrp) return false;

    } else {
        apfs_sysctl_register_addr -= kerndumpbase;

        adrp = step64(kernel, apfs_sysctl_register_addr, 0x20, INSN_ADRP);
        if (!adrp) return false;

        sysctl_register_oid_call = step64(kernel, adrp, 0x20, INSN_CALL);
        if (!sysctl_register_oid_call) return false;
    }

    struct apfs_sysctl_list *sysctl_list = (struct apfs_sysctl_list *)calc64(kernel, adrp, sysctl_register_oid_call, 0);
    if (!sysctl_list) return false;
    sysctl_list = (struct apfs_sysctl_list *)((addr_t)sysctl_list + kernel);
    SLIST_FIRST(sysctl_list) = (struct apfs_sysctl_list_entry *)KADDR_TO_BUF(kernel, remove_pac(SLIST_FIRST(sysctl_list)));
    if (!SLIST_NEXT(SLIST_FIRST(sysctl_list), next)) {
        // Older kernels just pointed at the first entry
        struct apfs_sysctl_list_entry *head = (struct apfs_sysctl_list_entry *)sysctl_list;
        sysctl_list = (struct apfs_sysctl_list *)&head;
        // Reset this because we expect to clean it up later
        head->entry = (struct sysctl_oid *)((addr_t)head->entry - (addr_t)kernel + kerndumpbase);
    }

    struct apfs_sysctl_list_entry *p;
    SLIST_FOREACH(p, sysctl_list, next) {
        p->entry = (struct sysctl_oid *)KADDR_TO_BUF(kernel, remove_pac(p->entry));
        SLIST_NEXT(p, next) = (struct apfs_sysctl_list_entry *)KADDR_TO_BUF(kernel, remove_pac(SLIST_NEXT(p, next)));
        struct sysctl_oid *entry = p->entry;
        if (!register_sysctl_oid(kernel, entry)) {
            fprintf(stderr, "failed to register entry at %llx\n", (addr_t)entry - (addr_t)kernel + kerndumpbase);
            return false;
        }
    }
    return true;
}

struct vfs_fsentry
{
    struct vfsops *vfe_vfsops;
    int vfe_vopcnt;
    struct vnodeopv_desc **vfe_opvdescs;
    int vfe_fstypenum;
    char vfe_fsname[15];
    uint32_t vfe_flags;
    void *vfe_reserv[2];
};

_pf64_external addr_t find_apfs_vnodeop_opv_desc_list(void) {
    static addr_t apfs_vnodeop_opv_desc_list = 0;
    if (apfs_vnodeop_opv_desc_list) return apfs_vnodeop_opv_desc_list + kerndumpbase;

    addr_t install_apfs_vfs_fs = find_install_apfs_vfs_fs();
    if (!install_apfs_vfs_fs) return 0;
    install_apfs_vfs_fs -= kerndumpbase;

    // ADD             X0, SP, #vfe_off
    addr_t add_x0_sp = step64(kernel, install_apfs_vfs_fs, 0x40, 0x91000000|(0x1f<<5), 0xFFC003FF);
    if (!add_x0_sp) return 0;

    unsigned delta = (INSN(add_x0_sp) >> 10) & 0xFFF;

    // STR Xn, [SP, #vfe.vfe_vopcnt]
    addr_t str = step64(kernel, add_x0_sp, 0x50, 0xF9000000|(0x1f<<5)|((delta+offsetof(struct vfs_fsentry, vfe_opvdescs))<<(10-3)), 0xFFFFFFE0);
    if (!str) return 0;

    int rn = INSN(str)&0x1f;
    apfs_vnodeop_opv_desc_list = calc64(kernel, add_x0_sp, str, rn);
    if (!apfs_vnodeop_opv_desc_list) return 0;

    return apfs_vnodeop_opv_desc_list + kerndumpbase;
}

typedef struct vfs_context * vfs_context_t;
struct vfsops {
    addr_t vfs_mount;
    addr_t vfs_start;
    addr_t vfs_unmount;
    addr_t vfs_root;
    addr_t vfs_quotactl;
    addr_t vfs_getattr;
    addr_t vfs_sync;
    addr_t vfs_vget;
    addr_t vfs_fhtovp;
    addr_t vfs_vptofh;
    addr_t vfs_init;
    addr_t vfs_sysctl;
    addr_t vfs_setattr;
    addr_t vfs_ioctl;
    addr_t vfs_vget_snapdir;
    addr_t vfs_reserved5;
    addr_t vfs_reserved4;
    addr_t vfs_reserved3;
    addr_t vfs_reserved2;
    addr_t vfs_reserved1;
};

_pf64_external addr_t find_apfs_vfsops(void) {
    static addr_t apfs_vfsops = 0;
    if (apfs_vfsops) return apfs_vfsops + kerndumpbase;

    addr_t install_apfs_vfs_fs = find_install_apfs_vfs_fs();
    if (!install_apfs_vfs_fs) return 0;
    install_apfs_vfs_fs -= kerndumpbase;

    // ADD             X0, SP, #vfe_off
    addr_t add_x0_sp = step64(kernel, install_apfs_vfs_fs, 0x40, 0x91000000|(0x1f<<5), 0xFFC003FF);
    if (!add_x0_sp) return 0;

    unsigned delta = (INSN(add_x0_sp) >> 10) & 0xFFF;

    // STR Xn, [SP, #vfe.vfe_vopcnt]
    addr_t str = step64(kernel, add_x0_sp, 0x50, 0xF9000000|(0x1f<<5)|((delta+offsetof(struct vfs_fsentry, vfe_vfsops))<<(10-3)), 0xFFFFFFE0);
    if (!str) return 0;

    int rn = INSN(str)&0x1f;
    apfs_vfsops = calc64(kernel, add_x0_sp, str, rn);
    if (!apfs_vfsops) return 0;

    return apfs_vfsops + kerndumpbase;
}

struct vnodeop_desc {
    int vdesc_offset;       /* offset in vector--first for speed */
    addr_t vdesc_name;     /* a readable name for debugging */
    int vdesc_flags;        /* VDESC_* flags */

    /*
     *   * These ops are used by bypass routines to map and locate arguments.
     *       * Creds and procs are not needed in bypass routines, but sometimes
     *           * they are useful to (for example) transport layers.
     *               * Nameidata is useful because it has a cred in it.
     *                   */
    int *vdesc_vp_offsets;  /* list ended by VDESC_NO_OFFSET */
    int vdesc_vpp_offset;   /* return vpp location */
    int vdesc_cred_offset;  /* cred location, if any */
    int vdesc_proc_offset;  /* proc location, if any */
    int vdesc_componentname_offset; /* if any */
    int vdesc_context_offset;   /* context location, if any */
    /*
     *   * Finally, we've got a list of private data (about each operation)
     *       * for each transport layer.  (Support to manage this list is not
     *           * yet part of BSD.)
     *               */
    struct caddr_t *vdesc_transports;
};

struct vnodeopv_entry_desc {
    addr_t opve_op;   /* which operation this is */
    addr_t opve_impl;       /* code implementing this operation */
};

struct vnodeopv_desc {
    /* ptr to the ptr to the vector where op should go */
    int (***opv_desc_vector_p)(void *);
    struct vnodeopv_entry_desc *opv_desc_ops;   /* null terminated list */
};

enum apfv_vnop_types {
    APFS_VNODEOP = 0,
    APFS_FAKE_VNODEOP,
    APFS_SNAP_VNODEOP,
    APFS_FIFO_VNODEOP,
    APFS_STREAM_VNODEOP
};

_pf64_external addr_t find_apfs_vnop(enum apfv_vnop_types type, const char *func_name) {
    addr_t apfs_vnodeop_opv_desc_list = find_apfs_vnodeop_opv_desc_list();
    if (!apfs_vnodeop_opv_desc_list) return 0;
    apfs_vnodeop_opv_desc_list -= kerndumpbase;

    addr_t apfs_fake_vnodeop_opv_desc = *(addr_t*)(kernel + apfs_vnodeop_opv_desc_list + 8*type);
    if (!apfs_fake_vnodeop_opv_desc) return 0;
    apfs_fake_vnodeop_opv_desc = remove_pac(apfs_fake_vnodeop_opv_desc) - kerndumpbase;

    addr_t apfs_fake_vnodeop_entries = *(addr_t*)(kernel + apfs_fake_vnodeop_opv_desc + offsetof(struct vnodeopv_desc, opv_desc_ops));
    if (!apfs_fake_vnodeop_entries) return 0;
    apfs_fake_vnodeop_entries = remove_pac(apfs_fake_vnodeop_entries) - kerndumpbase;

    for (struct vnodeopv_entry_desc *opv_desc_ops = (struct vnodeopv_entry_desc *)(kernel + apfs_fake_vnodeop_entries);
            opv_desc_ops->opve_op != 0 && opv_desc_ops->opve_impl != 0; opv_desc_ops++) {
        struct vnodeop_desc *op = (struct vnodeop_desc *)(remove_pac(opv_desc_ops->opve_op) - kerndumpbase + kernel);
        uint64_t impl = remove_pac((addr_t)opv_desc_ops->opve_impl);

        char *name = (char*)(remove_pac(op->vdesc_name) - kerndumpbase + kernel);
        if (strcmp(name, func_name)==0) return impl;
    }
    return 0;
}

_pf64_external addr_t find_apfs_vfsop(size_t offset) {
    addr_t vfsops = find_apfs_vfsops();
    if (!vfsops) return 0;
    vfsops = vfsops - kerndumpbase;

    addr_t func = *(addr_t*)(kernel + vfsops + offset);
    if (!func) return 0;

    func = remove_pac(func);
    return func;
}

_pf64_external addr_t find_apfs_fake_vnop_blockmap(void) {
    return find_apfs_vnop(APFS_FAKE_VNODEOP, "vnop_blockmap");
}

_pf64_external addr_t find_apfs_vfsop_root(void) {
    return find_apfs_vfsop(offsetof(struct vfsops, vfs_root));
}

_pf64_external addr_t find_ipc_task_init(void) {
    addr_t ref = find_strref("\"ipc_task_init\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, xnucore_base, ref);
    if (!base) return 0;

    // Check if this is really ipc_task_init
    addr_t mov_xn_x2 = step64(kernel, base, 0x50, 0xAA0203E0, 0xFFFFFFE0);
    if (mov_xn_x2) return 0;

    return base + kerndumpbase;
}

_pf64_external addr_t find_ipc_space_create(void) {
    addr_t ipc_task_init = find_ipc_task_init();
    if (!ipc_task_init) return 0;
    ipc_task_init -= kerndumpbase;

    addr_t call = step64(kernel, ipc_task_init, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_space_create = follow_call64(kernel, call);
    if (!ipc_space_create) return 0;

    return ipc_space_create + kerndumpbase;
}

_pf64_external addr_t find_proc_pidfileportlist(void) {
    addr_t ref = find_strref("\"more fileports returned than requested\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t base = bof64(kernel, xnucore_base, ref);
    if (!base) return 0;

    return base + kerndumpbase;
}

_pf64_external addr_t find_fileport_walk(void) {
    addr_t proc_pidfileportlist = find_proc_pidfileportlist();
    if (!proc_pidfileportlist) return 0;
    proc_pidfileportlist -= kerndumpbase;

    // MOV Xn, X0
    addr_t mov_x0 = step64(kernel, proc_pidfileportlist, 0x100, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_x0) return -1;

    int rn = INSN(mov_x0)&0x1f;

    // LDR X0, [Xn, #off]
    addr_t ldr_x0 = step64(kernel, mov_x0, 0x200, 0xF9400000|(rn<<5), 0xFFC003FF);
    if (!ldr_x0) return -1;

    addr_t call = step64(kernel, ldr_x0, 0x20, INSN_CALL);
    if (!call) return 0;

    addr_t fileport_walk = follow_call64(kernel, call);
    if (!fileport_walk) return 0;

    return fileport_walk + kerndumpbase;
}

_pf64_external addr_t find_mach_port_names(void) {
    addr_t fileport_walk = find_fileport_walk();
    if (!fileport_walk) return 0;
    fileport_walk -= kerndumpbase;

    addr_t call = step64(kernel, fileport_walk, 0x100, INSN_CALL);
    if (!call) return 0;

    addr_t mach_port_names = follow_call64(kernel, call);
    if (!mach_port_names) return 0;

    return mach_port_names + kerndumpbase;
}

_pf64_external addr_t find_mig_e(void) {
    static addr_t mig_e = 0;
    if (mig_e) return mig_e + kerndumpbase;

    addr_t ref = find_strref("\"the msgh_ids in mig_e[] aren't valid!\"", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    // LDR Xn, [Xy, Xz, LSL#3]
    addr_t ldr_xn = step64_back(kernel, ref, 0x200, 0xF8607800, 0xFFE0FC00);
    if (!ldr_xn) return 0;

    int rn = (INSN(ldr_xn)>>5)&0x1f;

    mig_e = calc64(kernel, ldr_xn - 0x40, ldr_xn, rn);
    if (!mig_e) return 0;

    return mig_e + kerndumpbase;
}

_pf64_external addr_t find_mig_e_subsystem(unsigned start, unsigned end) {
    addr_t mig_e_addr = find_mig_e();
    if (!mig_e_addr) return 0;
    mig_e_addr -= kerndumpbase;

    // Test structure
    mig_subsystem_t *mig_e = (mig_subsystem_t *)(kernel + mig_e_addr);
    for (int i=0; i<30; i++) {
        addr_t ref = remove_pac((addr_t)mig_e[i]);
        if (!ref) break;
        ref -= kerndumpbase;

        if (ref < data_const_base || ref > data_const_base + data_const_size) break;

        mig_subsystem_t subsystem = (mig_subsystem_t)(kernel + ref);
        if (!subsystem->start || subsystem->start > subsystem->end) break;

        if (subsystem->start == start && subsystem->end >= end) return ref + kerndumpbase;
    }
    return 0;
}

_pf64_external addr_t find_mig_subsystem_routine(addr_t subsystem_addr, mach_msg_id_t id) {
    if (!subsystem_addr || !id) return 0;

    mig_subsystem_t subsystem = (mig_subsystem_t)(kernel + subsystem_addr);
    if (subsystem->start > id || subsystem->end < id ||
            !subsystem->start || subsystem->start > subsystem->end) return 0;

    mig_routine_descriptor *routine = &subsystem->routine[id - subsystem->start];

    addr_t impl = remove_pac((addr_t)routine->impl_routine);
    if (!impl) impl = remove_pac((addr_t)routine->stub_routine);
    return impl;
}

_pf64_external addr_t find_mach_vm_subsystem(void) {
    return find_mig_e_subsystem(0x12C0, 0x12D5);
}

_pf64_external addr_t find__Xmach_vm_deallocate(void) {
    addr_t mach_vm_subsystem = find_mach_vm_subsystem();
    if (!mach_vm_subsystem) return 0;
    mach_vm_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(mach_vm_subsystem, 0x12C1);
}

_pf64_external addr_t find_task_subsystem(void) {
    return find_mig_e_subsystem(0xD48, 0xD7C);
}

_pf64_external addr_t find__Xtask_terminate(void) {
    addr_t task_subsystem = find_task_subsystem();
    if (!task_subsystem) return 0;
    task_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(task_subsystem, 0xD49);
}

_pf64_external addr_t find__Xtask_suspend(void) {
    addr_t task_subsystem = find_task_subsystem();
    if (!task_subsystem) return 0;
    task_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(task_subsystem, 0xD4F);
}

_pf64_external addr_t find__Xtask_get_mach_voucher(void) {
    addr_t task_subsystem = find_task_subsystem();
    if (!task_subsystem) return 0;
    task_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(task_subsystem, 0xD6F);
}

_pf64_external addr_t find_mach_port_subsystem(void) {
    return find_mig_e_subsystem(0xC80, 0xCA4);
}

_pf64_external addr_t find__Xmach_port_type(void) {
    addr_t mach_port_subsystem = find_mach_port_subsystem();
    if (!mach_port_subsystem) return 0;
    mach_port_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(mach_port_subsystem, 0xc81);
}

_pf64_external addr_t find__Xmach_port_construct(void) {
    addr_t mach_port_subsystem = find_mach_port_subsystem();
    if (!mach_port_subsystem) return 0;
    mach_port_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(mach_port_subsystem, 0xc9f);
}

_pf64_external addr_t find__Xmach_port_allocate(void) {
    addr_t mach_port_subsystem = find_mach_port_subsystem();
    if (!mach_port_subsystem) return 0;
    mach_port_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(mach_port_subsystem, 0xc84);
}

_pf64_external addr_t find__Xmach_port_insert_right(void) {
    addr_t mach_port_subsystem = find_mach_port_subsystem();
    if (!mach_port_subsystem) return 0;
    mach_port_subsystem -= kerndumpbase;

    return find_mig_subsystem_routine(mach_port_subsystem, 0xc8e);
}

_pf64_external addr_t find_mach_port_allocate_full(void) {
    addr_t _Xmach_port_allocate = find__Xmach_port_allocate();
    if (!_Xmach_port_allocate) return 0;
    _Xmach_port_allocate -= kerndumpbase;

    // MOV X2, #0
    addr_t mov_x2_0 = step64(kernel, _Xmach_port_allocate, 0x100, 0xD2800002, 0xFFFFFFFF);
    if (!mov_x2_0) return 0;
    
    addr_t call = step64(kernel, mov_x2_0, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t mach_port_allocate_full = follow_call64(kernel, call);
    if (!mach_port_allocate_full) return 0;

    return mach_port_allocate_full + kerndumpbase;
}

_pf64_external addr_t find_convert_voucher_to_port(void) {
    addr_t _Xtask_get_mach_voucher = find__Xtask_get_mach_voucher();
    if (!_Xtask_get_mach_voucher) return 0;
    _Xtask_get_mach_voucher -= kerndumpbase;

    // MOV X0, #0
    addr_t mov_x0_0 = step64(kernel, _Xtask_get_mach_voucher, 0x100, 0xD2800000, 0xFFFFFFFF);
    if (!mov_x0_0) return 0;

    addr_t call = step64(kernel, mov_x0_0, 0x8, INSN_CALL);
    if (!call) return 0;

    addr_t convert_voucher_to_port = follow_call64(kernel, call);
    if (!convert_voucher_to_port) return 0;

    return convert_voucher_to_port + kerndumpbase;
}

_pf64_external addr_t find_ipc_port_alloc_special(void) {
    addr_t convert_voucher_to_port = find_convert_voucher_to_port();
    if (!convert_voucher_to_port) return 0;
    convert_voucher_to_port -= kerndumpbase;

    addr_t mov_x1_rn = step64(kernel, convert_voucher_to_port, 0x100, 0xAA0003E1, 0xFFE0FFFF);

    addr_t parent = convert_voucher_to_port;
    if (mov_x1_rn) {
        addr_t call = step64(kernel, mov_x1_rn, 0x10, INSN_CALL);
        if (!call) return 0;

        addr_t ipc_kobject_make_send_lazy_alloc_port = follow_call64(kernel, call);
        if (!ipc_kobject_make_send_lazy_alloc_port) return 0;

        parent = ipc_kobject_make_send_lazy_alloc_port;
    }

    addr_t call = step64(kernel, parent, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_port_alloc_special = follow_call64(kernel, call);
    if (!ipc_port_alloc_special) return 0;

    addr_t mov_xn_x1 = step64(kernel, ipc_port_alloc_special, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    // Still didn't find it :(
    if (mov_xn_x1) return 0;

    return ipc_port_alloc_special + kerndumpbase;
}

_pf64_external addr_t find_ipc_object_copyout_name(void) {
    addr_t _Xmach_port_insert_right = find__Xmach_port_insert_right();
    if (!_Xmach_port_insert_right) return 0;
    _Xmach_port_insert_right -= kerndumpbase;

    addr_t call = step64(kernel, _Xmach_port_insert_right, 0x100, INSN_CALL);
    if (!call) return 0;

    addr_t mov_xn_x0 = step64(kernel, call, 0x10, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return 0;
    int rn = INSN(mov_xn_x0)&0x1f;

    addr_t mov_x0_xn = step64(kernel, mov_xn_x0, 0x100, 0xAA0003E0|(rn<<16), 0xFFFFFFFF);
    if (!mov_x0_xn) return 0;

    call = step64(kernel, mov_x0_xn, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_object_copyout_name = follow_call64(kernel, call);
    if (!ipc_object_copyout_name) return 0;

    return ipc_object_copyout_name + kerndumpbase;
}

_pf64_external addr_t find_ipc_right_reverse(void) {
    addr_t ipc_object_copyout_name = find_ipc_object_copyout_name();
    if (!ipc_object_copyout_name) return 0;
    ipc_object_copyout_name -= kerndumpbase;

    addr_t mov_xn_x1 = step64(kernel, ipc_object_copyout_name, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov_xn_x1) return 0;
    int rn = INSN(mov_xn_x1)&0x1f;

    addr_t mov_x1_xn = step64(kernel, mov_xn_x1, 0x100, 0xAA1003E1|(rn<<16), 0xFFFFFFFF);
    if (!mov_x1_xn) return 0;

    addr_t call = step64(kernel, mov_x1_xn, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_right_reverse = follow_call64(kernel, call);
    if (!ipc_right_reverse) return 0;

    return ipc_right_reverse + kerndumpbase;
}

_pf64_external addr_t find_vm_map_executable_immutable_verbose(void) {
    addr_t ref = find_strref("vm_map_executable_immutable_verbose", 1, string_base_cstring, true, false);
    if (!ref) return 0;
    ref -= kerndumpbase;

    addr_t call = step64(kernel, ref, 0x30, INSN_CALL);
    if (!call) return 0;

    addr_t vm_map_executable_immutable_verbose = calc64(kernel, call-0x100, call, 1);
    if (!vm_map_executable_immutable_verbose) return 0;

    return vm_map_executable_immutable_verbose + kerndumpbase;
}

_pf64_external addr_t find_ipc_object_alloc_name(void) {
    addr_t mach_port_allocate_full = find_mach_port_allocate_full();
    if (!mach_port_allocate_full) return 0;
    mach_port_allocate_full -= kerndumpbase;

    addr_t mov_w1_0 = step64(kernel, mach_port_allocate_full, 0x300, 0x52800001, 0xFFFFFFFF);
    if (!mov_w1_0) return 0;

    addr_t call = step64(kernel, mov_w1_0, 0x20, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_object_alloc_name = follow_call64(kernel, call);
    if (!ipc_object_alloc_name) return 0;

    return ipc_object_alloc_name + kerndumpbase;
}

_pf64_external addr_t find_kmem_free(void) {
    addr_t kfree = find_kfree();
    if (!kfree) return 0;
    kfree -= kerndumpbase;

    addr_t mov_xn_x1 = step64(kernel, kfree, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov_xn_x1) return 0;
    int rn = INSN(mov_xn_x1)&0x1f;

    addr_t mov_x2_xn = step64(kernel, mov_xn_x1, 0x300, 0xAA0003E2|(rn<<16), 0xFFFFFFFF);
    if (!mov_x2_xn) return 0;

    addr_t call = step64(kernel, mov_x2_xn, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t kmem_free = follow_call64(kernel, call);
    if (!kmem_free) return 0;

    return kmem_free + kerndumpbase;
}

_pf64_external addr_t find_ipc_right_info(void) {
    addr_t _Xmach_port_type = find__Xmach_port_type();
    if (!_Xmach_port_type) return 0;
    _Xmach_port_type -= kerndumpbase;

    // _convert_port_to_space
    addr_t cmn_wn_1 = step64(kernel, _Xmach_port_type, 0x80, 0x3100041F, 0xFFFFFC1F);
    if (!cmn_wn_1) {
        addr_t mov_xn_x1 = step64(kernel, _Xmach_port_type, 0x50, 0xAA0103E0, 0xFFFFFFE0);
        if (!mov_xn_x1) return 0;
        int rn = INSN(mov_xn_x1)&0x1f;

        addr_t add_x2_xn_imm = step64(kernel, mov_xn_x1, 0x60, 0x91000002|(rn<<5), 0xFFC003FF);
        if (!add_x2_xn_imm) return 0;

        addr_t call = step64(kernel, add_x2_xn_imm, 0x10, INSN_CALL);
        if (!call) return 0;

        addr_t mach_port_type = follow_call64(kernel, call);
        if (!mach_port_type) return 0;

        cmn_wn_1 = step64(kernel, mach_port_type, 0x80, 0x3100041F, 0xFFFFFC1F);
        if (!cmn_wn_1) return 0;
    }

    int rn = (INSN(cmn_wn_1)>>5)&0x1f;

    addr_t call=cmn_wn_1;
    for (int i=0; i<2; i++) {
        addr_t mov_x1_xn = step64(kernel, call+4, 0x50, 0xAA1003E1|(rn<<16), 0xFFFFFFFF);
        if (!mov_x1_xn) return 0;

        // ipc_right_lookup_write then ipc_right_info
        call = step64(kernel, mov_x1_xn, 0x10, INSN_CALL);
        if (!call) return 0;
    }

    addr_t ipc_right_info = follow_call64(kernel, call);
    if (!ipc_right_info) return 0;

    return ipc_right_info + kerndumpbase;
}

_pf64_external addr_t find_ipc_right_check(void) {
    addr_t ipc_right_info = find_ipc_right_info();
    if (!ipc_right_info) return 0;
    ipc_right_info -= kerndumpbase;

    addr_t mov_xn_x2 = step64(kernel, ipc_right_info, 0x50, 0xAA0203E0, 0xFFFFFFE0);
    if (!mov_xn_x2) return 0;

    int rn = INSN(mov_xn_x2)&0x1f;

    addr_t mov_x3_xn = step64(kernel, mov_xn_x2, 0x100, 0xAA0003E3|(rn<<16), 0xFFFFFFFF);
    if (!mov_x3_xn) return 0;

    addr_t call = step64(kernel, mov_x3_xn, 0x10, INSN_CALL);
    if (!call) return 0;

    addr_t ipc_right_check = follow_call64(kernel, call);
    if (!ipc_right_check) return 0;

    return ipc_right_check + kerndumpbase;
}

_pf64_external addr_t find_mach_port_construct(void) {
    addr_t _Xmach_port_construct = find__Xmach_port_construct();
    if (!_Xmach_port_construct) return 0;
    _Xmach_port_construct -= kerndumpbase;

    addr_t mov_xn_x1 = step64(kernel, _Xmach_port_construct, 0x50, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov_xn_x1) return 0;
    int rn = INSN(mov_xn_x1)&0x1f;

    addr_t add_x3_xn_imm = step64(kernel, mov_xn_x1, 0x100, 0x91000003|(rn<<5), 0xFFC003FF);
    if (!add_x3_xn_imm) return 0;

    addr_t call = step64(kernel, add_x3_xn_imm, 0x10, INSN_CALL);
    if (!call) return 0;
    
    addr_t mach_port_construct = follow_call64(kernel, call);
    if (!mach_port_construct) return 0;

    return mach_port_construct + kerndumpbase;
}

_pf64_external addr_t find_convert_port_to_task(void) {
    addr_t _Xtask_terminate = find__Xtask_terminate();
    if (!_Xtask_terminate) return 0;
    _Xtask_terminate -= kerndumpbase;

    addr_t call = step64(kernel, _Xtask_terminate, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t convert_port_to_task = follow_call64(kernel, call);
    if (!convert_port_to_task) return 0;
    return convert_port_to_task + kerndumpbase;
}

_pf64_external addr_t find_proc_info_internal(void) {
    addr_t proc_info = find_syscall(SYS_proc_info);
    fprintf(stderr, "proc_info: %llx\n", proc_info);
    if (!proc_info) return 0;
    proc_info -= kerndumpbase;

    addr_t call = step64(kernel, proc_info, 0x40, INSN_B);
    if (!call) return proc_info + kerndumpbase;

    addr_t proc_info_internal = follow_call64(kernel, call);
    if (!proc_info_internal) return 0;

    return proc_info_internal + kerndumpbase;
}

_pf64_external addr_t find_proc_pidinfo(void) {
    addr_t proc_info_internal = find_proc_info_internal();
    if (!proc_info_internal) return 0;
    proc_info_internal -= kerndumpbase;

    addr_t sw = follow_switch(kernel, proc_info_internal, 2);
    fprintf(stderr, "sw at %llx: %llx\n", proc_info_internal + kerndumpbase, sw + kerndumpbase);
    if (!sw) return 0;

    addr_t call = step64(kernel, sw, 0x20, INSN_B);
    if (!call) return 0;

    addr_t proc_pidinfo = follow_call64(kernel, call);
    if (!proc_pidinfo) return 0;

    return proc_pidinfo + kerndumpbase;
}

_pf64_external addr_t find_proc_pidshortbsdinfo(void) {
    addr_t proc_pidinfo = find_proc_pidinfo();
    fprintf(stderr, "proc_pidinfo: %llx\n", proc_pidinfo);
    if (!proc_pidinfo) return 0;
    proc_pidinfo -= kerndumpbase;

    addr_t proc_rele = find_proc_rele();
    fprintf(stderr, "proc_rele: %llx\n", proc_rele);
    if (!proc_rele) return 0;
    proc_rele -= kerndumpbase;

    addr_t ref = xref64(kernel, proc_pidinfo, proc_pidinfo+0x500, proc_rele);
    if (!ref) return 0;

    addr_t call_to_proc_security_policy = step64_back(kernel, ref-4, 0x20, INSN_CALL);
    if (!call_to_proc_security_policy) return 0;

    addr_t cbz = step64(kernel, call_to_proc_security_policy, 0x10, INSN_CBZ);
    if (!cbz) return 0;

    addr_t swloc = follow_cbz(kernel, cbz);
    if (!swloc) return 0;

    addr_t caseloc = follow_switch(kernel, swloc, 12);
    if (!caseloc) return 0;

    addr_t call = step64(kernel, caseloc, 0x20, INSN_CALL);
    if (!call) return 0;

    addr_t proc_pidshortbsdinfo = follow_call64(kernel, call);
    //fprintf(stderr, "proc_pidshortbsdinfo: %llx\n", proc_pidshortbsdinfo + kerndumpbase);
    if (!proc_pidshortbsdinfo) return 0;

    return proc_pidshortbsdinfo + kerndumpbase;
}

_pf64_external addr_t find_vm_map_deallocate(void) {
    addr_t _Xmach_vm_deallocate = find__Xmach_vm_deallocate();
    if (!_Xmach_vm_deallocate) return 0;
    _Xmach_vm_deallocate -= kerndumpbase;

    addr_t str_w0 = step64(kernel, _Xmach_vm_deallocate, 0x100, 0xB9000000, 0xFFC0001F);
    if (!str_w0) return 0;

    addr_t call_vm_map_deallocate = step64(kernel, str_w0, 0x12, INSN_CALL);
    if (!call_vm_map_deallocate) return 0;

    addr_t vm_map_deallocate = follow_call64(kernel, call_vm_map_deallocate);
    if (!vm_map_deallocate) return 0;

    return vm_map_deallocate + kerndumpbase;
}

_pf64_external addr_t find_exec_mach_imgact(void) {
    addr_t strref = find_strref("/dev/fd/%d", 1, string_base_cstring, true, false);
    if (!strref) return 0;
    strref -= kerndumpbase;

    addr_t exec_mach_imgact = bof64(kernel, xnucore_base, strref);
    if (!exec_mach_imgact) return 0;

    return exec_mach_imgact + kerndumpbase;
}

static addr_t _cached_offsetof_task_all_image_info_addr = 0;
static addr_t _cached_offsetof_task_all_image_info_size = 0;
_pf64_external addr_t find_task_set_dyld_info(void) {
    static addr_t task_set_dyld_info = 0;
    if (task_set_dyld_info) {
        return task_set_dyld_info + kerndumpbase;
    }
    addr_t exec_mach_imgact = find_exec_mach_imgact();
    if (!exec_mach_imgact) return 0;
    exec_mach_imgact -= kerndumpbase;

    addr_t vm_map_deallocate = find_vm_map_deallocate();
    if (!vm_map_deallocate) return 0;
    vm_map_deallocate -= kerndumpbase;

    // Search for beginning-ish of next func - first match is beginning of exec_mach_imgact
    addr_t next_func = step64(kernel, exec_mach_imgact, (size_t)(kernel+xnucore_base+xnucore_size - exec_mach_imgact), 0x510003FF, 0x7F8003FF);
    if (!next_func) return 0;
    next_func = step64(kernel, next_func+4, (size_t)(kernel+xnucore_base+xnucore_size - next_func+4), 0x510003FF, 0x7F8003FF);
    if (!next_func) return 0;

    addr_t ref = exec_mach_imgact;
    while (ref) {
        ref = xref64(kernel, ref+4, next_func, vm_map_deallocate);
        if (!ref) return 0;

        addr_t mov_x0_xn = step64(kernel, ref, 0x10, 0xAA0003E0, 0xFFE0FFFF);
        if (!mov_x0_xn) continue;

        addr_t mov_x1_0 = step64(kernel, mov_x0_xn+4, 4, 0xD2800001, 0xFFFFFFFF);
        if (!mov_x1_0) {
            // Inlined? :(
            int rn = (INSN(mov_x0_xn)>>16)&0x1f;

            addr_t str_q0 = step64(kernel, mov_x0_xn+4, 0x10, 0x3D800000, 0xFFFFFC1F);
            if (str_q0) {
                // ADD Xn, rn, #off
                addr_t add_x0_rn = step64(kernel, mov_x0_xn+4, 0x10, 0x91000000|(rn<<5), 0xFFC003C0);
                if (!add_x0_rn) return -1;

                unsigned delta = (INSN(add_x0_rn) >> 10) & 0xFFF;
                _cached_offsetof_task_all_image_info_addr = delta;
                _cached_offsetof_task_all_image_info_size = delta + 8;

            }
            addr_t str_xzr_rn = step64(kernel, mov_x0_xn+4, 0x10, 0xF900001F|(rn<<5), 0xFFC003FF);
            if (!str_xzr_rn) return 0;
            _cached_offsetof_task_all_image_info_addr = ((INSN(str_xzr_rn)>>10)&0xFFF)<<3;

            str_xzr_rn = step64(kernel, str_xzr_rn+4, 4, 0xF900001F|(rn<<5), 0xFFC003FF);
            if (!str_xzr_rn) return 0;

            _cached_offsetof_task_all_image_info_size = ((INSN(str_xzr_rn)>>10)&0xFFF)<<3;
            return 0;
        }

        addr_t call = step64(kernel, mov_x1_0, 0x10, INSN_CALL);
        if (!call) return 0;

        task_set_dyld_info = follow_call64(kernel, call);
        if (!task_set_dyld_info) return 0;

        return task_set_dyld_info + kerndumpbase;
    }
    return 0;
}

_pf64_external addr_t find_task_suspend(void) {
    addr_t _Xtask_suspend = find__Xtask_suspend();
    if (!_Xtask_suspend) return 0;
    _Xtask_suspend -= kerndumpbase;


    addr_t call_convert_port_to_task = step64(kernel, _Xtask_suspend, 0x50, INSN_CALL);
    if (!call_convert_port_to_task) return 0;

    addr_t call_task_suspend = step64(kernel, call_convert_port_to_task+4, 0x10, INSN_CALL);
    if (!call_task_suspend) return 0;

    addr_t task_suspend = follow_call64(kernel, call_task_suspend);
    if (!task_suspend) return 0;

    return task_suspend + kerndumpbase;
}

/*
 *  Offsets
 */

_pf64_external size_t offsetof_task_all_image_info_addr(void) {
    addr_t task_set_dyld_info = find_task_set_dyld_info();
    if (!task_set_dyld_info) {
        return _cached_offsetof_task_all_image_info_addr?_cached_offsetof_task_all_image_info_addr:-1;
    }

    task_set_dyld_info -= kerndumpbase;

    addr_t mov_xn_x0 = step64(kernel, task_set_dyld_info, 0x50, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return -1;
    int rn = INSN(mov_xn_x0)&0x1f;

    addr_t str_xn_xn = step64(kernel, mov_xn_x0+4, 0x100, 0xF9000000|(rn<<5), 0xFFC003E0);
    if (!str_xn_xn) return -1;

    size_t offset = ((INSN(str_xn_xn)>>10)&0xFFF)<<3;
    if (!offset) return -1;
    return offset;
}

_pf64_external size_t offsetof_task_all_image_info_size(void) {
    addr_t task_set_dyld_info = find_task_set_dyld_info();
    if (!task_set_dyld_info) {
        return _cached_offsetof_task_all_image_info_size?_cached_offsetof_task_all_image_info_size:-1;
    }
    task_set_dyld_info -= kerndumpbase;

    addr_t mov_xn_x0 = step64(kernel, task_set_dyld_info, 0x50, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return -1;
    int rn = INSN(mov_xn_x0)&0x1f;

    addr_t str_xn_xn = step64(kernel, mov_xn_x0+4, 0x100, 0xF9000000|(rn<<5), 0xFFC003E0);
    if (!str_xn_xn) return -1;

    str_xn_xn = step64(kernel, str_xn_xn+4, 0x4, 0xF9000000|(rn<<5), 0xFFC003E0);
    if (!str_xn_xn) return -1;

    size_t offset = ((INSN(str_xn_xn)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t offsetof_proc_p_pid(void) {
    addr_t proc_pid = find_proc_pid();
    if (!proc_pid) return -1;
    proc_pid -= kerndumpbase;

    // LDR W0, [X0, #offset]
    addr_t ldr_w0 = step64(kernel, proc_pid, 0x8, 0xB9400000, 0xFFC003FF);
    if (!ldr_w0) return -1;
    size_t offset = ((INSN(ldr_w0)>>10)&0xFFF)<<2;
    return offset;
}

_pf64_external size_t offsetof_proc_p_ucred(void) {
    addr_t link = find_syscall(SYS_link);
    //fprintf(stderr, "link at %llx\n", link);
    if (!link) return -1;
    link -= kerndumpbase;

    addr_t kernproc = find_kernproc();
    //fprintf(stderr, "kernproc at %llx\n", kernproc);
    if (!kernproc) return -1;
    kernproc -= kerndumpbase;

    addr_t call = xref64(kernel, link, link+0x100, kernproc);
    if (!call) {
        // not inlined
        addr_t call_to_vfs_context_current = step64(kernel, link, 0x20, INSN_CALL);
        if (!call_to_vfs_context_current) return -1;

        addr_t vfs_context_current = follow_call64(kernel, call_to_vfs_context_current);
       if (!vfs_context_current) return -1;
        call = xref64(kernel, vfs_context_current, vfs_context_current+0x100, kernproc);
    }
    //fprintf(stderr, "call at %llx\n", call + kerndumpbase);
    if (!call) return -1;

    // LDR Xn, [Xn, #_kernproc]
    uint32_t insn = INSN(call);
    //fprintf(stderr, "insn: %08x\n", insn);
    if ( (insn&0xFFC00000) != 0xF9400000 ) return -1;

    int rn = insn&0x1f;

    // LDR Xn, [Xn, #off]
    addr_t ldr_xn = step64(kernel, call+4, 0x4, 0xF9400000|rn|(rn<<5), 0xFFC003FF);
    if (!ldr_xn) return -1;

    size_t offset = ((INSN(ldr_xn)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t offsetof_proc_task(void) {
    addr_t task_for_pid = find_task_for_pid();
    if (!task_for_pid) return -1;
    task_for_pid -= kerndumpbase;

    // LDR Xn, [Xy, #off]
    addr_t ldr = step64(kernel, task_for_pid, 0x30, 0xF9400000, 0xFFC00000);
    if (!ldr) return -1;

    size_t offset = ((INSN(ldr)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t offsetof_proc_p_csflags(void) {
    addr_t csops_internal = find_csops_internal();
    if (!csops_internal) return -1;
    csops_internal -= kerndumpbase;

    addr_t orr = step64(kernel, csops_internal, 0x1000, 0x32140000, 0xFFFFFC00);
    if (!orr) return -1;

    int rn = (INSN(orr)>>5)&0x1f;

    // LDR Wn, [Xy, #off]
    addr_t ldr = step64_back(kernel, orr, 0x10, 0xB9400000|rn, 0xFFC0001F);
    if (!ldr) return -1;

    size_t offset = ((INSN(ldr)>>10)&0xFFF)<<2;
    return offset;
}

_pf64_internal size_t offsetin_PROC_UPDATE_CREDS_ONPROC_macro(int n) {
    addr_t setgid = find_syscall(SYS_setgid);
    if (!setgid) return -1;
    setgid -= kerndumpbase;

    // MOV Xn, X0
    addr_t mov_x0 = step64(kernel, setgid, 0x40, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_x0) return -1;

    int rn = INSN(mov_x0)&0x1f;

    // STR Wn, [Xn, #off]
    addr_t str_wn = step64(kernel, mov_x0, 0x200, 0xB9000000|(rn<<5), 0xFFC003E0);
    if (!str_wn) return -1;

    for (int i=0; i<n; i++) {
        str_wn = step64(kernel, str_wn+4, 0x60, 0xB9000000|(rn<<5), 0xFFC003E0);
        if (!str_wn) return -1;
    }

    size_t offset = ((INSN(str_wn)>>10)&0xFFF)<<2;
    return offset;
}

_pf64_external size_t offsetof_cred_cr_posix_cr_uid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(0);
}

_pf64_external size_t offsetof_cred_cr_posix_cr_gid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(1);
}

_pf64_external size_t offsetof_cred_cr_posix_cr_ruid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(2);
}

_pf64_external size_t offsetof_cred_cr_posix_cr_rgid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(3);
}

_pf64_external size_t offsetof_cred_cr_posix_cr_svuid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(4);
}

_pf64_external size_t offsetof_cred_cr_posix_cr_svgid(void) {
    return offsetin_PROC_UPDATE_CREDS_ONPROC_macro(5);
}

_pf64_external size_t offsetof_apfs_fsprivate_crypto_mount(void) {
    addr_t apfs_vfsop_root = find_apfs_vfsop_root();
    if (!apfs_vfsop_root) return 0;
    apfs_vfsop_root -= kerndumpbase;

    addr_t ldr = step64(kernel, apfs_vfsop_root, 0x30, 0xF9400000, 0xFFC00000);
    if (!ldr) return -1;

    size_t offset = ((INSN(ldr)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t offsetof_task_itk_space(void) {
    addr_t mach_reply_port = find_mach_reply_port();
    if (!mach_reply_port) return -1;
    mach_reply_port -= kerndumpbase;

    addr_t ldr_x0 = step64(kernel, mach_reply_port, 0x30, 0xF9400000, 0xFFC0001F);
    if (!ldr_x0) return -1;

    size_t offset = ((INSN(ldr_x0)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t sizeof_ipc_entry(void) {
    addr_t call_to_kalloc = 0;
    addr_t kalloc_canblock = find_kalloc_canblock();
    if (!kalloc_canblock) return 0;
    kalloc_canblock -= kerndumpbase;

    addr_t ipc_space_create = find_ipc_space_create();
    if (!ipc_space_create) {

        addr_t ref = find_strref("\"ipc_task_init\"", 1, string_base_cstring, true, false);
        if (!ref) return -1;
        ref -= kerndumpbase;

        call_to_kalloc = xref64(kernel, ref-0x100, ref, kalloc_canblock);
        if (!call_to_kalloc) return -1;
    } else {
        ipc_space_create -= kerndumpbase;

        call_to_kalloc = xref64(kernel, ipc_space_create, ipc_space_create+0x100, kalloc_canblock);
        if (!call_to_kalloc) return -1;
    }

    // ADD X0, SP, #off
    addr_t add_x0_sp = step64_back(kernel, call_to_kalloc, 0x10, 0x910003e0, 0xFFC003FF);
    if (!add_x0_sp) return -1;

    unsigned delta = (INSN(add_x0_sp) >> 10) & 0xFFF;

    // STR Xn, [SP, #off]
    addr_t str = step64_back(kernel, add_x0_sp, 0x50, 0xF9000000|(0x1f<<5)|delta<<(10-3), 0xFFFFFFE0);
    if (!str) return 0;
    int rn = INSN(str)&0x1f;

    addr_t ldr_wn = step64_back(kernel, str, 0x20, 0xB9400000|rn, 0xFFC0001F);
    if (!ldr_wn) return -1;

    uint32_t buffer[65]; // 65 insns
    memcpy(buffer, kernel + ldr_wn, str-ldr_wn);
    // MOV xn, #1
    buffer[0] = 0xD2800020 | rn;
        
    size_t offset = calc64((uint8_t*)buffer, 0, 65*sizeof(uint32_t), rn);
    return offset;
}

_pf64_external size_t offsetof_ipc_port_io_object(void) {
    addr_t ipc_port_alloc_special = find_ipc_port_alloc_special();
    if (!ipc_port_alloc_special) return -1;
    ipc_port_alloc_special -= kerndumpbase;

    // STR Dn, [Xn, #off]
    addr_t str_dn_xn = step64(kernel, ipc_port_alloc_special, 0x100, 0xFD000000, 0xFFC00000);
    if (!str_dn_xn) return -1;

    size_t offset = IMM(str_dn_xn, 10, 12, 3);
    return offset;
}

_pf64_external size_t offsetof_ipc_port_ip_srights(void) {
    addr_t ipc_right_info = find_ipc_right_check();
    if (!ipc_right_info) return -1;
    ipc_right_info -= kerndumpbase;

    addr_t mov_x1_xn = step64(kernel, ipc_right_info, 0x30, 0xAA0103E0, 0xFFFFFFE0);
    if (!mov_x1_xn) return -1;
    int obj_rn = INSN(mov_x1_xn)&0x1f;

    addr_t sub_wn_wn_1 = step64(kernel, mov_x1_xn, 0x100, 0x51000400, 0xFFFFFC00);
    int rn = INSN(sub_wn_wn_1)&0x1f;
    if (!sub_wn_wn_1 || rn != ((INSN(sub_wn_wn_1)>>5)&0x1f)) return -1;

    addr_t ldr_wn = step64_back(kernel, sub_wn_wn_1, 0x8, 0xB9400000|rn|(obj_rn<<5), 0xFFC003FF);
    if (!ldr_wn) return -1;

    size_t offset = ((INSN(ldr_wn)>>10)&0xFFF)<<2;
    return offset;
}

_pf64_external size_t offsetof_ipc_port_ip_receiver(void) {
    addr_t ipc_right_reverse = find_ipc_right_reverse();
    if (!ipc_right_reverse) return -1;
    ipc_right_reverse -= kerndumpbase;

    addr_t mov_xn_x0 = step64(kernel, ipc_right_reverse, 0x50, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return -1;
    int rn = INSN(mov_xn_x0)&0x1f;


    addr_t cmp_xy_xn = step64(kernel, mov_xn_x0, 0x100, 0xEB00001F|(rn<<16), 0xFFFFFC1F);
    if (!cmp_xy_xn) return -1;
    int ry = (INSN(cmp_xy_xn)>>5)&0x1f;
    // LDR Xy, [Xz, #off]
    // F94032C8
    addr_t ldr_xn_xy = step64_back(kernel, cmp_xy_xn, 0x10, 0xF9400000|ry, 0xFFC0001F);
    if (!ldr_xn_xy) return -1;

    size_t offset = ((INSN(ldr_xn_xy)>>10)&0xFFF)<<3;
    return offset;
    //A905D51F
    //  STP XZR, Xn, [Xy,#offset]
    //addr_t stp_xzr_xn_xy = step64(kernel, mov_xn_x0, 0x50, 

}

_pf64_external size_t offsetof_ipc_port_ip_kobject(void) {
    addr_t convert_port_to_task = find_convert_port_to_task();
    if (!convert_port_to_task) return -1;
    convert_port_to_task -= kerndumpbase;

    addr_t mov_xn_x0 = step64(kernel, convert_port_to_task, 0x50, 0xAA0003E0, 0xFFFFFFE0);
    if (!mov_xn_x0) return -1;
    int rn = INSN(mov_xn_x0)&0x1f;

    addr_t ldr_xy_xn = step64(kernel, mov_xn_x0, 0x100, 0xF9400000|(rn<<5), 0xFFC003E0);
    if (!ldr_xy_xn) return -1;

    size_t offset = ((INSN(ldr_xy_xn)>>10)&0xFFF)<<3;
    return offset;
}

_pf64_external size_t sizeof_the_real_platform_profile_data(void) {
    addr_t hook_policy_init = find_hook_policy_init();
    if (!hook_policy_init) return -1;
    hook_policy_init -= kerndumpbase;

    addr_t mov_x2_0 = step64(kernel, hook_policy_init, 0x400, 0xD2800002, 0xFFFFFFFF);
    if (!mov_x2_0) return -1;

    mov_x2_0 = step64(kernel, mov_x2_0+4, 0x200, 0xD2800002, 0xFFFFFFFF);
    if (!mov_x2_0) return -1;

    addr_t adrp_x1 = step64_back(kernel, mov_x2_0, 0x10, 0x90000001, 0x9F00001F);
    if (!adrp_x1) return -1;

    addr_t adrp_x0 = step64_back(kernel, adrp_x1, 0x10, 0x90000000, 0x9F00001F);
    if (!adrp_x0) return -1;

    addr_t call = step64(kernel, mov_x2_0, 0x20, INSN_CALL);
    if (!call) return -1;

    size_t size = calc64(kernel, adrp_x0, call, 3);
    if (!size) return -1;
    return size;
}

_pf64_internal size_t size_sbops(void) {
    addr_t platform_profile = find_platform_profile();
    if (!platform_profile) return -1;
    platform_profile += sizeof(addr_t);
    platform_profile -= kerndumpbase;
    size_t size = 0;
    while (true) {
        addr_t pointer = *(addr_t *)(kernel + platform_profile);
        if (!pointer) return -1;
        pointer = remove_pac(pointer);
        pointer -= kerndumpbase;
        char *string = (char *)(kernel + pointer);
        if (!string) return -1;
        if (!strcmp(string, "HOME")) break;
        platform_profile += sizeof(addr_t);
        size++;
    }
    return size;
}

_pf64_external const char **list_sbops(void) {
    addr_t platform_profile = find_platform_profile();
    if (!platform_profile) return NULL;
    platform_profile += sizeof(addr_t);
    platform_profile -= kerndumpbase;
    size_t count = size_sbops();
    if (count == -1) return NULL;
    const char **list = (const char **)calloc(count + 1, sizeof(const char *));
    if (list == NULL) return NULL;
    size_t index = 0;
    while (index < count) {
        addr_t pointer = *(addr_t *)(kernel + platform_profile);
        if (!pointer) {
            free(list);
            return NULL;
        }
        pointer = remove_pac(pointer);
        pointer -= kerndumpbase;
        const char *string = (char *)(kernel + pointer);
        if (!string) {
            free(list);
            return NULL;
        }
        list[index] = string;
        platform_profile += sizeof(addr_t);
        index++;
    }
    return list;
}

_pf64_external size_t offsetof_sbop(const char *sbop) {
    const char **sbops = list_sbops();
    if (sbops == NULL) return -1;
    size_t index = 0;
    for (const char **list = sbops; *list; list++) {
        if (strcmp(*list, sbop) == 0) break;
        index++;
    }
    free(sbops);
    return index;
}

_pf64_internal bool populate_proc_offsets(void) {
    addr_t proc_pidshortbsdinfo = find_proc_pidshortbsdinfo();
    if (!proc_pidshortbsdinfo) return 0;
    proc_pidshortbsdinfo -= kerndumpbase;

    addr_t ret = step64(kernel, proc_pidshortbsdinfo, 0x200, INSN_RET);
    fprintf(stderr, "ret, %llx\n", ret);
    if (!ret) return 0;

    addr_t call = step64_back(kernel, ret, 0x50, INSN_CALL);
    if (!call) return 0;

    addr_t ldr_wy_xn = step64(kernel, call, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    size_t offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_uid = offset;

    ldr_wy_xn = step64(kernel, ldr_wy_xn+4, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_gid = offset;

    ldr_wy_xn = step64(kernel, ldr_wy_xn+4, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_ruid = offset;

    ldr_wy_xn = step64(kernel, ldr_wy_xn+4, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_rgid = offset;

    ldr_wy_xn = step64(kernel, ldr_wy_xn+4, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_svuid = offset;

    ldr_wy_xn = step64(kernel, ldr_wy_xn+4, 0x8, 0xB9400000, 0xFFC00000);
    if (!ldr_wy_xn) return 0;
    offset = ((INSN(ldr_wy_xn)>>10)&0xFFF)<<2;
    proc_offsets.p_svgid = offset;

    return 1;
}

_pf64_external size_t offsetof_proc_p_uid(void) {
    if (!proc_offsets.p_uid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_uid;
}

_pf64_external size_t offsetof_proc_p_gid(void) {
    if (!proc_offsets.p_gid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_gid;
}

_pf64_external size_t offsetof_proc_p_ruid(void) {
    if (!proc_offsets.p_ruid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_ruid;
}

_pf64_external size_t offsetof_proc_p_rgid(void) {
    if (!proc_offsets.p_rgid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_rgid;
}

_pf64_external size_t offsetof_proc_p_svuid(void) {
    if (!proc_offsets.p_svuid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_svuid;
}

_pf64_external size_t offsetof_proc_p_svgid(void) {
    if (!proc_offsets.p_svgid && !populate_proc_offsets()) return -1;
    return proc_offsets.p_svgid;
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


_pf64_external addr_t
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
#include <mach/mach_error.h>
#include <mach/mach.h>

mach_port_t tfp0 = MACH_PORT_NULL;

vm_size_t kernel_read(vm_address_t addr, void *buf, vm_size_t size)
{
    //fprintf(stderr, "Reading kernel bytes 0x%lx-0x%lx", addr, addr + size);
    kern_return_t ret;
    mach_port_t kernel_task = tfp0;
    vm_size_t remainder = size,
              bytes_read = 0;

    // The vm_* APIs are part of the mach_vm subsystem, which is a MIG thing
    // and therefore has a hard limit of 0x1000 bytes that it accepts. Due to
    // this, we have to do both reading and writing in chunks smaller than that.
    for(vm_address_t end = addr + size; addr < end; remainder -= size)
    {
        size = remainder > 0xfff ? 0xfff : remainder;
        ret = vm_read_overwrite(kernel_task, addr, size, (vm_address_t)&((char*)buf)[bytes_read], &size);
        if(ret != KERN_SUCCESS || size == 0)
        {
            fprintf(stderr, "vm_read error: %s", mach_error_string(ret));
            break;
        }
        bytes_read += size;
        addr += size;
    }

    return bytes_read;
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: patchfinder64 _decompressed_kernel_image_\n");
        printf("iOS ARM64 kernel patchfinder\n");
        exit(EXIT_FAILURE);
    }
    addr_t kernel_base = strtoull(argv[1], NULL, 16);
    if (kernel_base == 0 && access(argv[1], F_OK) != 0) {
        printf("%s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }
    int rv;
    const addr_t vm_kernel_slide = 0;
    if (kernel_base) {
        fprintf(stderr, "kernel base %llx\n", kernel_base);
        task_for_pid(mach_task_self(), 0, &tfp0);
        if (!MACH_PORT_VALID(tfp0)) {
            printf("Failed to get tfp0\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "tfp0: %x\n", tfp0);
        if (init_kernel((kread_t)&kernel_read, kernel_base, NULL) != 0) {
            printf("Failed to prepare kernel\n");
            exit(EXIT_FAILURE);
        }
    } else {
        if (init_kernel(NULL, kernel_base, argv[1]) != 0) {
            printf("Failed to prepare kernel\n");
            exit(EXIT_FAILURE);
        }
    }
    if (!apfs_sysctl_register()) fprintf(stderr, "apfs_sysctl_register failed - apfs sysctls will be unavailable\n");
    printf("Testing: %s\n", kernel_version_string);

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
    CHECK(_Xtask_suspend);
    CHECK(task_suspend);
    printf("count: %zu offset: %zu\n", size_sbops(), offsetof_sbop("process-exec*"));
    CHECK(platform_profile);
    printf("size: 0x%zx\n", sizeof_the_real_platform_profile_data());
    CHECK(kernel_task);
    CHECK(convert_port_to_task);
    CHECK(_Xmach_vm_deallocate);
    CHECK(vm_map_deallocate);
    CHECK(exec_mach_imgact);
    fprintf(stderr, "offsetof_task_all_image_info_addr: 0x%zx\n", offsetof_task_all_image_info_addr());
    fprintf(stderr, "offsetof_task_all_image_info_size: 0x%zx\n", offsetof_task_all_image_info_size());
    if (offsetof_task_all_image_info_addr() == -1) CHECK(task_set_dyld_info);
    CHECK(apfs_sysctl_register);
#if 0
    printf("offset ipc_port.ip_kobject: 0x%zx\n", offsetof_ipc_port_ip_kobject());
    CHECK(_Xmach_port_insert_right);
    CHECK(_kernelrpc_mach_port_insert_right_trap);
    CHECK(ipc_object_copyout_name);
    CHECK(ipc_right_reverse);
    printf("offset ipc_port.ip_receiver: 0x%zx\n", offsetof_ipc_port_ip_receiver());
    CHECK(mach_port_construct);
    CHECK(ipc_right_check);
    printf("offset ipc_port.ip_srights: 0x%zx\n", offsetof_ipc_port_ip_srights());
    fprintf(stderr, "vm.cs_blob_size_max at %llx\n", (addr_t)find_sysctl_by_name("vm.cs_blob_size_max", NULL) - (addr_t)kernel + kerndumpbase);
    fprintf(stderr, "vfs.generic.apfs.allocated at %llx\n", (addr_t)find_sysctl_by_name("vfs.generic.apfs.allocated", NULL) - (addr_t)kernel + kerndumpbase);
    CHECK(cs_blob_generation_count);
    CHECK(ubc_cs_blob_add_site);
    CHECK(ubc_cs_blob_add);
    CHECK(kalloc_canblock);
    CHECK(kmem_free);
    CHECK(ipc_object_alloc_name);
    CHECK(mach_port_allocate_full);
    CHECK(_Xmach_port_allocate);
    if (kernel_major >= 18) CHECK(vm_map_executable_immutable_verbose);
    CHECK(mig_e);
    CHECK(mach_vm_subsystem);
    CHECK(mach_port_subsystem);
    CHECK(_Xmach_port_type);
    CHECK(mach_port_names);
    CHECK(mach_reply_port);
    CHECK(_Xtask_get_mach_voucher);
    CHECK(convert_voucher_to_port);
    CHECK(ipc_port_alloc_special);
    printf("offset ipc_port.io_object: 0x%zx\n", offsetof_ipc_port_io_object());
    printf("offset task.itk_space: 0x%zx\n", offsetof_task_itk_space());
    printf("sizeof ipc_entry: 0x%zx\n", sizeof_ipc_entry());

    printf("offset proc_p_uid: 0x%zx\n", offsetof_proc_p_uid());
    printf("offset proc_p_gid: 0x%zx\n", offsetof_proc_p_gid());
    printf("offset proc_p_ruid: 0x%zx\n", offsetof_proc_p_ruid());
    printf("offset proc_p_rgid: 0x%zx\n", offsetof_proc_p_rgid());
    printf("offset proc_p_svuid: 0x%zx\n", offsetof_proc_p_svuid());
    printf("offset proc_p_svgid: 0x%zx\n", offsetof_proc_p_svgid());
    CHECK(apfs_vnodeop_opv_desc_list);
    CHECK(apfs_fake_vnop_blockmap);
    CHECK(apfs_vfsop_root);
    printf("offset apfs/fsprivate.crypto_mount: 0x%zx\n", offsetof_apfs_fsprivate_crypto_mount());
    CHECK(csops_internal);
    printf("offset proc.p_csflags: 0x%zx\n", offsetof_proc_p_csflags());
    CHECK(proc_pid);
    printf("offset proc.p_pid: 0x%zx\n", offsetof_proc_p_pid());
    printf("offset proc.p_ucred: 0x%zx\n", offsetof_proc_p_ucred());
    CHECK(mach_trap_table);
    CHECK(task_for_pid);
    printf("offset proc.task: 0x%zx\n", offsetof_proc_task());
    printf("offset proc.p_uid: 0x%zx\n", offsetof_proc_p_uid());
    CHECK(amfi_is_cdhash_in_trust_cache);
    CHECK(realhost);
    CHECK(copyin);
    CHECK(copyout);
    CHECK(socketops);
    CHECK(ipc_space_kernel);
    CHECK(boot_args);
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
    CHECK(pthread_callbacks);
    CHECK(unix_syscall_return);
    CHECK(sfree);
    CHECK(sstrdup);
    CHECK(pthread_kext_register);
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
    CHECK(allproc);
    CHECK(vnode_recycle);
    CHECK(lck_mtx_lock);
    CHECK(lck_mtx_unlock);
    CHECK(strlen);
    CHECK(add_x0_x0_0x40_ret);
    if (kernel_major >= 19) {
        if (!auth_ptrs) CHECK(pmap_image4_trust_caches);
    } else {
        CHECK(trustcache);
    }
    CHECK(move_snapshot_to_purgatory);
    CHECK(apfs_jhash_getvnode);
    CHECK(zone_map);
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
        CHECK(ppl_stubs);
        CHECK(pmap_cs_cd_register);
        CHECK(pmap_cs_cd_unregister);
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
    CHECK(kernel_memory_allocate);
    CHECK(kernel_map);
    CHECK(ubc_cs_blob_add_site);
    CHECK(ubc_cs_blob_add);
    
#endif
    term_kernel();
    return EXIT_SUCCESS;
}

#endif	/* HAVE_MAIN */

// (C)2019 Sam Bingner


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <lzfse.h>
#include "lzssdec.h"

#ifdef __APPLE__

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#else
#include <endian.h>
#endif

#define FORMAT_ASN1     4                      /* ASN.1/DER */
#define IMAGE_LZSS       0x6C7A7373

static const uint64_t lzss_magic = 0x636f6d706c7a7373;

struct lzss_hdr {
    uint64_t magic;
    uint32_t checksum;
    uint32_t size;
    uint32_t src_size;
    uint32_t unk1;
    uint8_t padding[0x168];
};

// streaming version of the lzss algorithm, as defined in BootX-75/bootx.tproj/sl.subproj/lzss.c
// you can use lzssdec in a filter, like:
//
// cat file.lzss | lzssdec > file.decompressed
//
static FILE *_output=stdout;
static FILE *_input=stdin;

uint64_t read_asn1len(uint8_t len)
{
    uint8_t buf[8];
    uint64_t full_len = 0;
    if ((len & 0x80) != 0x80) 
        return len;

    int size = len&0x7F;
    if (size > 8) {
        fprintf(stderr, "Sorry, no support for kernel this large or not a kernel\n");
        exit(1);
    }
    size_t nr = fread(buf, 1, size, _input);
    if (nr != size) {
        perror("read");
        exit(1);
    }
    for (int i=0; i<size; i++) {
        full_len = full_len<<8 | buf[i];
    }
    return le64toh(full_len);
}

void read_asn1hdr(uint8_t *buf)
{
    if (fread(buf, 1, 2, _input) != 2) {
        perror("asn1hdr read");
        exit(1);
    }
}

char *read_asn1str() {
    uint8_t buf[2];
    read_asn1hdr(buf);
    if (*buf != 0x16) {
        fprintf(stderr, "Invalid input - not string (0x%02x)\n", *buf);
        exit(1);
    }

    uint64_t len = read_asn1len(buf[1]);

    char *str = (char*)calloc(len+1, 1);
    if (fread(str, 1, len, _input) != len) {
        perror("read");
        exit(1);
    }
    if (g_debug) fprintf(stderr, "read str \"%s\"\n", str);
    return str;
}

extern "C" int decompress_kernel(FILE *inputfh, FILE* outputfh, FILE *kppfh, bool quiet)
{
    if (inputfh)
        _input = inputfh;
    if (outputfh)
        _output = outputfh;
#define CHUNK 0x10000

    lzssdecompress lzss;
    uint8_t *ibuf= (uint8_t*)malloc(CHUNK);
    uint8_t *obuf= (uint8_t*)malloc(CHUNK);
    bzero(ibuf, CHUNK);
    size_t nr;

next:
    read_asn1hdr(ibuf);
    if (*ibuf != 0x30) {
        fprintf(stderr, "Invalid input - not IM4P %x\n", *ibuf);
        return 1;
    }

    uint64_t len = read_asn1len(ibuf[1]);

    if (g_debug) fprintf(stderr, "file length: %lld\n", len);

    char *str = read_asn1str();

    if (strcasecmp(str, "IMG4") == 0) {
        // Encapsulated in IMG4
        goto next;
    }

    if (strcasecmp(str, "IM4P")) {
        fprintf(stderr, "Invalid input - not IM4P (%s)\n", str);
        return 1;
    }

    if (g_debug) fprintf(stderr, "Have an %s container\n", str);

    free(str);

    str = read_asn1str();

    if (strcasecmp(str, "krnl")) {
        fprintf(stderr, "Invalid input - not Kernel (0x%02x)\n", *ibuf);
        return 1;
    }

    free(str);

    str = read_asn1str();

    read_asn1hdr(ibuf);

    if (*ibuf != 0x04) {
        fprintf(stderr, "Invalid input - no kernel data\n");
        return 1;
    }

    uint64_t data_len = read_asn1len(ibuf[1]);

    // See if it might be LZFSE
    char peek = fgetc(_input);
    ungetc(peek, _input);

    if (peek == 0x62) {
        // LZFSE?
        if (!quiet) fprintf(stderr, "Writing kernelcache...\n");
        int error;
        size_t decoded = lzfse_decode_file(_output, _input, data_len, &error, NULL);
        if (decoded <= 0) {
            perror("Unable to decode LZFSE kernel cache");
            return 1;
        }
    } else {
        // LZSS?
        struct lzss_hdr hdr;
        nr = fread(&hdr, 1, sizeof(struct lzss_hdr), _input);
        if (nr != sizeof(struct lzss_hdr)) {
            perror("read");
            return 1;
        }

        hdr.magic = be64toh(hdr.magic);
        if (hdr.magic != lzss_magic) {
            fprintf(stderr, "Invalid input - no lzss magic 0x%llx\n", hdr.magic);
            return 1;
        }

        hdr.size = be32toh(hdr.size);
        hdr.src_size = be32toh(hdr.src_size);
        if (g_debug) fprintf(stderr, "Found kernelcache size %u compressed: %u (asn1 size %lld)\n", hdr.size, hdr.src_size, data_len);
        if (hdr.src_size > data_len - sizeof(struct lzss_hdr)) {
            fprintf(stderr, "Invalid input - reports size larger than available\n");
            return 1;
        }

        uint64_t total_written=0;
        uint64_t total_read=0;
        if (!quiet) fprintf(stderr, "Writing kernelcache...\n");
        while (!feof(_input) && total_read < hdr.src_size)
        {
            if (total_read + CHUNK > hdr.src_size) {
                nr = fread(ibuf, 1, hdr.src_size - total_read, _input);
            } else {
                nr = fread(ibuf, 1, CHUNK, _input);
            }
            if (nr==0) {
                perror("input file short read");
                break;
            }

            total_read += nr;
            size_t srcp= 0;
            while (srcp<nr) {
                uint32_t dstused;
                uint32_t srcused;
                lzss.decompress(obuf, CHUNK, &dstused, ibuf+srcp, nr-srcp, &srcused);
                srcp+=srcused;
                if (total_written + dstused > hdr.size) {
                    dstused = hdr.size - total_written;
                }
                size_t nw= fwrite(obuf, 1, dstused, _output);
                if (nw<dstused) {
                    perror("write");
                    return 1;
                }
                total_written += nw;
                if (g_debug) fprintf(stderr, "decompress: 0x%x -> 0x%x\n", srcused, dstused);
            }
        }
    }
    if (!quiet) fprintf(stderr, "... done\n");
    if (kppfh != NULL) {
        while ((nr = fread(ibuf, 1, CHUNK, _input))) {
            if (fwrite(ibuf, 1, nr, kppfh) != nr) {
                perror("write kpp:");
                return 1;
            }
        }
    }
    if (g_debug) fprintf(stderr, "done reading\n");
    uint32_t dstused;
    lzss.flush(obuf, CHUNK, &dstused);
    size_t nw = fwrite(obuf, 1, dstused, _output);
    if (nw<dstused) {
        perror("write");
        return 1;
    }

    if (g_debug) fprintf(stderr, "flush: %d bytes\n", dstused);

    return 0;
}

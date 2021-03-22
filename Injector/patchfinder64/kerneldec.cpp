// (C)2009 Willem Hengeveld  itsme@xs4all.nl
// Modifications (C)2019 Sam Bingner
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
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
#include <algorithm>

# define FORMAT_ASN1     4                      /* ASN.1/DER */
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
static int g_debug= 0;
static FILE *output=stdout;
static FILE *input=stdin;

class lzssdecompress
{
    enum { COPYFROMDICT, EXPECTINGFLAG, PROCESSFLAGBIT, EXPECTING2NDBYTE };
    int _state;
    uint8_t _flags;
    int _bitnr;
    uint8_t *_src, *_srcend;
    uint8_t *_dst, *_dstend;
    uint8_t _firstbyte;

    uint8_t *_dict;

    int _dictsize;
    int _maxmatch;
    int _copythreshold;

    int _dictptr;

    int _copyptr;
    int _copycount;

    int _inputoffset;
    int _outputoffset;
public:
    lzssdecompress()
    {
        _maxmatch= 18;  // 4 bit size + threshold 
        _dictsize= 4096; // 12 bit size
        _copythreshold= 3; // 0 == copy 3 bytes
        _dict= new uint8_t[_dictsize+_maxmatch-1];

        reset();
    }
    ~lzssdecompress()
    {
        delete[] _dict;
        _dict= 0; _dictsize= 0;
    }
    void reset()
    {
        _state=EXPECTINGFLAG;
        _flags= 0; _bitnr= 0;
        _src=_srcend=_dst=_dstend=0;
        memset(_dict, ' ', _dictsize+_maxmatch-1);
        _dictptr= _dictsize-_maxmatch;
        _inputoffset= 0;
        _outputoffset= 0;
        _firstbyte= 0;
        _copyptr= 0;
        _copycount= 0;
    }
    void decompress(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused, uint8_t *src, uint32_t srclen, uint32_t *psrcused)
    {
        _src= src;  _srcend= src+srclen;
        _dst= dst;  _dstend= dst+dstlen;

        while (_src<_srcend && _dst<_dstend)
        {
            switch(_state)
            {
                case EXPECTINGFLAG:
                    if (g_debug) fprintf(stderr, "%08x,%08x: flag: %02x\n", _inputoffset, _outputoffset, *_src);
                    _flags= *_src++;
                    _inputoffset++;
                    _bitnr= 0;
                    _state= PROCESSFLAGBIT;
                    break;
                case PROCESSFLAGBIT:
                    if (_flags&1) {
                        if (g_debug) fprintf(stderr, "%08x,%08x: bit%d: %03x copybyte %02x\n", _inputoffset, _outputoffset, _bitnr, _dictptr, *_src);
                        addtodict(*_dst++ = *_src++);
                        _inputoffset++;
                        _outputoffset++;
                        nextflagbit();
                    }
                    else {
                        _firstbyte= *_src++;
                        _inputoffset++;
                        _state= EXPECTING2NDBYTE;
                    }
                    break;
                case EXPECTING2NDBYTE:
                    {
                    uint8_t secondbyte= *_src++;
                    _inputoffset++;
                    setcounter(_firstbyte, secondbyte);
                    if (g_debug) fprintf(stderr, "%08x,%08x: bit%d: %03x %02x %02x : copy %d bytes from %03x", _inputoffset-2, _outputoffset, _bitnr, _dictptr, _firstbyte, secondbyte, _copycount, _copyptr);
                    if (g_debug) dumpcopydata();
                    _state= COPYFROMDICT;
                    }
                    break;
                case COPYFROMDICT:
                    copyfromdict();
                    break;
            }
        }
        if (g_debug) fprintf(stderr, "decompress state= %d, copy: 0x%x, 0x%x\n", _state, _copyptr, _copycount);
        if (pdstused) *pdstused= _dst-dst;
        if (psrcused) *psrcused= _src-src;
    }
    void flush(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused)
    {
        if (g_debug) fprintf(stderr, "flash before state= %d, copy: 0x%x, 0x%x\n", _state, _copyptr, _copycount);
        _src= _srcend= NULL;
        _dst= dst;  _dstend= dst+dstlen;

        if (_state==COPYFROMDICT)
            copyfromdict();

        if (pdstused) *pdstused= _dst-dst;
        if (g_debug) fprintf(stderr, "flash after state= %d, copy: 0x%x, 0x%x\n", _state, _copyptr, _copycount);
    }
    void copyfromdict()
    {
        while (_dst<_dstend && _copycount)
        {
            addtodict(*_dst++ = _dict[_copyptr++]);
            _outputoffset++;
            _copycount--;
            _copyptr= _copyptr&(_dictsize-1);
        }
        if (_copycount==0)
            nextflagbit();
    }
    void dumpcopydata()
    {
        // note: we are printing incorrect data, if _copyptr == _dictptr-1
        for (int i=0 ; i<_copycount ; i++)
            fprintf(stderr, " %02x", _dict[(_copyptr+i)&(_dictsize-1)]);
        fprintf(stderr, "\n");
    }
    void addtodict(uint8_t c)
    {
        _dict[_dictptr++]= c;
        _dictptr = _dictptr&(_dictsize-1);
    }
    void nextflagbit()
    {
        _bitnr++;
        _flags>>=1;
        _state = _bitnr==8 ? EXPECTINGFLAG : PROCESSFLAGBIT;
    }
    void setcounter(uint8_t first, uint8_t second)
    {
        _copyptr= first | ((second&0xf0)<<4);
        _copycount= _copythreshold + (second&0xf);
    }
};

#if 1
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
    size_t nr = fread(buf, 1, size, input);
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
    if (fread(buf, 1, 2, input) != 2) {
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
    if (fread(str, 1, len, input) != len) {
        perror("read");
        exit(1);
    }
    if (g_debug) fprintf(stderr, "read str \"%s\"\n", str);
    return str;
}
#endif

void usage()
{
    fprintf(stderr, "Usage: kerneldec [-d]\n");
}

int main(int argc,char**argv)
{
//    _setmode(fileno(stdin),O_BINARY);
//    _setmode(fileno(stdout),O_BINARY);

    char *kppfile = NULL;
    const char *infile = "stdin";
    const char *outfile = "stdout";
    for (int i=1 ; i<argc ; i++)
    {
        if (argv[i][0]=='-') switch(argv[i][1])
        {
            case 'd': g_debug++;
                      if (argv[i][2]=='d')
                          g_debug++;
                      break;
            case 'i': 
                      i++;
                      infile = argv[i];
                      if (input != stdin)
                          fclose(input);
                      input = fopen(infile, "r");
                      break;
            case 'k': 
                      i++;
                      kppfile = argv[i];
                      break;
            case 'o': 
                      i++;
                      outfile = argv[i];
                      break;
            default:
                      usage();
                      return 1;
        }
        else {
            usage();
            return 1;
        }
    }
    if (!output || !input) {
        usage();
        return 1;
    }
#define CHUNK 0x10000

    lzssdecompress lzss;
    uint8_t *ibuf= (uint8_t*)malloc(CHUNK);
    uint8_t *obuf= (uint8_t*)malloc(CHUNK);
    size_t nr;
    uint8_t flag=0;

    // skip first <skipbytes> bytes
    char lzssmagic[] = "complzss";

    read_asn1hdr(ibuf);
    if (*ibuf != 0x30) {
        fprintf(stderr, "Invalid input - not IM4P\n");
        return 1;
    }

    uint64_t len = read_asn1len(ibuf[1]);

    if (g_debug) fprintf(stderr, "file length: %lld\n", len);

    char *str = read_asn1str();

    if (strcasecmp(str, "IM4P")) {
        fprintf(stderr, "Invalid input - not IM4P (0x%02x)\n", *ibuf);
        return 1;
    }

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

    struct lzss_hdr hdr;
    nr = fread(&hdr, 1, sizeof(struct lzss_hdr), input);
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
    if (strcasecmp(outfile, "stdout") != 0) {
        if (output != stdout)
            fclose(output);
        output = fopen(outfile, "w");
    }
    fprintf(stderr, "Writing kernelcache to %s...\n", outfile);
    while (!feof(input) && total_read < hdr.src_size)
    {
        if (total_read + CHUNK > hdr.src_size) {
            nr = fread(ibuf, 1, hdr.src_size - total_read, input);
        } else {
            nr = fread(ibuf, 1, CHUNK, input);
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
            size_t nw= fwrite(obuf, 1, dstused, output);
            if (nw<dstused) {
                perror("write");
                return 1;
            }
            total_written += nw;
            if (g_debug) fprintf(stderr, "decompress: 0x%x -> 0x%x\n", srcused, dstused);
        }
    }
    fprintf(stderr, "... done\n");
    if (kppfile != NULL) {
        fprintf(stderr, "Saving kpp to %s\n", kppfile);
        FILE *kpp = fopen(kppfile, "w");
        while ((nr = fread(ibuf, 1, CHUNK, input))) {
            if (fwrite(ibuf, 1, nr, kpp) != nr) {
                perror("write kpp:");
                return 1;
            }
        }
        fclose(kpp);
    }
    if (g_debug) fprintf(stderr, "done reading\n");
    uint32_t dstused;
    lzss.flush(obuf, CHUNK, &dstused);
    size_t nw= fwrite(obuf, 1, dstused, output);
    if (nw<dstused) {
        perror("write");
        return 1;
    }

    if (g_debug) fprintf(stderr, "flush: %d bytes\n", dstused);

    if (output != stdout)
        fclose(output);

    if (input != stdout)
        fclose(input);
    return 0;
}

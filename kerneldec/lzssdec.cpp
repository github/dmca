// (C)2009 Willem Hengeveld  itsme@xs4all.nl
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include "lzssdec.h"

// streaming version of the lzss algorithm, as defined in BootX-75/bootx.tproj/sl.subproj/lzss.c
// you can use lzssdec in a filter, like:
//
// cat file.lzss | lzssdec > file.decompressed
//
extern "C" int g_debug= 0;

lzssdecompress::lzssdecompress()
{
    _maxmatch= 18;  // 4 bit size + threshold 
    _dictsize= 4096; // 12 bit size
    _copythreshold= 3; // 0 == copy 3 bytes
    _dict= new uint8_t[_dictsize+_maxmatch-1];

    reset();
}
lzssdecompress::~lzssdecompress()
{
    delete[] _dict;
    _dict= 0; _dictsize= 0;
}
void lzssdecompress::reset()
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
void lzssdecompress::decompress(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused, uint8_t *src, uint32_t srclen, uint32_t *psrcused)
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
void lzssdecompress::flush(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused)
{
    if (g_debug) fprintf(stderr, "flash before state= %d, copy: 0x%x, 0x%x\n", _state, _copyptr, _copycount);
    _src= _srcend= NULL;
    _dst= dst;  _dstend= dst+dstlen;

    if (_state==COPYFROMDICT)
        copyfromdict();

    if (pdstused) *pdstused= _dst-dst;
    if (g_debug) fprintf(stderr, "flash after state= %d, copy: 0x%x, 0x%x\n", _state, _copyptr, _copycount);
}
void lzssdecompress::copyfromdict()
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
void lzssdecompress::dumpcopydata()
{
    // note: we are printing incorrect data, if _copyptr == _dictptr-1
    for (int i=0 ; i<_copycount ; i++)
        fprintf(stderr, " %02x", _dict[(_copyptr+i)&(_dictsize-1)]);
    fprintf(stderr, "\n");
}
void lzssdecompress::addtodict(uint8_t c)
{
    _dict[_dictptr++]= c;
    _dictptr = _dictptr&(_dictsize-1);
}
void lzssdecompress::nextflagbit()
{
    _bitnr++;
    _flags>>=1;
    _state = _bitnr==8 ? EXPECTINGFLAG : PROCESSFLAGBIT;
}
void lzssdecompress::setcounter(uint8_t first, uint8_t second)
{
    _copyptr= first | ((second&0xf0)<<4);
    _copycount= _copythreshold + (second&0xf);
}

#ifdef HAVE_MAIN
void usage()
{
    fprintf(stderr, "Usage: lzssdec [-d] [-o OFFSET]\n");
}
int main(int argc,char**argv)
{
#define HANDLEULOPTION(var, type) (argv[i][2] ? var= (type)strtoul(argv[i]+2, 0, 0) : i+1<argc ? var= (type)strtoul(argv[++i], 0, 0) : 0)

    uint32_t skipbytes=0;
    for (int i=1 ; i<argc ; i++)
    {
        if (argv[i][0]=='-') switch(argv[i][1])
        {
            case 'd': g_debug++;
                      if (argv[i][2]=='d')
                          g_debug++;
                      break;
            case 'o': HANDLEULOPTION(skipbytes, uint32_t); break;
            default:
                      usage();
                      return 1;
        }
        else {
            usage();
            return 1;
        }
    }
#define CHUNK 0x10000

    lzssdecompress lzss;
    uint8_t *ibuf= (uint8_t*)malloc(CHUNK);
    uint8_t *obuf= (uint8_t*)malloc(CHUNK);

    // skip first <skipbytes> bytes
    while (skipbytes && !feof(stdin)) {
        int nr= fread(ibuf, 1, std::min(skipbytes,(uint32_t)CHUNK), stdin);
        skipbytes -= nr;
    }

    while (!feof(stdin))
    {
        size_t nr= fread(ibuf, 1, CHUNK, stdin);
        if (nr==0) {
            perror("read");
            return 1;
        }
        if (nr==0)
            break;

        size_t srcp= 0;
        while (srcp<nr) {
            uint32_t dstused;
            uint32_t srcused;
            lzss.decompress(obuf, CHUNK, &dstused, ibuf+srcp, nr-srcp, &srcused);
            srcp+=srcused;
            size_t nw= fwrite(obuf, 1, dstused, stdout);
            if (nw<dstused) {
                perror("write");
                return 1;
            }
            if (g_debug) fprintf(stderr, "decompress: 0x%x -> 0x%x\n", srcused, dstused);
        }
    }
    if (g_debug) fprintf(stderr, "done reading\n");
    uint32_t dstused;
    lzss.flush(obuf, CHUNK, &dstused);
    size_t nw= fwrite(obuf, 1, dstused, stdout);
    if (nw<dstused) {
        perror("write");
        return 1;
    }

    if (g_debug) fprintf(stderr, "flush: %d bytes\n", dstused);

    return 0;
}
#endif

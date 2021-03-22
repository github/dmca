#ifndef _LZSSDEC_HPP
#define _LZSSDEC_HPP
#ifdef __cplusplus
extern "C" int g_debug;
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
    lzssdecompress();
    ~lzssdecompress();
    void reset();
    void decompress(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused, uint8_t *src, uint32_t srclen, uint32_t *psrcused);
    void flush(uint8_t *dst, uint32_t dstlen, uint32_t *pdstused);
    void copyfromdict();
    void dumpcopydata();
    void addtodict(uint8_t c);
    void nextflagbit();
    void setcounter(uint8_t first, uint8_t second);
};
#else
extern int g_debug;
#endif

#endif

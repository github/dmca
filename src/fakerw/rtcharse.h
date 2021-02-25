#pragma once

typedef rw::Charset RtCharset;
typedef rw::Charset::Desc RtCharsetDesc;

RwBool       RtCharsetOpen(void);
void         RtCharsetClose(void);
RtCharset   *RtCharsetPrint(RtCharset * charSet, const RwChar * string, RwInt32 x, RwInt32 y);
RtCharset   *RtCharsetPrintBuffered(RtCharset * charSet, const RwChar * string, RwInt32 x, RwInt32 y, RwBool hideSpaces);
RwBool       RtCharsetBufferFlush(void);
RtCharset   *RtCharsetSetColors(RtCharset * charSet, const RwRGBA * foreGround, const RwRGBA * backGround);
RtCharset   *RtCharsetGetDesc(RtCharset * charset, RtCharsetDesc * desc);
RtCharset   *RtCharsetCreate(const RwRGBA * foreGround, const RwRGBA * backGround);
RwBool       RtCharsetDestroy(RtCharset * charSet);

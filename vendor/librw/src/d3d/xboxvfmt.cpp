#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwanim.h"
#include "../rwengine.h"
#include "../rwplugins.h"
#include "rwxbox.h"

namespace rw {
namespace xbox {

static int32 vertexFmtOffset;

uint32 vertexFormatSizes[6] = {
	0, 1, 2, 2, 4, 4
};

uint32*
getVertexFmt(Geometry *g)
{
	return PLUGINOFFSET(uint32, g, vertexFmtOffset);
}

uint32
makeVertexFmt(int32 flags, uint32 numTexSets)
{
	if(numTexSets > 4)
		numTexSets = 4;
	uint32 fmt = 0x5;	// FLOAT3
	if(flags & Geometry::NORMALS)
		fmt |= 0x40;	// NORMPACKED3
	for(uint32 i = 0; i < numTexSets; i++)
		fmt |= 0x500 << i*4;	// FLOAT2
	if(flags & Geometry::PRELIT)
		fmt |= 0x1000000;	// D3DCOLOR
	return fmt;
}

uint32
getVertexFmtStride(uint32 fmt)
{
	uint32 stride = 0;
	uint32 v = fmt & 0xF;
	uint32 n = (fmt >> 4) & 0xF;
	stride += v == 4 ? 4 : 3*vertexFormatSizes[v];
	stride += n == 4 ? 4 : 3*vertexFormatSizes[n];
	if(fmt & 0x1000000)
		stride += 4;
	for(int i = 0; i < 4; i++){
		uint32 t = (fmt >> (i*4 + 8)) & 0xF;
		stride += t == 4 ? 4 : 2*vertexFormatSizes[t];
	}
	if(fmt & 0xE000000)
		stride += 8;
	return stride;
}

static void*
createVertexFmt(void *object, int32 offset, int32)
{
	*PLUGINOFFSET(uint32, object, offset) = 0;
	return object;
}

static void*
copyVertexFmt(void *dst, void *src, int32 offset, int32)
{
	*PLUGINOFFSET(uint32, dst, offset) = *PLUGINOFFSET(uint32, src, offset);
	return dst;
}

static Stream*
readVertexFmt(Stream *stream, int32, void *object, int32 offset, int32)
{
	uint32 fmt = stream->readU32();
	*PLUGINOFFSET(uint32, object, offset) = fmt;
	// TODO: ? create and attach "vertex shader"
	return stream;
}

static Stream*
writeVertexFmt(Stream *stream, int32, void *object, int32 offset, int32)
{
	stream->writeI32(*PLUGINOFFSET(uint32, object, offset));
	return stream;
}

static int32
getSizeVertexFmt(void*, int32, int32)
{
	if(rw::platform != PLATFORM_XBOX)
		return -1;
	return 4;
}

void
registerVertexFormatPlugin(void)
{
	vertexFmtOffset = Geometry::registerPlugin(sizeof(uint32), ID_VERTEXFMT,
	                         createVertexFmt, nil, copyVertexFmt);
	Geometry::registerPluginStream(ID_VERTEXFMT,
	                               readVertexFmt,
	                               writeVertexFmt,
	                               getSizeVertexFmt);
}

}
}

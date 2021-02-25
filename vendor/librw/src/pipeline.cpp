#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rwbase.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

#define COLOR_ARGB(a,r,g,b) \
    ((uint32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace rw {

static void nothing(ObjPipeline *, Atomic*) {}

void
ObjPipeline::init(uint32 platform)
{
	Pipeline::init(platform);
	this->impl.instance = nothing;
	this->impl.uninstance = nothing;
	this->impl.render = nothing;
}

ObjPipeline*
ObjPipeline::create(void)
{
	ObjPipeline *pipe = rwNewT(ObjPipeline, 1, MEMDUR_GLOBAL);
	pipe->init(PLATFORM_NULL);
	return pipe;
}

void
ObjPipeline::destroy(void)
{
	rwFree(this);
}

// helper functions

void
findMinVertAndNumVertices(uint16 *indices, uint32 numIndices, uint32 *minVert, int32 *numVertices)
{
	uint32 min = 0xFFFFFFFF;
	uint32 max = 0;
	while(numIndices--){
		if(*indices < min)
			min = *indices;
		if(*indices > max)
			max = *indices;
		indices++;
	}
	uint32 num = max - min + 1;
	// if mesh is empty, this can happen
	if(min > max){
		min = 0;
		num = 0;
	}
	if(minVert)
		*minVert = min;
	if(numVertices)
		*numVertices = num;
}

void
instV4d(int type, uint8 *dst, V4d *src, uint32 numVertices, uint32 stride)
{
	if(type == VERT_FLOAT4)
		for(uint32 i = 0; i < numVertices; i++){
			memcpy(dst, src, 16);
			dst += stride;
			src++;
		}
	else
		assert(0 && "unsupported instV4d type");
}

void
instV3d(int type, uint8 *dst, V3d *src, uint32 numVertices, uint32 stride)
{
	if(type == VERT_FLOAT3)
		for(uint32 i = 0; i < numVertices; i++){
			memcpy(dst, src, 12);
			dst += stride;
			src++;
		}
	else if(type == VERT_COMPNORM)
		for(uint32 i = 0; i < numVertices; i++){
			uint32 n = ((((uint32)(src->z *  511.0f)) & 0x3ff) << 22) |
				   ((((uint32)(src->y * 1023.0f)) & 0x7ff) << 11) |
				   ((((uint32)(src->x * 1023.0f)) & 0x7ff) <<  0);
			*(uint32*)dst = n;
			dst += stride;
			src++;
		}
	else
		assert(0 && "unsupported instV3d type");
}

void
uninstV3d(int type, V3d *dst, uint8 *src, uint32 numVertices, uint32 stride)
{
	if(type == VERT_FLOAT3)
		for(uint32 i = 0; i < numVertices; i++){
			memcpy(dst, src, 12);
			src += stride;
			dst++;
		}
	else if(type == VERT_COMPNORM)
		for(uint32 i = 0; i < numVertices; i++){
			uint32 n = *(uint32*)src;
			int32 normal[3];
			normal[0] = n & 0x7FF;
			normal[1] = (n >> 11) & 0x7FF;
			normal[2] = (n >> 22) & 0x3FF;
			// sign extend
			if(normal[0] & 0x400) normal[0] |= ~0x7FF;
			if(normal[1] & 0x400) normal[1] |= ~0x7FF;
			if(normal[2] & 0x200) normal[2] |= ~0x3FF;
			dst->x = normal[0] / 1023.0f;
			dst->y = normal[1] / 1023.0f;
			dst->z = normal[2] / 511.0f;
			src += stride;
			dst++;
		}
	else
		assert(0 && "unsupported uninstV3d type");
}

void
instTexCoords(int type, uint8 *dst, TexCoords *src, uint32 numVertices, uint32 stride)
{
	assert(type == VERT_FLOAT2);
	for(uint32 i = 0; i < numVertices; i++){
		memcpy(dst, src, 8);
		dst += stride;
		src++;
	}
}

void
uninstTexCoords(int type, TexCoords *dst, uint8 *src, uint32 numVertices, uint32 stride)
{
	assert(type == VERT_FLOAT2);
	for(uint32 i = 0; i < numVertices; i++){
		memcpy(dst, src, 8);
		src += stride;
		dst++;
	}
}

bool32
instColor(int type, uint8 *dst, RGBA *src, uint32 numVertices, uint32 stride)
{
	uint8 alpha = 0xFF;
	if(type == VERT_ARGB){
		for(uint32 i = 0; i < numVertices; i++){
			dst[0] = src->blue;
			dst[1] = src->green;
			dst[2] = src->red;
			dst[3] = src->alpha;
			alpha &= src->alpha;
			dst += stride;
			src++;
		}
	}else if(type == VERT_RGBA){
		for(uint32 i = 0; i < numVertices; i++){
			dst[0] = src->red;
			dst[1] = src->green;
			dst[2] = src->blue;
			dst[3] = src->alpha;
			alpha &= src->alpha;
			dst += stride;
			src++;
		}
	}else
		assert(0 && "unsupported color type");
	return alpha != 0xFF;
}

void
uninstColor(int type, RGBA *dst, uint8 *src, uint32 numVertices, uint32 stride)
{
	assert(type == VERT_ARGB);
	for(uint32 i = 0; i < numVertices; i++){
		dst->red = src[2];
		dst->green = src[1];
		dst->blue = src[0];
		dst->alpha = src[3];
		src += stride;
		dst++;
	}
}

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "rwxbox.h"

#include "rwxboximpl.h"

#define PLUGIN_ID 2

namespace rw {
namespace xbox {

static void*
driverOpen(void *o, int32, int32)
{
	engine->driver[PLATFORM_XBOX]->defaultPipeline = makeDefaultPipeline();

	engine->driver[PLATFORM_XBOX]->rasterNativeOffset = nativeRasterOffset;
	engine->driver[PLATFORM_XBOX]->rasterCreate = rasterCreate;
	engine->driver[PLATFORM_XBOX]->rasterLock = rasterLock;
	engine->driver[PLATFORM_XBOX]->rasterUnlock = rasterUnlock;
	engine->driver[PLATFORM_XBOX]->rasterNumLevels = rasterNumLevels;
	engine->driver[PLATFORM_XBOX]->rasterToImage = rasterToImage;

	return o;
}

static void*
driverClose(void *o, int32, int32)
{
	return o;
}

void
registerPlatformPlugins(void)
{
	Driver::registerPlugin(PLATFORM_XBOX, 0, PLATFORM_XBOX,
	                       driverOpen, driverClose);
	registerNativeRaster();
}

void*
destroyNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_XBOX)
		return object;
	InstanceDataHeader *header =
		(InstanceDataHeader*)geometry->instData;
	geometry->instData = nil;
	rwFree(header->vertexBuffer);
	rwFree(header->begin);
	rwFree(header->data);
	rwFree(header);
	return object;
}

Stream*
readNativeData(Stream *stream, int32, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	uint32 vers;
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, &vers)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_XBOX){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	if(vers < 0x35000){
		RWERROR((ERR_VERSION, vers));
		return nil;
	}
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	geometry->instData = header;
	header->platform = PLATFORM_XBOX;

	int32 size = stream->readI32();
	// The 0x18 byte are the resentryheader.
	// We don't have it but it's used for alignment.
	header->data = rwNewT(uint8, size + 0x18, MEMDUR_EVENT | ID_GEOMETRY);
	uint8 *p = header->data+0x18+4;
	stream->read8(p, size-4);

	header->size = size;
	header->serialNumber = *(uint16*)p; p += 2;
	header->numMeshes = *(uint16*)p; p += 2;
	header->primType = *(uint32*)p; p += 4;
	header->numVertices = *(uint32*)p; p += 4;
	header->stride = *(uint32*)p; p += 4;
	// RxXboxVertexFormat in 3.3 here
	p += 4;	// skip vertexBuffer pointer
	header->vertexAlpha = *(bool32*)p; p += 4;
	p += 8; // skip begin, end pointers

	InstanceData *inst  = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);
	header->begin = inst;
	for(int i = 0; i < header->numMeshes; i++){
		inst->minVert = *(uint32*)p; p += 4;
		inst->numVertices = *(int32*)p; p += 4;
		inst->numIndices = *(int32*)p; p += 4;
		inst->indexBuffer = header->data + *(uint32*)p; p += 4;
		p += 8; // skip material and vertexShader
		inst->vertexShader = 0;
		// pixelShader in 3.3 here
		inst++;
	}
	header->end = inst;

	header->vertexBuffer = rwNewT(uint8, header->stride*header->numVertices, MEMDUR_EVENT | ID_GEOMETRY);
	stream->read8(header->vertexBuffer, header->stride*header->numVertices);
	return stream;
}

Stream*
writeNativeData(Stream *stream, int32 len, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	writeChunkHeader(stream, ID_STRUCT, len-12);
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_XBOX)
		return stream;
	stream->writeU32(PLATFORM_XBOX);
	assert(rw::version >= 0x35000 && "can't write native Xbox data < 0x35000");
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;

	// we just fill header->data and write that
	uint8 *p = header->data+0x18;
	*(int32*)p = header->size; p += 4;
	*(uint16*)p = header->serialNumber; p += 2;
	*(uint16*)p = header->numMeshes; p += 2;
	*(uint32*)p = header->primType; p += 4;
	*(uint32*)p = header->numVertices; p += 4;
	*(uint32*)p = header->stride; p += 4;
	// RxXboxVertexFormat in 3.3 here
	p += 4;	// skip vertexBuffer pointer
	*(bool32*)p = header->vertexAlpha; p += 4;
	p += 8; // skip begin, end pointers

	InstanceData *inst = header->begin;
	for(int i = 0; i < header->numMeshes; i++){
		*(uint32*)p = inst->minVert; p += 4;
		*(int32*)p = inst->numVertices; p += 4;
		*(int32*)p = inst->numIndices; p += 4;
		*(uint32*)p = (uint8*)inst->indexBuffer - header->data; p += 4;
		p += 8; // skip material and vertexShader
		// pixelShader in 3.3 here
		inst++;
	}

	stream->write8(header->data+0x18, header->size);
	stream->write8(header->vertexBuffer, header->stride*header->numVertices);
	return stream;
}

int32
getSizeNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_XBOX)
		return 0;
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	return 12 + 4 + header->size + header->stride*header->numVertices;
}

void
registerNativeDataPlugin(void)
{
	Geometry::registerPlugin(0, ID_NATIVEDATA,
	                         nil, destroyNativeData, nil);
	Geometry::registerPluginStream(ID_NATIVEDATA,
	                               readNativeData,
	                               writeNativeData,
	                               getSizeNativeData);
}

enum {
	D3DPT_TRIANGLELIST    =  5,
	D3DPT_TRIANGLESTRIP   =  6
};

static void
instance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	// TODO: allow for REINSTANCE (or not, xbox can't render)
	if(geo->instData)
		return;
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	MeshHeader *meshh = geo->meshHeader;
	geo->instData = header;
	header->platform = PLATFORM_XBOX;

	header->size = 0x24 + meshh->numMeshes*0x18 + 0x10;
	Mesh *mesh = meshh->getMeshes();
	for(uint32 i = 0; i < meshh->numMeshes; i++)
		header->size += (mesh++->numIndices*2 + 0xF) & ~0xF;
	// The 0x18 byte are the resentryheader.
	// We don't have it but it's used for alignment.
	header->data = rwNewT(uint8, header->size + 0x18, MEMDUR_EVENT | ID_GEOMETRY);
	header->serialNumber = meshh->serialNum;
	header->numMeshes = meshh->numMeshes;
	header->primType = meshh->flags == MeshHeader::TRISTRIP ?
		D3DPT_TRIANGLESTRIP : D3DPT_TRIANGLELIST;
	header->numVertices = geo->numVertices;
	header->vertexAlpha = 0;
	// set by the instanceCB
	header->stride = 0;
	header->vertexBuffer = nil;

	InstanceData *inst = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);
	header->begin = inst;
	mesh = meshh->getMeshes();
	uint8 *indexbuf = (uint8*)header->data + ((0x18 + 0x24 + header->numMeshes*0x18 + 0xF)&~0xF);
	for(uint32 i = 0; i < header->numMeshes; i++){
		findMinVertAndNumVertices(mesh->indices, mesh->numIndices,
		                          &inst->minVert, &inst->numVertices);
		inst->numIndices = mesh->numIndices;
		inst->indexBuffer = indexbuf;
		memcpy(inst->indexBuffer, mesh->indices, inst->numIndices*sizeof(uint16));
		indexbuf += (inst->numIndices*2 + 0xF) & ~0xF;
		inst->material = mesh->material;
		inst->vertexShader = 0;	// TODO?
		mesh++;
		inst++;
	}
	header->end = inst;

	pipe->instanceCB(geo, header);
}

static void
uninstance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	if((geo->flags & Geometry::NATIVE) == 0)
		return;
	assert(geo->instData != nil);
	assert(geo->instData->platform == PLATFORM_XBOX);

	InstanceDataHeader *header = (InstanceDataHeader*)geo->instData;
	InstanceData *inst = header->begin;
	Mesh *mesh = geo->meshHeader->getMeshes();
	// For some reason numIndices in mesh and instance data are not always equal
	// And primitive isn't always correct either. Maybe some internal conversion...
	geo->meshHeader->totalIndices = 0;
	for(uint32 i = 0; i < header->numMeshes; i++){
		mesh[i].numIndices = inst[i].numIndices;
		geo->meshHeader->totalIndices += mesh[i].numIndices;
	}
	geo->meshHeader->flags = header->primType == D3DPT_TRIANGLESTRIP ?
		MeshHeader::TRISTRIP : 0;

	geo->numTriangles = geo->meshHeader->guessNumTriangles();
	geo->allocateData();
	geo->allocateMeshes(geo->meshHeader->numMeshes, geo->meshHeader->totalIndices, 0);

	mesh = geo->meshHeader->getMeshes();
	for(uint32 i = 0; i < header->numMeshes; i++){
		uint16 *indices = (uint16*)inst->indexBuffer;
		memcpy(mesh->indices, indices, inst->numIndices*2);
		mesh++;
		inst++;
	}

	pipe->uninstanceCB(geo, header);
	geo->generateTriangles();
	geo->flags &= ~Geometry::NATIVE;
	destroyNativeData(geo, 0, 0);
}

void
ObjPipeline::init(void)
{
	this->rw::ObjPipeline::init(PLATFORM_XBOX);
	this->impl.instance = xbox::instance;
	this->impl.uninstance = xbox::uninstance;
	this->instanceCB = nil;
	this->uninstanceCB = nil;
}

ObjPipeline*
ObjPipeline::create(void)
{
	ObjPipeline *pipe = rwNewT(ObjPipeline, 1, MEMDUR_GLOBAL);
	pipe->init();
	return pipe;
}

int v3dFormatMap[] = {
	-1, VERT_BYTE3, VERT_SHORT3, VERT_NORMSHORT3, VERT_COMPNORM, VERT_FLOAT3
};

int v2dFormatMap[] = {
	-1, VERT_BYTE2, VERT_SHORT2, VERT_NORMSHORT2, VERT_COMPNORM, VERT_FLOAT2
};

void
defaultInstanceCB(Geometry *geo, InstanceDataHeader *header)
{
	uint32 *vertexFmt = getVertexFmt(geo);
	if(*vertexFmt == 0)
		*vertexFmt = makeVertexFmt(geo->flags, geo->numTexCoordSets);
	header->stride = getVertexFmtStride(*vertexFmt);
	header->vertexBuffer = rwNewT(uint8, header->stride*header->numVertices, MEMDUR_EVENT | ID_GEOMETRY);
	uint8 *dst = (uint8*)header->vertexBuffer;

	uint32 fmt = *vertexFmt;
	uint32 sel = fmt & 0xF;
	instV3d(v3dFormatMap[sel], dst, geo->morphTargets[0].vertices,
	        header->numVertices, header->stride);
	dst += sel == 4 ? 4 : 3*vertexFormatSizes[sel];

	sel = (fmt >> 4) & 0xF;
	if(sel){
		instV3d(v3dFormatMap[sel], dst, geo->morphTargets[0].normals,
		        header->numVertices, header->stride);
		dst += sel == 4 ? 4 : 3*vertexFormatSizes[sel];
	}

	if(fmt & 0x1000000){
		header->vertexAlpha = instColor(VERT_ARGB, dst, geo->colors,
		                                header->numVertices, header->stride);
		dst += 4;
	}

	for(int i = 0; i < 4; i++){
		sel = (fmt >> (i*4 + 8)) & 0xF;
		if(sel == 0)
			break;
		instTexCoords(v2dFormatMap[sel], dst, geo->texCoords[i],
		        header->numVertices, header->stride);
		dst += sel == 4 ? 4 : 2*vertexFormatSizes[sel];
	}

	if(fmt & 0xE000000)
		assert(0 && "can't instance tangents or whatever it is");
}

void
defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header)
{
	uint32 *vertexFmt = getVertexFmt(geo);
	uint32 fmt = *vertexFmt;
	assert(fmt != 0);
	uint8 *src = (uint8*)header->vertexBuffer;

	uint32 sel = fmt & 0xF;
	uninstV3d(v3dFormatMap[sel], geo->morphTargets[0].vertices, src,
	          header->numVertices, header->stride);
	src += sel == 4 ? 4 : 3*vertexFormatSizes[sel];

	sel = (fmt >> 4) & 0xF;
	if(sel){
		uninstV3d(v3dFormatMap[sel], geo->morphTargets[0].normals, src,
		          header->numVertices, header->stride);
		src += sel == 4 ? 4 : 3*vertexFormatSizes[sel];
	}

	if(fmt & 0x1000000){
		uninstColor(VERT_ARGB, geo->colors, src,
		            header->numVertices, header->stride);
		src += 4;
	}

	for(int i = 0; i < 4; i++){
		sel = (fmt >> (i*4 + 8)) & 0xF;
		if(sel == 0)
			break;
		uninstTexCoords(v2dFormatMap[sel], geo->texCoords[i], src,
		          header->numVertices, header->stride);
		src += sel == 4 ? 4 : 2*vertexFormatSizes[sel];
	}
}

ObjPipeline*
makeDefaultPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	return pipe;
}

// Native Texture and Raster

int32 nativeRasterOffset;

static uint32
calculateTextureSize(uint32 width, uint32 height, uint32 depth, uint32 format)
{
#define D3DFMT_W11V11U10 65
	switch(format){
	default:
	case D3DFMT_UNKNOWN:
		return 0;
	case D3DFMT_A8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_AL8:
	case D3DFMT_LIN_A8:
	case D3DFMT_LIN_AL8:
	case D3DFMT_LIN_L8:
		return width * height * depth;
	case D3DFMT_R5G6B5:
	case D3DFMT_R6G5B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_R4G4B4A4:
	case D3DFMT_R5G5B5A1:
	case D3DFMT_R8B8:
	case D3DFMT_G8B8:
	case D3DFMT_A8L8:
	case D3DFMT_L16:
	//case D3DFMT_V8U8:
	//case D3DFMT_L6V5U5:
	case D3DFMT_D16_LOCKABLE:
	//case D3DFMT_D16:
	case D3DFMT_F16:
	case D3DFMT_YUY2:
	case D3DFMT_UYVY:
	case D3DFMT_LIN_A1R5G5B5:
	case D3DFMT_LIN_A4R4G4B4:
	case D3DFMT_LIN_G8B8:
	case D3DFMT_LIN_R4G4B4A4:
	case D3DFMT_LIN_R5G5B5A1:
	case D3DFMT_LIN_R5G6B5:
	case D3DFMT_LIN_R6G5B5:
	case D3DFMT_LIN_R8B8:
	case D3DFMT_LIN_X1R5G5B5:
	case D3DFMT_LIN_A8L8:
	case D3DFMT_LIN_L16:
	//case D3DFMT_LIN_V8U8:
	//case D3DFMT_LIN_L6V5U5:
	case D3DFMT_LIN_D16:
	case D3DFMT_LIN_F16:
		return width * 2 * height * depth;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_B8G8R8A8:
	case D3DFMT_R8G8B8A8:
	//case D3DFMT_X8L8V8U8:
	//case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_D24S8:
	case D3DFMT_F24S8:
	case D3DFMT_LIN_A8B8G8R8:
	case D3DFMT_LIN_A8R8G8B8:
	case D3DFMT_LIN_B8G8R8A8:
	case D3DFMT_LIN_R8G8B8A8:
	case D3DFMT_LIN_X8R8G8B8:
	case D3DFMT_LIN_V16U16:
	//case D3DFMT_LIN_X8L8V8U8:
	//case D3DFMT_LIN_Q8W8V8U8:
	case D3DFMT_LIN_D24S8:
	case D3DFMT_LIN_F24S8:
		return width * 4 * height * depth;
	case D3DFMT_DXT1:
		assert(depth <= 1);
		return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;
	//case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	//case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		assert(depth <= 1);
		return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
	}
}

static void*
createTexture(int32 width, int32 height, int32 numlevels, uint32 format)
{
	int32 w = width;
	int32 h = height;
	int32 size = 0;
	for(int32 i = 0; i < numlevels; i++){
		size += calculateTextureSize(w, h, 1, format);
		w /= 2;
		if(w == 0) w = 1;
		h /= 2;
		if(h == 0) h = 1;
	}
	size = (size+3)&~3;
	uint8 *data = (uint8*)rwNew(sizeof(RasterLevels)+sizeof(RasterLevels::Level)*(numlevels-1)+size,
		MEMDUR_EVENT | ID_DRIVER);
	RasterLevels *levels = (RasterLevels*)data;
	data += sizeof(RasterLevels)+sizeof(RasterLevels::Level)*(numlevels-1);
	levels->numlevels = numlevels;
	levels->format = format;
	w = width;
	h = height;
	for(int32 i = 0; i < numlevels; i++){
		levels->levels[i].width = w;
		levels->levels[i].height = h;
		levels->levels[i].data = data;
		levels->levels[i].size = calculateTextureSize(w, h, 1, format);
		data += levels->levels[i].size;
		w /= 2;
		if(w == 0) w = 1;
		h /= 2;
		if(h == 0) h = 1;
	}
	return levels;
}

struct RasterFormatInfo
{
	uint32 d3dformat;
	int32 depth;
	bool32 hasAlpha;
	uint32 rwFormat;
};

// indexed directly by RW format
static RasterFormatInfo formatInfoRW[16] = {
	{ 0, 0, 0, 0},
	{ D3DFMT_A1R5G5B5, 16, 1, Raster::C1555 },
	{ D3DFMT_R5G6B5,   16, 0, Raster::C565 },
	{ D3DFMT_A4R4G4B4, 16, 1, Raster::C4444 },
	{ D3DFMT_L8,        8, 0, Raster::LUM8 },
	{ D3DFMT_A8R8G8B8, 32, 1, Raster::C8888 },
	{ D3DFMT_X8R8G8B8, 32, 0, Raster::C888 },
	{ D3DFMT_UNKNOWN,  16, 0, Raster::D16 },
	{ D3DFMT_UNKNOWN,  32, 0, Raster::D24 },
	{ D3DFMT_UNKNOWN,  32, 0, Raster::D32 },
	{ D3DFMT_X1R5G5B5, 16, 0, Raster::C555 }
};

static void
rasterSetFormat(Raster *raster)
{
	assert(raster->format != 0);	// no default yet

	XboxRaster *natras = GETXBOXRASTEREXT(raster);
	if(raster->format & (Raster::PAL4 | Raster::PAL8)){
		natras->format = D3DFMT_P8;
		raster->depth = 8;
	}else{
		natras->format = formatInfoRW[(raster->format >> 8) & 0xF].d3dformat;
		raster->depth = formatInfoRW[(raster->format >> 8) & 0xF].depth;
	}
	natras->bpp = raster->depth/8;
	natras->hasAlpha =  formatInfoRW[(raster->format >> 8) & 0xF].hasAlpha;
	raster->stride = raster->width&natras->bpp;
}

static Raster*
rasterCreateTexture(Raster *raster)
{
	XboxRaster *natras = GETXBOXRASTEREXT(raster);
	int32 levels;

	if(natras->format == D3DFMT_P8)
		natras->palette = (uint8*)rwNew(4*256, MEMDUR_EVENT | ID_DRIVER);
	levels = Raster::calculateNumLevels(raster->width, raster->height);
	assert(natras->texture == nil);
	natras->texture = createTexture(raster->width, raster->height,
	                                raster->format & Raster::MIPMAP ? levels : 1,
	                                natras->format);
	if(natras->texture == nil){
		RWERROR((ERR_NOTEXTURE));
		return nil;
	}
	return raster;
}

Raster*
rasterCreate(Raster *raster)
{
	rasterSetFormat(raster);

	Raster *ret = raster;

	// Dummy to use as subraster
	if(raster->width == 0 || raster->height == 0){
		raster->flags |= Raster::DONTALLOCATE;
		raster->stride = 0;
		goto ret;
	}
	if(raster->flags & Raster::DONTALLOCATE)
		goto ret;

	switch(raster->type){
	case Raster::NORMAL:
	case Raster::TEXTURE:
		ret = rasterCreateTexture(raster);
		break;
	default:
		RWERROR((ERR_INVRASTER));
		return nil;
	}

ret:
	raster->originalWidth = raster->width;
	raster->originalHeight = raster->height;
	raster->originalStride = raster->stride;
	raster->originalPixels = raster->pixels;
	return ret;
}

uint8*
rasterLock(Raster *raster, int32 level, int32 lockMode)
{
	XboxRaster *natras = GETXBOXRASTEREXT(raster);

	// check if already locked
	if(raster->privateFlags & (Raster::PRIVATELOCK_READ|Raster::PRIVATELOCK_WRITE))
		return nil;

	RasterLevels *levels = (RasterLevels*)natras->texture;
	raster->pixels = levels->levels[level].data;
	raster->width = levels->levels[level].width;
	raster->height = levels->levels[level].height;
	raster->stride = raster->width*natras->bpp;

	if(lockMode & Raster::LOCKREAD) raster->privateFlags |= Raster::PRIVATELOCK_READ;
	if(lockMode & Raster::LOCKWRITE) raster->privateFlags |= Raster::PRIVATELOCK_WRITE;

	return raster->pixels;
}

void
rasterUnlock(Raster *raster, int32 level)
{
	raster->width = raster->originalWidth;
	raster->height = raster->originalHeight;
	raster->stride = raster->originalStride;
	raster->pixels = raster->originalPixels;

	raster->privateFlags &= ~(Raster::PRIVATELOCK_READ|Raster::PRIVATELOCK_WRITE);
}

int32
rasterNumLevels(Raster *raster)
{
	XboxRaster *natras = GETXBOXRASTEREXT(raster);
	RasterLevels *levels = (RasterLevels*)natras->texture;
	return levels->numlevels;
}

static void
unswizzle(uint8 *dst, uint8 *src, int32 w, int32 h, int32 bpp)
{
	uint32 maskU = 0;
	uint32 maskV = 0;
	int32 i = 1;
	int32 j = 1;
	int32 c;
	do{
		c = 0;
		if(i < w){
			maskU |= j;
			j <<= 1;
			c = j;
		}
		if(i < h){
			maskV |= j;
			j <<= 1;
			c = j;
		}
		i <<= 1;
	}while(c);
	int32 x, y, u, v;
	v = 0;
	for(y = 0; y < h; y++){
		u = 0;
		for(x = 0; x < w; x++){
			memcpy(&dst[(y*w + x)*bpp], &src[(u|v)*bpp], bpp);
			u = (u - maskU) & maskU;
		}
		v = (v - maskV) & maskV;
	}
}

Image*
rasterToImage(Raster *raster)
{
	int32 depth;
	Image *image;

	bool unlock = false;
	if(raster->pixels == nil){
		raster->lock(0, Raster::LOCKREAD);
		unlock = true;
	}

	XboxRaster *natras = GETXBOXRASTEREXT(raster);
	if(natras->customFormat){
		int w = raster->width;
		int h = raster->height;
		// pixels are in the upper right corner
		if(w < 4) w = 4;
		if(h < 4) h = 4;
		image = Image::create(w, h, 32);
		image->allocate();
		uint8 *pix = raster->pixels;
		switch(natras->format){
		case D3DFMT_DXT1:
			image->setPixelsDXT(1, pix);
			// TODO: is this correct?
			if(!natras->hasAlpha)
				image->removeMask();
			break;
		case D3DFMT_DXT3:
			image->setPixelsDXT(3, pix);
			break;
		case D3DFMT_DXT5:
			image->setPixelsDXT(5, pix);
			break;
		default:
			assert(0 && "unknown format");
			image->destroy();
			if(unlock)
				raster->unlock(0);
			return nil;
		}
		// fix it up again
		image->width = raster->width;
		image->height = raster->height;

		if(unlock)
			raster->unlock(0);
		return image;
	}

	switch(raster->format & 0xF00){
	case Raster::C1555:
		depth = 16;
		break;
	case Raster::C8888:
		depth = 32;
		break;
	case Raster::C888:
		depth = 24;
		break;
	case Raster::C555:
		depth = 16;
		break;

	default:
	case Raster::C565:
	case Raster::C4444:
	case Raster::LUM8:
		assert(0 && "unsupported raster format");
	}
	int32 pallength = 0;
	if((raster->format & Raster::PAL4) == Raster::PAL4){
		depth = 4;
		pallength = 16;
	}else if((raster->format & Raster::PAL8) == Raster::PAL8){
		depth = 8;
		pallength = 256;
	}

	image = Image::create(raster->width, raster->height, depth);
	image->allocate();

	if(pallength){
		uint8 *out = image->palette;
		uint8 *in = (uint8*)natras->palette;
		// bytes are BGRA unlike regular d3d!
		for(int32 i = 0; i < pallength; i++){
			conv_BGRA8888_from_RGBA8888(out, in);
			in += 4;
			out += 4;
		}
	}

	uint8 *imgpixels = image->pixels;
	uint8 *pixels = raster->pixels;

	// NB:
	assert(image->bpp == (int)natras->bpp);
	assert(image->stride == raster->stride);
	unswizzle(imgpixels, pixels, image->width, image->height, image->bpp);
	// Fix RGB order
	uint8 tmp;
	if(depth > 8)
		for(int32 y = 0; y < image->height; y++){
			uint8 *imgrow = imgpixels;
		//	uint8 *rasrow = pixels;
			for(int32 x = 0; x < image->width; x++){
				switch(raster->format & 0xF00){
				case Raster::C8888:
				case Raster::C888:
					tmp = imgrow[0];
					imgrow[0] = imgrow[2];
					imgrow[2] = tmp;
					imgrow += image->bpp;
					break;
				}
			}
			imgpixels += image->stride;
		//	pixels += raster->stride;
		}
	image->compressPalette();

	if(unlock)
		raster->unlock(0);

	return image;
}

int32
getLevelSize(Raster *raster, int32 level)
{
	XboxRaster *ras = GETXBOXRASTEREXT(raster);
	RasterLevels *levels = (RasterLevels*)ras->texture;
	return levels->levels[level].size;
}

static void*
createNativeRaster(void *object, int32 offset, int32)
{
	XboxRaster *raster = PLUGINOFFSET(XboxRaster, object, offset);
	raster->texture = nil;
	raster->palette = nil;
	raster->format = 0;
	raster->hasAlpha = 0;
	raster->customFormat = 0;
	raster->unknownFlag = 0;
	return object;
}

static void*
destroyNativeRaster(void *object, int32, int32)
{
	// TODO:
	return object;
}

static void*
copyNativeRaster(void *dst, void *, int32 offset, int32)
{
	XboxRaster *raster = PLUGINOFFSET(XboxRaster, dst, offset);
	raster->texture = nil;
	raster->palette = nil;
	raster->format = 0;
	raster->hasAlpha = 0;
	raster->unknownFlag = 0;
	return dst;
}

void
registerNativeRaster(void)
{
	nativeRasterOffset = Raster::registerPlugin(sizeof(XboxRaster),
	                                            ID_RASTERXBOX,
	                                            createNativeRaster,
	                                            destroyNativeRaster,
	                                            copyNativeRaster);
}

Texture*
readNativeTexture(Stream *stream)
{
	uint32 vers, platform;
	if(!findChunk(stream, ID_STRUCT, nil, &vers)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_XBOX){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	if(version < 0x34001){
		RWERROR((ERR_VERSION, version));
		return nil;
	}
	Texture *tex = Texture::create(nil);
	if(tex == nil)
		return nil;


	// Texture
	tex->filterAddressing = stream->readU32();
	stream->read8(tex->name, 32);
	stream->read8(tex->mask, 32);

//if(strcmp(tex->name, "bluallu") == 0)
//__debugbreak();

	// Raster
	int32 format = stream->readI32();
	bool32 hasAlpha = stream->readI16();
	bool32 unknownFlag = stream->readI16();
	int32 width = stream->readU16();
	int32 height = stream->readU16();
	int32 depth = stream->readU8();
	int32 numLevels = stream->readU8();
	int32 type = stream->readU8();
	int32 compression = stream->readU8();
	int32 totalSize = stream->readI32();

	// isn't this the cube map flag?
	assert(unknownFlag == 0);
	Raster *raster;
	if(compression){
		raster = Raster::create(width, height, depth, format | type | Raster::DONTALLOCATE, PLATFORM_XBOX);
		XboxRaster *ras = GETXBOXRASTEREXT(raster);
		ras->format = compression;
		ras->hasAlpha = hasAlpha;
		ras->texture = createTexture(raster->width, raster->height,
	                                     raster->format & Raster::MIPMAP ? numLevels : 1,
	                                     ras->format);
		ras->customFormat = 1;
		raster->flags &= ~Raster::DONTALLOCATE;
	}else
		raster = Raster::create(width, height, depth, format | type, PLATFORM_XBOX);
	XboxRaster *ras = GETXBOXRASTEREXT(raster);
	tex->raster = raster;

	if(raster->format & Raster::PAL4)
		stream->read8(ras->palette, 4*32);
	else if(raster->format & Raster::PAL8)
		stream->read8(ras->palette, 4*256);

	// exploit the fact that mipmaps are allocated consecutively
	uint8 *data = raster->lock(0, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
	stream->read8(data, totalSize);
	raster->unlock(0);

	return tex;
}

void
writeNativeTexture(Texture *tex, Stream *stream)
{
	int32 chunksize = getSizeNativeTexture(tex);
	writeChunkHeader(stream, ID_STRUCT, chunksize-12);
	stream->writeU32(PLATFORM_XBOX);

	// Texture
	stream->writeU32(tex->filterAddressing);
	stream->write8(tex->name, 32);
	stream->write8(tex->mask, 32);

	// Raster
	Raster *raster = tex->raster;
	XboxRaster *ras = GETXBOXRASTEREXT(raster);
	int32 numLevels = raster->getNumLevels();
	stream->writeI32(raster->format);
	stream->writeI16(ras->hasAlpha);
	stream->writeI16(ras->unknownFlag);
	stream->writeU16(raster->width);
	stream->writeU16(raster->height);
	stream->writeU8(raster->depth);
	stream->writeU8(numLevels);
	stream->writeU8(raster->type);
	stream->writeU8(ras->format);

	int32 totalSize = 0;
	for(int32 i = 0; i < numLevels; i++)
		totalSize += getLevelSize(tex->raster, i);
	totalSize = (totalSize+3)&~3;
	stream->writeI32(totalSize);

	if(raster->format & Raster::PAL4)
		stream->write8(ras->palette, 4*32);
	else if(raster->format & Raster::PAL8)
		stream->write8(ras->palette, 4*256);

	// exploit the fact that mipmaps are allocated consecutively
	uint8 *data = raster->lock(0, Raster::LOCKREAD);
	stream->write8(data, totalSize);
	raster->unlock(0);
}

uint32
getSizeNativeTexture(Texture *tex)
{
	uint32 size = 12 + 72 + 16 + 4;
	int32 levels = tex->raster->getNumLevels();
	for(int32 i = 0; i < levels; i++)
		size += getLevelSize(tex->raster, i);
	size = (size+3)&~3;
	if(tex->raster->format & Raster::PAL4)
		size += 4*32;
	else if(tex->raster->format & Raster::PAL8)
		size += 4*256;
	return size;
}

}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WITH_D3D
#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "rwd3d.h"
#include "rwd3d8.h"

#include "rwd3dimpl.h"

#define PLUGIN_ID 2

namespace rw {
namespace d3d8 {
using namespace d3d;

static void*
driverOpen(void *o, int32, int32)
{
	engine->driver[PLATFORM_D3D8]->defaultPipeline = makeDefaultPipeline();

	engine->driver[PLATFORM_D3D8]->rasterNativeOffset = nativeRasterOffset;
	engine->driver[PLATFORM_D3D8]->rasterCreate       = rasterCreate;
	engine->driver[PLATFORM_D3D8]->rasterLock         = rasterLock;
	engine->driver[PLATFORM_D3D8]->rasterUnlock       = rasterUnlock;
	engine->driver[PLATFORM_D3D8]->rasterNumLevels    = rasterNumLevels;
	engine->driver[PLATFORM_D3D8]->imageFindRasterFormat = imageFindRasterFormat;
	engine->driver[PLATFORM_D3D8]->rasterFromImage    = rasterFromImage;
	engine->driver[PLATFORM_D3D8]->rasterToImage      = rasterToImage;
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
	Driver::registerPlugin(PLATFORM_D3D8, 0, PLATFORM_D3D8,
	                       driverOpen, driverClose);
	// shared between D3D8 and 9
	if(nativeRasterOffset == 0)
		registerNativeRaster();
}

uint32
makeFVFDeclaration(uint32 flags, int32 numTex)
{
	uint32 fvf = 0x2;
	if(flags & Geometry::NORMALS)
		fvf |= 0x10;
	if(flags & Geometry::PRELIT)
		fvf |= 0x40;
	fvf |= numTex << 8;
	return fvf;
}

int32
getStride(uint32 flags, int32 numTex)
{
	int32 stride = 12;
	if(flags & Geometry::NORMALS)
		stride += 12;;
	if(flags & Geometry::PRELIT)
		stride += 4;
	stride += numTex*8;
	return stride;
}

void*
destroyNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_D3D8)
		return object;
	InstanceDataHeader *header =
		(InstanceDataHeader*)geometry->instData;
	geometry->instData = nil;
	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		destroyIndexBuffer(inst->indexBuffer);
		destroyVertexBuffer(inst->vertexBuffer);
		inst++;
	}
	rwFree(header->inst);
	rwFree(header);
	return object;
}

Stream*
readNativeData(Stream *stream, int32, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_D3D8){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	geometry->instData = header;
	header->platform = PLATFORM_D3D8;

	int32 size = stream->readI32();
	uint8 *data = rwNewT(uint8, size, MEMDUR_FUNCTION | ID_GEOMETRY);
	stream->read8(data, size);
	uint8 *p = data;
	header->serialNumber = *(uint16*)p; p += 2;
	header->numMeshes = *(uint16*)p; p += 2;
	header->inst = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		inst->minVert = *(uint32*)p; p += 4;
		inst->stride = *(uint32*)p; p += 4;
		inst->numVertices = *(uint32*)p; p += 4;
		inst->numIndices = *(uint32*)p; p += 4;
		uint32 matid = *(uint32*)p; p += 4;
		inst->material = geometry->matList.materials[matid];
		inst->vertexShader = *(uint32*)p; p += 4;
		inst->primType = *(uint32*)p; p += 4;
		inst->indexBuffer = nil; p += 4;
		inst->vertexBuffer = nil; p += 4;
		inst->baseIndex = 0; p += 4;
		inst->vertexAlpha = *p++;
		inst->managed = 0; p++;
		inst->remapped = 0; p++;	// TODO: really unused? and what's that anyway?
		inst++;
	}
	rwFree(data);

	inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		assert(inst->indexBuffer == nil);
		inst->indexBuffer = createIndexBuffer(inst->numIndices*2, false);
		uint16 *indices = lockIndices(inst->indexBuffer, 0, 0, 0);
		stream->read8(indices, 2*inst->numIndices);
		unlockIndices(inst->indexBuffer);

		inst->managed = 1;
		assert(inst->vertexBuffer == nil);
		inst->vertexBuffer = createVertexBuffer(inst->stride*inst->numVertices, 0, false);
		uint8 *verts = lockVertices(inst->vertexBuffer, 0, 0, D3DLOCK_NOSYSLOCK);
		stream->read8(verts, inst->stride*inst->numVertices);
		unlockVertices(inst->vertexBuffer);

		inst++;
	}
	return stream;
}

Stream*
writeNativeData(Stream *stream, int32 len, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	writeChunkHeader(stream, ID_STRUCT, len-12);
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_D3D8)
		return stream;
	stream->writeU32(PLATFORM_D3D8);
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;

	int32 size = 4 + geometry->meshHeader->numMeshes*0x2C;
	uint8 *data = rwNewT(uint8, size, MEMDUR_FUNCTION | ID_GEOMETRY);
	stream->writeI32(size);
	uint8 *p = data;
	*(uint16*)p = header->serialNumber; p += 2;
	*(uint16*)p = header->numMeshes; p += 2;

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		*(uint32*)p = inst->minVert; p += 4;
		*(uint32*)p = inst->stride; p += 4;
		*(uint32*)p = inst->numVertices; p += 4;
		*(uint32*)p = inst->numIndices; p += 4;
		int32 matid = geometry->matList.findIndex(inst->material);
		*(int32*)p = matid; p += 4;
		*(uint32*)p = inst->vertexShader; p += 4;
		*(uint32*)p = inst->primType; p += 4;
		*(uint32*)p = 0; p += 4;		// index buffer
		*(uint32*)p = 0; p += 4;		// vertex buffer
		*(uint32*)p = inst->baseIndex; p += 4;
		*p++ = inst->vertexAlpha;
		*p++ = inst->managed;
		*p++ = inst->remapped;
		inst++;
	}
	stream->write8(data, size);
	rwFree(data);

	inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		uint16 *indices = lockIndices(inst->indexBuffer, 0, 0, 0);
		stream->write8(indices, 2*inst->numIndices);
		unlockIndices(inst->indexBuffer);

		uint8 *verts = lockVertices(inst->vertexBuffer, 0, 0, D3DLOCK_NOSYSLOCK);
		stream->write8(verts, inst->stride*inst->numVertices);
		unlockVertices(inst->vertexBuffer);
		inst++;
	}
	return stream;
}

int32
getSizeNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_D3D8)
		return 0;

	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	InstanceData *inst = header->inst;
	int32 size = 12 + 4 + 4 + 4 + header->numMeshes*0x2C;
	for(int32 i = 0; i < header->numMeshes; i++){
		size += inst->numIndices*2 + inst->numVertices*inst->stride;
		inst++;
	}
	return size;
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


static void
instance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	// TODO: allow for REINSTANCE
	if(geo->instData)
		return;
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	MeshHeader *meshh = geo->meshHeader;
	geo->instData = header;
	header->platform = PLATFORM_D3D8;

	header->serialNumber = meshh->serialNum;
	header->numMeshes = meshh->numMeshes;
	header->inst = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);

	InstanceData *inst = header->inst;
	Mesh *mesh = meshh->getMeshes();
	for(uint32 i = 0; i < header->numMeshes; i++){
		findMinVertAndNumVertices(mesh->indices, mesh->numIndices,
		                          &inst->minVert, &inst->numVertices);
		inst->numIndices = mesh->numIndices;
		inst->material = mesh->material;
		inst->vertexShader = 0;
		inst->primType = meshh->flags == 1 ? D3DPT_TRIANGLESTRIP : D3DPT_TRIANGLELIST;
		inst->vertexBuffer = nil;
		inst->baseIndex = 0;	// (maybe) not used by us
		inst->vertexAlpha = 0;
		inst->managed = 0;
		inst->remapped = 0;

		inst->indexBuffer = createIndexBuffer(inst->numIndices*2, false);
		uint16 *indices = lockIndices(inst->indexBuffer, 0, 0, 0);
		if(inst->minVert == 0)
			memcpy(indices, mesh->indices, inst->numIndices*2);
		else
			for(int32 j = 0; j < inst->numIndices; j++)
				indices[j] = mesh->indices[j] - inst->minVert;
		unlockIndices(inst->indexBuffer);

		pipe->instanceCB(geo, inst);
		mesh++;
		inst++;
	}
}

static void
uninstance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	if((geo->flags & Geometry::NATIVE) == 0)
		return;
	assert(geo->instData != nil);
	assert(geo->instData->platform == PLATFORM_D3D8);
	geo->numTriangles = geo->meshHeader->guessNumTriangles();
	geo->allocateData();
	geo->allocateMeshes(geo->meshHeader->numMeshes, geo->meshHeader->totalIndices, 0);

	InstanceDataHeader *header = (InstanceDataHeader*)geo->instData;
	InstanceData *inst = header->inst;
	Mesh *mesh = geo->meshHeader->getMeshes();
	for(uint32 i = 0; i < header->numMeshes; i++){
		uint16 *indices = lockIndices(inst->indexBuffer, 0, 0, 0);
		if(inst->minVert == 0)
			memcpy(mesh->indices, indices, inst->numIndices*2);
		else
			for(int32 j = 0; j < inst->numIndices; j++)
				mesh->indices[j] = indices[j] + inst->minVert;
		unlockIndices(inst->indexBuffer);

		pipe->uninstanceCB(geo, inst);
		mesh++;
		inst++;
	}
	geo->generateTriangles();
	geo->flags &= ~Geometry::NATIVE;
	destroyNativeData(geo, 0, 0);
}

static void
render(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	// TODO: allow for REINSTANCE
	if(geo->instData == nil)
		pipe->instance(atomic);
	assert(geo->instData != nil);
	assert(geo->instData->platform == PLATFORM_D3D8);
	if(pipe->renderCB)
		pipe->renderCB(atomic, (InstanceDataHeader*)geo->instData);
}

void
ObjPipeline::init(void)
{
	this->rw::ObjPipeline::init(PLATFORM_D3D8);
	this->impl.instance = d3d8::instance;
	this->impl.uninstance = d3d8::uninstance;
	this->impl.render = d3d8::render;
	this->instanceCB = nil;
	this->uninstanceCB = nil;
	this->renderCB = nil;
}

ObjPipeline*
ObjPipeline::create(void)
{
	ObjPipeline *pipe = rwNewT(ObjPipeline, 1, MEMDUR_GLOBAL);
	pipe->init();
	return pipe;
}

void
defaultInstanceCB(Geometry *geo, InstanceData *inst)
{
	inst->vertexShader = makeFVFDeclaration(geo->flags, geo->numTexCoordSets);
	inst->stride = getStride(geo->flags, geo->numTexCoordSets);

	assert(inst->vertexBuffer == nil);
	inst->vertexBuffer = createVertexBuffer(inst->numVertices*inst->stride,
	                                              inst->vertexShader, false);
	inst->managed = 1;

	uint8 *dst = lockVertices(inst->vertexBuffer, 0, 0, D3DLOCK_NOSYSLOCK);
	instV3d(VERT_FLOAT3, dst,
		&geo->morphTargets[0].vertices[inst->minVert],
		inst->numVertices, inst->stride);
	dst += 12;

	if(geo->flags & Geometry::NORMALS){
		instV3d(VERT_FLOAT3, dst,
		        &geo->morphTargets[0].normals[inst->minVert],
		        inst->numVertices, inst->stride);
		dst += 12;
	}

	inst->vertexAlpha = 0;
	if(geo->flags & Geometry::PRELIT){
		inst->vertexAlpha = instColor(VERT_ARGB, dst, &geo->colors[inst->minVert],
		                              inst->numVertices, inst->stride);
		dst += 4;
	}

	for(int32 i = 0; i < geo->numTexCoordSets; i++){
		instTexCoords(VERT_FLOAT2, dst, &geo->texCoords[i][inst->minVert],
		        inst->numVertices, inst->stride);
		dst += 8;
	}
	unlockVertices(inst->vertexBuffer);
}

void
defaultUninstanceCB(Geometry *geo, InstanceData *inst)
{
	uint8 *src = lockVertices(inst->vertexBuffer, 0, 0, D3DLOCK_NOSYSLOCK);
	uninstV3d(VERT_FLOAT3,
		&geo->morphTargets[0].vertices[inst->minVert],
		src, inst->numVertices, inst->stride);
	src += 12;

	if(geo->flags & Geometry::NORMALS){
		uninstV3d(VERT_FLOAT3,
		          &geo->morphTargets[0].normals[inst->minVert],
		          src, inst->numVertices, inst->stride);
		src += 12;
	}

	inst->vertexAlpha = 0;
	if(geo->flags & Geometry::PRELIT){
		uninstColor(VERT_ARGB, &geo->colors[inst->minVert], src,
		            inst->numVertices, inst->stride);
		src += 4;
	}

	for(int32 i = 0; i < geo->numTexCoordSets; i++){
		uninstTexCoords(VERT_FLOAT2, &geo->texCoords[i][inst->minVert], src,
		          inst->numVertices, inst->stride);
		src += 8;
	}
	unlockVertices(inst->vertexBuffer);
}

ObjPipeline*
makeDefaultPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->renderCB = defaultRenderCB;
	return pipe;
}

// Native Texture and Raster

// only handles 4 and 8 bit textures right now
Raster*
readAsImage(Stream *stream, int32 width, int32 height, int32 depth, int32 format, int32 numLevels)
{
	uint8 palette[256*4];
	int32 pallen = 0;
	uint8 *data = nil;

	Image *img = Image::create(width, height, 32);
	img->allocate();

	if(format & Raster::PAL4){
		pallen = 16;
		stream->read8(palette, 4*32);
	}else if(format & Raster::PAL8){
		pallen = 256;
		stream->read8(palette, 4*256);
	}
	if(!Raster::formatHasAlpha(format))
		for(int32 i = 0; i < pallen; i++)
			palette[i*4+3] = 0xFF;

	Raster *ras = nil;

	for(int i = 0; i < numLevels; i++){
		uint32 size = stream->readU32();

		// don't read levels that don't exist
		if(ras && i >= ras->getNumLevels()){
			stream->seek(size);
			continue;
		}

		// one allocation is enough, first level is largest
		if(data == nil)
			data = rwNewT(uint8, size, MEMDUR_FUNCTION | ID_IMAGE);
		stream->read8(data, size);

		if(ras){
			ras->lock(i, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
			img->width = ras->width;
			img->height = ras->height;
			img->stride = img->width*img->bpp;
		}

		if(format & (Raster::PAL4 | Raster::PAL8)){
			uint8 *idx = data;
			uint8 *pixels = img->pixels;
			for(int y = 0; y < img->height; y++){
				uint8 *line = pixels;
				for(int x = 0; x < img->width; x++){
					line[0] = palette[*idx*4+0];
					line[1] = palette[*idx*4+1];
					line[2] = palette[*idx*4+2];
					line[3] = palette[*idx*4+3];
					line += img->bpp;
					idx++;
				}
				pixels += img->stride;
			}
		}

		if(ras == nil){
			// Important to have filled the image with data
			int32 newformat;
			Raster::imageFindRasterFormat(img, format&7, &width, &height, &depth, &newformat);
			newformat |= format & (Raster::MIPMAP | Raster::AUTOMIPMAP);
			ras = Raster::create(width, height, depth, newformat);
			ras->lock(i, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
		}

		ras->setFromImage(img);
		ras->unlock(i);
	}

	rwFree(data);
	img->destroy();
	return ras;
}

Texture*
readNativeTexture(Stream *stream)
{
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_D3D8){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	Texture *tex = Texture::create(nil);
	if(tex == nil)
		return nil;

	// Texture
	tex->filterAddressing = stream->readU32();
	stream->read8(tex->name, 32);
	stream->read8(tex->mask, 32);

	// Raster
	uint32 format = stream->readU32();
	bool32 hasAlpha = stream->readI32();
	int32 width = stream->readU16();
	int32 height = stream->readU16();
	int32 depth = stream->readU8();
	int32 numLevels = stream->readU8();
	int32 type = stream->readU8();
	int32 compression = stream->readU8();

	int32 pallength = 0;
	if(format & Raster::PAL4 || format & Raster::PAL8){
		pallength = format & Raster::PAL4 ? 32 : 256;
		if(!d3d::isP8supported){
			tex->raster = readAsImage(stream, width, height, depth, format|type, numLevels);
			return tex;
		}
	}

	Raster *raster;
	D3dRaster *ras;
	if(compression){
		raster = Raster::create(width, height, depth, format | type | Raster::DONTALLOCATE, PLATFORM_D3D8);
		ras = GETD3DRASTEREXT(raster);
		allocateDXT(raster, compression, numLevels, hasAlpha);
		ras->customFormat = 1;
	}else{
		raster = Raster::create(width, height, depth, format | type, PLATFORM_D3D8);
		ras = GETD3DRASTEREXT(raster);
	}
	tex->raster = raster;

	// TODO: check if format supported and convert if necessary

	if(pallength != 0)
		stream->read8(ras->palette, 4*pallength);

	uint32 size;
	uint8 *data;
	for(int32 i = 0; i < numLevels; i++){
		size = stream->readU32();
		if(i < raster->getNumLevels()){
			data = raster->lock(i, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
			stream->read8(data, size);
			raster->unlock(i);
		}else
			stream->seek(size);
	}
	return tex;
}

void
writeNativeTexture(Texture *tex, Stream *stream)
{
	int32 chunksize = getSizeNativeTexture(tex);
	writeChunkHeader(stream, ID_STRUCT, chunksize-12);
	stream->writeU32(PLATFORM_D3D8);

	// Texture
	stream->writeU32(tex->filterAddressing);
	stream->write8(tex->name, 32);
	stream->write8(tex->mask, 32);

	// Raster
	Raster *raster = tex->raster;
	D3dRaster *ras = GETD3DRASTEREXT(raster);
	int32 numLevels = raster->getNumLevels();
	stream->writeI32(raster->format);
	stream->writeI32(ras->hasAlpha);
	stream->writeU16(raster->width);
	stream->writeU16(raster->height);
	stream->writeU8(raster->depth);
	stream->writeU8(numLevels);
	stream->writeU8(raster->type);
	int32 compression = 0;
	if(ras->format)
		switch(ras->format){
		case 0x31545844:	// DXT1
			compression = 1;
			break;
		case 0x32545844:	// DXT2
			compression = 2;
			break;
		case 0x33545844:	// DXT3
			compression = 3;
			break;
		case 0x34545844:	// DXT4
			compression = 4;
			break;
		case 0x35545844:	// DXT5
			compression = 5;
			break;
		}
	stream->writeU8(compression);

	if(raster->format & Raster::PAL4)
		stream->write8(ras->palette, 4*32);
	else if(raster->format & Raster::PAL8)
		stream->write8(ras->palette, 4*256);

	uint32 size;
	uint8 *data;
	for(int32 i = 0; i < numLevels; i++){
		size = getLevelSize(raster, i);
		stream->writeU32(size);
		data = raster->lock(i, Raster::LOCKREAD);
		stream->write8(data, size);
		raster->unlock(i);
	}
}

uint32
getSizeNativeTexture(Texture *tex)
{
	uint32 size = 12 + 72 + 16;
	int32 levels = tex->raster->getNumLevels();
	if(tex->raster->format & Raster::PAL4)
		size += 4*32;
	else if(tex->raster->format & Raster::PAL8)
		size += 4*256;
	for(int32 i = 0; i < levels; i++)
		size += 4 + getLevelSize(tex->raster, i);
	return size;
}

}
}

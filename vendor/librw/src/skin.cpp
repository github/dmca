#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwanim.h"
#include "rwengine.h"
#include "rwplugins.h"
#include "ps2/rwps2.h"
#include "ps2/rwps2plg.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"
#include "gl/rwwdgl.h"
#include "gl/rwgl3.h"
#include "gl/rwgl3plg.h"

#define PLUGIN_ID ID_SKIN

namespace rw {

SkinGlobals skinGlobals = { 0, 0, { nil }, nil };

static void*
createSkin(void *object, int32 offset, int32)
{
	*PLUGINOFFSET(Skin*, object, offset) = nil;
	return object;
}

static void*
destroySkin(void *object, int32 offset, int32)
{
	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	if(skin){
		rwFree(skin->data);
		rwFree(skin->remapIndices);
//		delete[] skin->platformData;
	}
	rwFree(skin);
	return object;
}

static void*
copySkin(void *dst, void *src, int32 offset, int32)
{
	Skin *srcskin = *PLUGINOFFSET(Skin*, src, offset);
	if(srcskin == nil)
		return dst;
	Geometry *geometry = (Geometry*)src;
	assert(geometry->instData == nil);
	assert(((Geometry*)src)->numVertices == ((Geometry*)dst)->numVertices);
	Skin *dstskin = rwNewT(Skin, 1, MEMDUR_EVENT | ID_SKIN);
	*PLUGINOFFSET(Skin*, dst, offset) = dstskin;
	dstskin->numBones = srcskin->numBones;
	dstskin->numUsedBones = srcskin->numUsedBones;
	dstskin->numWeights = srcskin->numWeights;

	assert(0 && "can't copy skin yet");
	dstskin->init(srcskin->numBones, srcskin->numUsedBones,
	              geometry->numVertices);
	memcpy(dstskin->usedBones, srcskin->usedBones, srcskin->numUsedBones);
	memcpy(dstskin->inverseMatrices, srcskin->inverseMatrices,
	       srcskin->numBones*64);
	memcpy(dstskin->indices, srcskin->indices, geometry->numVertices*4);
	memcpy(dstskin->weights, srcskin->weights, geometry->numVertices*16);
	return dst;
}

Stream*
readSkinSplitData(Stream *stream, Skin *skin)
{
	uint32 sz;
	int8 *data;

	skin->boneLimit = stream->readI32();
	skin->numMeshes = stream->readI32();
	skin->rleSize = stream->readI32();
	if(skin->numMeshes){
		sz = skin->numBones + 2*(skin->numMeshes+skin->rleSize);
		data = (int8*)rwMalloc(sz, MEMDUR_EVENT | ID_SKIN);
		stream->read8(data, sz);
		skin->remapIndices = data;
		skin->rleCount = (Skin::RLEcount*)(data + skin->numBones);
		skin->rle = (Skin::RLE*)(data + skin->numBones + 2*skin->numMeshes);
	}
	return stream;
}

Stream*
writeSkinSplitData(Stream *stream, Skin *skin)
{
	stream->writeI32(skin->boneLimit);
	stream->writeI32(skin->numMeshes);
	stream->writeI32(skin->rleSize);
	if(skin->numMeshes)
		stream->write8(skin->remapIndices,
		              skin->numBones + 2*(skin->numMeshes+skin->rleSize));
	return stream;
}

int32
skinSplitDataSize(Skin *skin)
{
	if(skin->numMeshes == 0)
		return 12;
	return 12 + skin->numBones + 2*(skin->numMeshes+skin->rleSize);
}

static Stream*
readSkin(Stream *stream, int32 len, void *object, int32 offset, int32)
{
	uint8 header[4];
	Geometry *geometry = (Geometry*)object;

	if(geometry->instData){
		// TODO: function pointers
		if(geometry->instData->platform == PLATFORM_PS2)
			return ps2::readNativeSkin(stream, len, object, offset);
		else if(geometry->instData->platform == PLATFORM_WDGL)
			return wdgl::readNativeSkin(stream, len, object, offset);
		else if(geometry->instData->platform == PLATFORM_XBOX)
			return xbox::readNativeSkin(stream, len, object, offset);
		else{
			assert(0 && "unsupported native skin platform");
			return nil;
		}
	}

	stream->read8(header, 4);  // numBones, numUsedBones,
	                          // numWeights, unused
	Skin *skin = rwNewT(Skin, 1, MEMDUR_EVENT | ID_SKIN);
	*PLUGINOFFSET(Skin*, geometry, offset) = skin;

	// numUsedBones and numWeights appear in/after 34003
	// but not in/before 33002 (probably rw::version >= 0x34000)
	bool oldFormat = header[1] == 0;

	// Use numBones for numUsedBones to allocate data,
	// find out the correct value later
	if(oldFormat)
		skin->init(header[0], header[0], geometry->numVertices);
	else
		skin->init(header[0], header[1], geometry->numVertices);
	skin->numWeights = header[2];

	if(!oldFormat)
		stream->read8(skin->usedBones, skin->numUsedBones);
	if(skin->indices)
		stream->read8(skin->indices, geometry->numVertices*4);
	if(skin->weights)
		stream->read32(skin->weights, geometry->numVertices*16);
	for(int32 i = 0; i < skin->numBones; i++){
		if(oldFormat)
			stream->seek(4);	// skip 0xdeaddead
		stream->read32(&skin->inverseMatrices[i*16], 64);
	}

	if(oldFormat){
		skin->findNumWeights(geometry->numVertices);
		skin->findUsedBones(geometry->numVertices);
	}

	if(!oldFormat)
		readSkinSplitData(stream, skin);

	return stream;
}

static Stream*
writeSkin(Stream *stream, int32 len, void *object, int32 offset, int32)
{
	uint8 header[4];
	Geometry *geometry = (Geometry*)object;

	if(geometry->instData){
		if(geometry->instData->platform == PLATFORM_PS2)
			return ps2::writeNativeSkin(stream, len, object, offset);
		else if(geometry->instData->platform == PLATFORM_WDGL)
			return wdgl::writeNativeSkin(stream, len, object, offset);
		else if(geometry->instData->platform == PLATFORM_XBOX)
			return xbox::writeNativeSkin(stream, len, object, offset);
		else{
			assert(0 && "unsupported native skin platform");
			return nil;
		}
	}

	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	// not sure which version introduced the new format
	bool oldFormat = version < 0x34000;
	header[0] = skin->numBones;
	if(oldFormat){
		header[1] = 0;
		header[2] = 0;
	}else{
		header[1] = skin->numUsedBones;
		header[2] = skin->numWeights;
	}
	header[3] = 0;
	stream->write8(header, 4);
	if(!oldFormat)
		stream->write8(skin->usedBones, skin->numUsedBones);
	stream->write8(skin->indices, geometry->numVertices*4);
	stream->write32(skin->weights, geometry->numVertices*16);
	for(int32 i = 0; i < skin->numBones; i++){
		if(oldFormat)
			stream->writeU32(0xdeaddead);
		stream->write32(&skin->inverseMatrices[i*16], 64);
	}

	if(!oldFormat)
		writeSkinSplitData(stream, skin);
	return stream;
}

static int32
getSizeSkin(void *object, int32 offset, int32)
{
	Geometry *geometry = (Geometry*)object;

	if(geometry->instData){
		if(geometry->instData->platform == PLATFORM_PS2)
			return ps2::getSizeNativeSkin(object, offset);
		if(geometry->instData->platform == PLATFORM_WDGL)
			return wdgl::getSizeNativeSkin(object, offset);
		if(geometry->instData->platform == PLATFORM_XBOX)
			return xbox::getSizeNativeSkin(object, offset);
		if(geometry->instData->platform == PLATFORM_D3D8)
			return -1;
		if(geometry->instData->platform == PLATFORM_D3D9)
			return -1;
		assert(0 && "unsupported native skin platform");
	}

	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	if(skin == nil)
		return -1;

	int32 size = 4 + geometry->numVertices*(16+4) +
	             skin->numBones*64;
	// not sure which version introduced the new format
	if(version < 0x34000)
		size += skin->numBones*4;
	else
		size += skin->numUsedBones + skinSplitDataSize(skin);
	return size;
}

static Stream*
readSkinLegacy(Stream *stream, int32 len, void *object, int32, int32)
{
	Atomic *atomic = (Atomic*)object;
	Geometry *geometry = atomic->geometry;

	int32 numBones = stream->readI32();
	int32 numVertices = stream->readI32();
	assert(numVertices == geometry->numVertices);

	Skin *skin = rwNewT(Skin, 1, MEMDUR_EVENT | ID_SKIN);
	*PLUGINOFFSET(Skin*, geometry, skinGlobals.geoOffset) = skin;
	skin->init(numBones, numBones, numVertices);
	skin->legacyType = 1;
	skin->numWeights = 4;

	stream->read8(skin->indices, numVertices*4);
	stream->read32(skin->weights, numVertices*16);

	HAnimHierarchy *hier = HAnimHierarchy::create(numBones, nil, nil, 0, 36);

	for(int i = 0; i < numBones; i++){
		hier->nodeInfo[i].id = stream->readI32();
		hier->nodeInfo[i].index = stream->readI32();
		hier->nodeInfo[i].flags = stream->readI32() & 3;
//		printf("%d %d %d %d\n", i, hier->nodeInfo[i].id, hier->nodeInfo[i].index, hier->nodeInfo[i].flags);
		stream->read32(&skin->inverseMatrices[i*16], 64);

		Matrix mat;
		Matrix::invert(&mat, (Matrix*)&skin->inverseMatrices[i*16]);
//		printf("[ [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
//		       "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
//		       "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
//		       "  [ %8.4f, %8.4f, %8.4f, %8.4f ] ]\n"
//			"  %08x == flags\n",
//			mat.right.x, mat.up.x, mat.at.x, mat.pos.x,
//			mat.right.y, mat.up.y, mat.at.y, mat.pos.y,
//			mat.right.z, mat.up.z, mat.at.z, mat.pos.z,
//			0.0f, 0.0f, 0.0f, 1.0f,
//			mat.flags);
	}
	Frame *frame = atomic->getFrame()->child;
	assert(frame->next == nil);	// in old files atomic is above hierarchy it seems
	assert(frame->count() == numBones);	// assuming one frame per node this should also be true
	HAnimData::get(frame)->hierarchy = hier;
	hier->parentFrame = frame;

	return stream;
}

static void
skinRights(void *object, int32, int32, uint32)
{
	Skin::setPipeline((Atomic*)object, 1);
}

static void
skinAlways(void *object, int32, int32)
{
	Atomic *atomic = (Atomic*)object;
	Geometry *geo = atomic->geometry;
	if(geo == nil) return;
	Skin *skin = Skin::get(geo);
	if(skin == nil) return;
	Skin::setPipeline((Atomic*)object, 1);
}

static void*
createSkinAtm(void *object, int32 offset, int32)
{
	*PLUGINOFFSET(void*, object, offset) = nil;
	return object;
}

static void*
destroySkinAtm(void *object, int32 offset, int32)
{
	return object;
}

static void*
copySkinAtm(void *dst, void *src, int32 offset, int32)
{
	*PLUGINOFFSET(void*, dst, offset) = *PLUGINOFFSET(void*, src, offset);
	return dst;
}

static void*
skinOpen(void *o, int32, int32)
{
	// init dummy pipelines
	skinGlobals.dummypipe = ObjPipeline::create();
	skinGlobals.dummypipe->pluginID = ID_SKIN;
	skinGlobals.dummypipe->pluginData = 1;
	for(uint i = 0; i < nelem(skinGlobals.pipelines); i++)
		skinGlobals.pipelines[i] = skinGlobals.dummypipe;
	return o;
}

static void*
skinClose(void *o, int32, int32)
{
	for(uint i = 0; i < nelem(skinGlobals.pipelines); i++)
		if(skinGlobals.pipelines[i] == skinGlobals.dummypipe)
			matFXGlobals.pipelines[i] = nil;
	skinGlobals.dummypipe->destroy();
	skinGlobals.dummypipe = nil;
	return o;
}

void
registerSkinPlugin(void)
{
	Driver::registerPlugin(PLATFORM_NULL, 0, ID_SKIN,
	                       skinOpen, skinClose);
	ps2::initSkin();
	xbox::initSkin();
	d3d8::initSkin();
	d3d9::initSkin();
	wdgl::initSkin();
	gl3::initSkin();

	int32 o;
	o = Geometry::registerPlugin(sizeof(Skin*), ID_SKIN,
	                             createSkin, destroySkin, copySkin);
	Geometry::registerPluginStream(ID_SKIN,
	                               readSkin, writeSkin, getSizeSkin);
	skinGlobals.geoOffset = o;
	o = Atomic::registerPlugin(sizeof(HAnimHierarchy*),ID_SKIN,
	                           createSkinAtm, destroySkinAtm, copySkinAtm);
	skinGlobals.atomicOffset = o;
	Atomic::registerPluginStream(ID_SKIN, readSkinLegacy, nil, nil);
	Atomic::setStreamRightsCallback(ID_SKIN, skinRights);
	Atomic::setStreamAlwaysCallback(ID_SKIN, skinAlways);
}

void
Skin::init(int32 numBones, int32 numUsedBones, int32 numVertices)
{
	this->numBones = numBones;
	this->numUsedBones = numUsedBones;
	uint32 size = this->numUsedBones +
	              this->numBones*64 +
	              numVertices*(16+4) + 0xF;
	this->data = rwNewT(uint8, size, MEMDUR_EVENT | ID_SKIN);
	uint8 *p = this->data;

	this->usedBones = nil;
	if(this->numUsedBones){
		this->usedBones = p;
		p += this->numUsedBones;
	}

	p = (uint8*)(((uintptr)p + 0xF) & ~0xF);
	this->inverseMatrices = nil;
	if(this->numBones){
		this->inverseMatrices = (float*)p;
		p += 64*this->numBones;
	}

	this->indices = nil;
	if(numVertices){
		this->indices = p;
		p += 4*numVertices;
	}

	this->weights = nil;
	if(numVertices)
		this->weights = (float*)p;

	this->boneLimit = 0;
	this->numMeshes = 0;
	this->rleSize = 0;
	this->remapIndices = nil;
	this->rleCount = nil;
	this->rle = nil;

	this->platformData = nil;
	this->legacyType = 0;
}



//static_assert(sizeof(Skin::RLEcount) == 2, "RLEcount size");
//static_assert(sizeof(Skin::RLE) == 2, "RLE size");

void
Skin::findNumWeights(int32 numVertices)
{
	this->numWeights = 1;
	float *w = this->weights;
	while(numVertices--){
		while(w[this->numWeights] != 0.0f){
			this->numWeights++;
			if(this->numWeights == 4)
				return;
		}
		w += 4;
	}
}

void
Skin::findUsedBones(int32 numVertices)
{
	uint8 usedTab[256];
	uint8 *indices = this->indices;
	float *weights = this->weights;
	memset(usedTab, 0, 256);
	while(numVertices--){
		for(int32 i = 0; i < this->numWeights; i++){
			if(weights[i] == 0.0f)
				continue;	// TODO: this could probably be optimized
			if(usedTab[indices[i]] == 0)
				usedTab[indices[i]]++;
		}
		indices += 4;
		weights += 4;
	}
	this->numUsedBones = 0;
	for(int32 i = 0; i < 256; i++)
		if(usedTab[i])
			this->usedBones[this->numUsedBones++] = i;
}

void
Skin::setPipeline(Atomic *a, int32 type)
{
	(void)type;
	a->pipeline = skinGlobals.pipelines[rw::platform];
}

}

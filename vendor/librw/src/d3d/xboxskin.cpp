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

#define PLUGIN_ID ID_SKIN

namespace rw {
namespace xbox {

struct NativeSkin
{
	int32 table1[256];	// maps indices to bones
	int32 table2[256];	// maps bones to indices
	int32 numUsedBones;
	void *vertexBuffer;
	int32 stride;
};

Stream*
readNativeSkin(Stream *stream, int32, void *object, int32 offset)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
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
	if(vers < 0x35000){
		RWERROR((ERR_VERSION, vers));
		return nil;
	}

	Skin *skin = rwNewT(Skin, 1, MEMDUR_EVENT | ID_SKIN);
	*PLUGINOFFSET(Skin*, geometry, offset) = skin;

	int32 numBones = stream->readI32();
	skin->init(numBones, 0, 0);
	NativeSkin *natskin = rwNewT(NativeSkin, 1, MEMDUR_EVENT | ID_SKIN);
	skin->platformData = natskin;
	stream->read32(natskin->table1, 256*sizeof(int32));
	stream->read32(natskin->table2, 256*sizeof(int32));
	natskin->numUsedBones = stream->readI32();
	skin->numWeights = stream->readI32();
	stream->seek(4);	// skip pointer to vertexBuffer
	natskin->stride = stream->readI32();
	int32 size = geometry->numVertices*natskin->stride;
	natskin->vertexBuffer = rwNewT(uint8, size, MEMDUR_EVENT | ID_SKIN);
	stream->read8(natskin->vertexBuffer, size);
	stream->read32(skin->inverseMatrices, skin->numBones*64);

	readSkinSplitData(stream, skin);
	return stream;
}

Stream*
writeNativeSkin(Stream *stream, int32 len, void *object, int32 offset)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	assert(skin->platformData);
	assert(rw::version >= 0x35000 && "can't handle native xbox skin < 0x35000");
	NativeSkin *natskin = (NativeSkin*)skin->platformData;

	writeChunkHeader(stream, ID_STRUCT, len-12);
	stream->writeU32(PLATFORM_XBOX);
	stream->writeI32(skin->numBones);
	stream->write32(natskin->table1, 256*sizeof(int32));
	stream->write32(natskin->table2, 256*sizeof(int32));
	stream->writeI32(natskin->numUsedBones);
	stream->writeI32(skin->numWeights);
	stream->writeU32(0xBADEAFFE);	// pointer to vertexBuffer
	stream->writeI32(natskin->stride);
	stream->write8(natskin->vertexBuffer,
	              geometry->numVertices*natskin->stride);
	stream->write32(skin->inverseMatrices, skin->numBones*64);

	writeSkinSplitData(stream, skin);
	return stream;
}

int32
getSizeNativeSkin(void *object, int32 offset)
{
	Geometry *geometry = (Geometry*)object;
	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	if(skin == nil)
		return -1;
	if(skin->platformData == nil)
		return -1;
	NativeSkin *natskin = (NativeSkin*)skin->platformData;
	return 12 + 8 + 2*256*4 + 4*4 +
	        natskin->stride*geometry->numVertices + skin->numBones*64 +
		skinSplitDataSize(skin);
}

void
skinInstanceCB(Geometry *geo, InstanceDataHeader *header)
{
	defaultInstanceCB(geo, header);

	Skin *skin = Skin::get(geo);
	if(skin == nil)
		return;
	NativeSkin *natskin = rwNewT(NativeSkin, 1, MEMDUR_EVENT | ID_SKIN);
	skin->platformData = natskin;

	natskin->numUsedBones = skin->numUsedBones;
	memset(natskin->table1, 0xFF, sizeof(natskin->table1));
	memset(natskin->table2, 0x00, sizeof(natskin->table2));
	for(int32 i = 0; i < skin->numUsedBones; i++){
		natskin->table1[i] = skin->usedBones[i];
		natskin->table2[skin->usedBones[i]] = i;
	}

	natskin->stride = 3*skin->numWeights;
	uint8 *vbuf = rwNewT(uint8, header->numVertices*natskin->stride, MEMDUR_EVENT | ID_SKIN);
	natskin->vertexBuffer = vbuf;

	int32 w[4];
	int sum;
	float32 *weights = skin->weights;
	uint8 *p = vbuf;
	int32 numVertices = header->numVertices;
	while(numVertices--){
		sum = 0;
		for(int i = 1; i < skin->numWeights; i++){
			w[i] = (int32)(weights[i]*255.0f + 0.5f);
			sum += w[i];
		}
		w[0] = 255 - sum;
		for(int i = 0; i < skin->numWeights; i++)
			p[i] = w[i];
		p += natskin->stride;
		weights += 4;
	}

	numVertices = header->numVertices;
	p = vbuf + skin->numWeights;
	uint8 *indices = skin->indices;
	uint16 *idx;
	while(numVertices--){
		idx = (uint16*)p;
		for(int i = 0; i < skin->numWeights; i++)
			idx[i] = 3*natskin->table2[indices[i]];
		p += natskin->stride;
		indices += 4;
	}
}

void
skinUninstanceCB(Geometry *geo, InstanceDataHeader *header)
{
	defaultUninstanceCB(geo, header);

	Skin *skin = Skin::get(geo);
	if(skin == nil)
		return;
	NativeSkin *natskin = (NativeSkin*)skin->platformData;

	uint8 *data = skin->data;
	float *invMats = skin->inverseMatrices;
	skin->init(skin->numBones, natskin->numUsedBones, geo->numVertices);
	memcpy(skin->inverseMatrices, invMats, skin->numBones*64);
	rwFree(data);

	for(int32 j = 0; j < skin->numUsedBones; j++)
		skin->usedBones[j] = natskin->table1[j];

	float *weights = skin->weights;
	uint8 *indices = skin->indices;
	uint8 *p = (uint8*)natskin->vertexBuffer;
	int32 numVertices = header->numVertices;
	float w[4];
	uint8 i[4];
	uint16 *ip;
	while(numVertices--){
		w[0] = w[1] = w[2] = w[3] = 0.0f;
		i[0] = i[1] = i[2] = i[3] = 0;

		for(int32 j = 0; j < skin->numWeights; j++)
			w[j] = *p++/255.0f;

		ip = (uint16*)p;
		for(int32 j = 0; j < skin->numWeights; j++){
			i[j] = natskin->table1[*ip++/3];
			if(w[j] == 0.0f) i[j] = 0;	// clean up a bit
		}
		p = (uint8*)ip;

		for(int32 j = 0; j < 4; j++){
			*weights++ = w[j];
			*indices++ = i[j];
		}
	}

	rwFree(natskin->vertexBuffer);
	rwFree(natskin);
}

static void*
skinOpen(void *o, int32, int32)
{
	skinGlobals.pipelines[PLATFORM_XBOX] = makeSkinPipeline();
	return o;
}

static void*
skinClose(void *o, int32, int32)
{
	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_XBOX])->destroy();
	skinGlobals.pipelines[PLATFORM_XBOX] = nil;
	return o;
}

void
initSkin(void)
{
	Driver::registerPlugin(PLATFORM_XBOX, 0, ID_SKIN,
	                       skinOpen, skinClose);
}

ObjPipeline*
makeSkinPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = skinInstanceCB;
	pipe->uninstanceCB = skinUninstanceCB;
	pipe->pluginID = ID_SKIN;
	pipe->pluginData = 1;
	return pipe;
}

}
}

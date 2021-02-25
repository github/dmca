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
#include "rwps2.h"
#include "rwps2plg.h"

#include "rwps2impl.h"

#define PLUGIN_ID ID_SKIN

namespace rw {
namespace ps2 {

static void*
skinOpen(void *o, int32, int32)
{
	skinGlobals.pipelines[PLATFORM_PS2] = makeSkinPipeline();
	return o;
}

static void*
skinClose(void *o, int32, int32)
{
	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_PS2])->groupPipeline->destroy();
	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_PS2])->groupPipeline = nil;
	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_PS2])->destroy();
	skinGlobals.pipelines[PLATFORM_PS2] = nil;
	return o;
}

void
initSkin(void)
{
	Driver::registerPlugin(PLATFORM_PS2, 0, ID_SKIN,
	                       skinOpen, skinClose);
}

ObjPipeline*
makeSkinPipeline(void)
{
	MatPipeline *pipe = MatPipeline::create();
	pipe->pluginID = ID_SKIN;
	pipe->pluginData = 1;
	pipe->attribs[AT_XYZ] = &attribXYZ;
	pipe->attribs[AT_UV] = &attribUV;
	pipe->attribs[AT_RGBA] = &attribRGBA;
	pipe->attribs[AT_NORMAL] = &attribNormal;
	pipe->attribs[AT_NORMAL+1] = &attribWeights;
	uint32 vertCount = MatPipeline::getVertCount(VU_Lights-0x100, 5, 3, 2);
	pipe->setTriBufferSizes(5, vertCount);
	pipe->vifOffset = pipe->inputStride*vertCount;
	pipe->instanceCB = skinInstanceCB;
	pipe->uninstanceCB = genericUninstanceCB;
	pipe->preUninstCB = skinPreCB;
	pipe->postUninstCB = skinPostCB;

	ObjPipeline *opipe = ObjPipeline::create();
	opipe->pluginID = ID_SKIN;
	opipe->pluginData = 1;
	opipe->groupPipeline = pipe;
	return opipe;
}

Stream*
readNativeSkin(Stream *stream, int32, void *object, int32 offset)
{
	uint8 header[4];
	Geometry *geometry = (Geometry*)object;
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_PS2){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	stream->read8(header, 4);
	Skin *skin = rwNewT(Skin, 1, MEMDUR_EVENT | ID_SKIN);
	*PLUGINOFFSET(Skin*, geometry, offset) = skin;

	// numUsedBones and numWeights appear in/after 34003
	// but not in/before 33002 (probably rw::version >= 0x34000)
	bool oldFormat = header[1] == 0;

	// Use numBones for numUsedBones to allocate data
	if(oldFormat)
		skin->init(header[0], header[0], 0);
	else
		skin->init(header[0], header[1], 0);
	skin->numWeights = header[2];

	if(!oldFormat)
		stream->read8(skin->usedBones, skin->numUsedBones);
	if(skin->numBones)
		stream->read32(skin->inverseMatrices, skin->numBones*64);

	// dummy data in case we need to write data in the new format
	if(oldFormat){
		skin->numWeights = 4;
		for(int32 i = 0; i < skin->numUsedBones; i++)
			skin->usedBones[i] = i;
	}

	if(!oldFormat){
		// TODO: what is this?
		stream->seek(4*4);

		readSkinSplitData(stream, skin);
	}
	return stream;
}

Stream*
writeNativeSkin(Stream *stream, int32 len, void *object, int32 offset)
{
	uint8 header[4];

	writeChunkHeader(stream, ID_STRUCT, len-12);
	stream->writeU32(PLATFORM_PS2);
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
	stream->write32(skin->inverseMatrices, skin->numBones*64);
	if(!oldFormat){
		uint32 buffer[4] = { 0, 0, 0, 0, };
		stream->write32(buffer, 4*4);

		writeSkinSplitData(stream, skin);
	}
	return stream;
}

int32
getSizeNativeSkin(void *object, int32 offset)
{
	Skin *skin = *PLUGINOFFSET(Skin*, object, offset);
	if(skin == nil)
		return -1;
	int32 size = 12 + 4 + 4 + skin->numBones*64;
	// not sure which version introduced the new format
	if(version >= 0x34000)
		size += skin->numUsedBones + 16 + skinSplitDataSize(skin);
	return size;
}

void
instanceSkinData(Geometry*, Mesh *m, Skin *skin, uint32 *data)
{
	uint16 j;
	float32 *weights = (float32*)data;
	uint32 *indices = data;
	for(uint32 i = 0; i < m->numIndices; i++){
		j = m->indices[i];
		for(int32 k = 0; k < 4; k++){
			*weights++ = skin->weights[j*4+k];
			*indices &= ~0x3FF;
			*indices++ |= skin->indices[j*4+k] && skin->weights[j*4+k] ?
					(skin->indices[j*4+k]+1) << 2 : 0;
		}
	}
}

void
skinInstanceCB(MatPipeline *, Geometry *g, Mesh *m, uint8 **data)
{
	Skin *skin = Skin::get(g);
	if(skin == nil)
		return;
	instanceSkinData(g, m, skin, (uint32*)data[4]);
}

// TODO: call base function perhaps?
int32
findVertexSkin(Geometry *g, uint32 flags[], uint32 mask, Vertex *v)
{
	Skin *skin = Skin::get(g);
	float32 *wghts = nil;
	uint8 *inds = nil;
	if(skin){
		wghts = skin->weights;
		inds = skin->indices;
	}

	V3d *verts = g->morphTargets[0].vertices;
	TexCoords *tex = g->texCoords[0];
	TexCoords *tex1 = g->texCoords[1];
	V3d *norms = g->morphTargets[0].normals;
	RGBA *cols = g->colors;

	for(int32 i = 0; i < g->numVertices; i++){
		uint32 flag = flags ? flags[i] : ~0;
		if(mask & flag & 0x1 && !equal(*verts, v->p))
			goto cont;
		if(mask & flag & 0x10 && !equal(*norms, v->n))
			goto cont;
		if(mask & flag & 0x100 && !equal(*cols, v->c))
			goto cont;
		if(mask & flag & 0x1000 && !equal(*tex, v->t))
			goto cont;
		if(mask & flag & 0x2000 && !equal(*tex1, v->t1))
			goto cont;
		if(mask & flag & 0x10000 &&
		   !(wghts[0] == v->w[0] && wghts[1] == v->w[1] &&
		     wghts[2] == v->w[2] && wghts[3] == v->w[3] &&
		     inds[0] == v->i[0] && inds[1] == v->i[1] &&
		     inds[2] == v->i[2] && inds[3] == v->i[3]))
			goto cont;
		return i;
	cont:
		verts++;
		tex++;
		tex1++;
		norms++;
		cols++;
		wghts += 4;
		inds += 4;
	}
	return -1;
}

void
insertVertexSkin(Geometry *geo, int32 i, uint32 mask, Vertex *v)
{
	Skin *skin = Skin::get(geo);
	insertVertex(geo, i, mask, v);
	if(mask & 0x10000){
		memcpy(&skin->weights[i*4], v->w, 16);
		memcpy(&skin->indices[i*4], v->i, 4);
	}
}

/*
void
skinUninstanceCB(MatPipeline*, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[])
{
	float32 *verts     = (float32*)data[AT_XYZ];
	float32 *texcoords = (float32*)data[AT_UV];
	uint8 *colors      = (uint8*)data[AT_RGBA];
	int8 *norms        = (int8*)data[AT_NORMAL];
	uint32 *wghts      = (uint32*)data[AT_NORMAL+1];
	uint32 mask = 0x1;	// vertices
	if(geo->flags & Geometry::NORMALS)
		mask |= 0x10;
	if(geo->flags & Geometry::PRELIT)
		mask |= 0x100;
	if(geo->numTexCoordSets > 0)
		mask |= 0x1000;
	mask |= 0x10000;

	Vertex v;
	for(uint32 i = 0; i < mesh->numIndices; i++){
		if(mask & 0x1)
			memcpy(&v.p, verts, 12);
		if(mask & 0x10){
			v.n[0] = norms[0]/127.0f;
			v.n[1] = norms[1]/127.0f;
			v.n[2] = norms[2]/127.0f;
		}
		if(mask & 0x100)
			memcpy(&v.c, colors, 4);
		if(mask & 0x1000)
			memcpy(&v.t, texcoords, 8);
		for(int j = 0; j < 4; j++){
			((uint32*)v.w)[j] = wghts[j] & ~0x3FF;
			v.i[j] = (wghts[j] & 0x3FF) >> 2;
			if(v.i[j]) v.i[j]--;
			if(v.w[j] == 0.0f) v.i[j] = 0;
		}
		int32 idx = findVertexSkin(geo, flags, mask, &v);
		if(idx < 0)
			idx = geo->numVertices++;
		mesh->indices[i] = idx;
		flags[idx] = mask;
		insertVertexSkin(geo, idx, mask, &v);
		verts += 3;
		texcoords += 2;
		colors += 4;
		norms += 3;
		wghts += 4;
	}
}
*/

void
skinPreCB(MatPipeline*, Geometry *geo)
{
	Skin *skin = Skin::get(geo);
	if(skin == nil)
		return;
	uint8 *data = skin->data;
	float *invMats = skin->inverseMatrices;
	// meshHeader->totalIndices is highest possible number of vertices again
	skin->init(skin->numBones, skin->numBones, geo->meshHeader->totalIndices);
	memcpy(skin->inverseMatrices, invMats, skin->numBones*64);
	rwFree(data);
}

void
skinPostCB(MatPipeline*, Geometry *geo)
{
	Skin *skin = Skin::get(geo);
	if(skin){
		skin->findNumWeights(geo->numVertices);
		skin->findUsedBones(geo->numVertices);
	}
}

}
}

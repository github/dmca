#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WITH_D3D
#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwrender.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwanim.h"
#include "../rwengine.h"
#include "../rwplugins.h"
#include "rwd3d.h"
#include "rwd3d9.h"

namespace rw {
namespace d3d9 {
using namespace d3d;

#ifndef RW_D3D9
void skinInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance) {}
void skinRenderCB(Atomic *atomic, InstanceDataHeader *header) {}
#else


static void *skin_amb_VS;
static void *skin_amb_dir_VS;
static void *skin_all_VS;

#define NUMDECLELT 14

void
skinInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance)
{
	int i = 0;
	VertexElement dcl[NUMDECLELT];
	VertexStream *s = &header->vertexStream[0];

	bool isPrelit = (geo->flags & Geometry::PRELIT) != 0;
	bool hasNormals = (geo->flags & Geometry::NORMALS) != 0;

	// TODO: support both vertex buffers

	if(!reinstance){
		// Create declarations and buffers only the first time

		assert(s->vertexBuffer == nil);
		s->offset = 0;
		s->managed = 1;
		s->geometryFlags = 0;
		s->dynamicLock = 0;

		dcl[i].stream = 0;
		dcl[i].offset = 0;
		dcl[i].type = D3DDECLTYPE_FLOAT3;
		dcl[i].method = D3DDECLMETHOD_DEFAULT;
		dcl[i].usage = D3DDECLUSAGE_POSITION;
		dcl[i].usageIndex = 0;
		i++;
		uint16 stride = 12;
		s->geometryFlags |= 0x2;

		if(isPrelit){
			dcl[i].stream = 0;
			dcl[i].offset = stride;
			dcl[i].type = D3DDECLTYPE_D3DCOLOR;
			dcl[i].method = D3DDECLMETHOD_DEFAULT;
			dcl[i].usage = D3DDECLUSAGE_COLOR;
			dcl[i].usageIndex = 0;
			i++;
			s->geometryFlags |= 0x8;
			stride += 4;
		}

		for(int32 n = 0; n < geo->numTexCoordSets; n++){
			dcl[i].stream = 0;
			dcl[i].offset = stride;
			dcl[i].type = D3DDECLTYPE_FLOAT2;
			dcl[i].method = D3DDECLMETHOD_DEFAULT;
			dcl[i].usage = D3DDECLUSAGE_TEXCOORD;
			dcl[i].usageIndex = (uint8)n;
			i++;
			s->geometryFlags |= 0x10 << n;
			stride += 8;
		}

		if(hasNormals){
			dcl[i].stream = 0;
			dcl[i].offset = stride;
			dcl[i].type = D3DDECLTYPE_FLOAT3;
			dcl[i].method = D3DDECLMETHOD_DEFAULT;
			dcl[i].usage = D3DDECLUSAGE_NORMAL;
			dcl[i].usageIndex = 0;
			i++;
			s->geometryFlags |= 0x4;
			stride += 12;
		}

		dcl[i].stream = 0;
		dcl[i].offset = stride;
		dcl[i].type = D3DDECLTYPE_FLOAT4;
		dcl[i].method = D3DDECLMETHOD_DEFAULT;
		dcl[i].usage = D3DDECLUSAGE_BLENDWEIGHT;
		dcl[i].usageIndex = 0;
		i++;
		stride += 16;

		dcl[i].stream = 0;
		dcl[i].offset = stride;
		dcl[i].type = D3DDECLTYPE_UBYTE4;
		dcl[i].method = D3DDECLMETHOD_DEFAULT;
		dcl[i].usage = D3DDECLUSAGE_BLENDINDICES;
		dcl[i].usageIndex = 0;
		i++;
		stride += 4;

		// We expect some attributes to always be there, use the constant buffer as fallback
		if(!isPrelit){
			dcl[i].stream = 2;
			dcl[i].offset = offsetof(VertexConstantData, color);
			dcl[i].type = D3DDECLTYPE_D3DCOLOR;
			dcl[i].method = D3DDECLMETHOD_DEFAULT;
			dcl[i].usage = D3DDECLUSAGE_COLOR;
			dcl[i].usageIndex = 0;
			i++;
		}
		if(geo->numTexCoordSets == 0){
			dcl[i].stream = 2;
			dcl[i].offset = offsetof(VertexConstantData, texCoors[0]);
			dcl[i].type = D3DDECLTYPE_FLOAT2;
			dcl[i].method = D3DDECLMETHOD_DEFAULT;
			dcl[i].usage = D3DDECLUSAGE_TEXCOORD;
			dcl[i].usageIndex = 0;
			i++;
		}

		dcl[i] = D3DDECL_END();
		s->stride = stride;

		assert(header->vertexDeclaration == nil);
		header->vertexDeclaration = createVertexDeclaration((VertexElement*)dcl);

		assert(s->vertexBuffer == nil);
		s->vertexBuffer = createVertexBuffer(header->totalNumVertex*s->stride, 0, false);
	}else
		getDeclaration(header->vertexDeclaration, dcl);

	Skin *skin = Skin::get(geo);
	uint8 *verts = lockVertices(s->vertexBuffer, 0, 0, D3DLOCK_NOSYSLOCK);

	// Instance vertices
	if(!reinstance || geo->lockedSinceInst&Geometry::LOCKVERTICES){
		for(i = 0; dcl[i].usage != D3DDECLUSAGE_POSITION || dcl[i].usageIndex != 0; i++)
			;
		instV3d(vertFormatMap[dcl[i].type], verts + dcl[i].offset,
			geo->morphTargets[0].vertices,
			header->totalNumVertex,
			header->vertexStream[dcl[i].stream].stride);
	}

	// Instance prelight colors
	if(isPrelit && (!reinstance || geo->lockedSinceInst&Geometry::LOCKPRELIGHT)){
		for(i = 0; dcl[i].usage != D3DDECLUSAGE_COLOR || dcl[i].usageIndex != 0; i++)
			;
		InstanceData *inst = header->inst;
		uint32 n = header->numMeshes;
		while(n--){
			uint32 stride = header->vertexStream[dcl[i].stream].stride;
			inst->vertexAlpha = instColor(vertFormatMap[dcl[i].type],
				verts + dcl[i].offset + stride*inst->minVert,
				geo->colors + inst->minVert,
				inst->numVertices,
				stride);
			inst++;
		}
	}

	// Instance tex coords
	for(int32 n = 0; n < geo->numTexCoordSets; n++){
		if(!reinstance || geo->lockedSinceInst&(Geometry::LOCKTEXCOORDS<<n)){
			for(i = 0; dcl[i].usage != D3DDECLUSAGE_TEXCOORD || dcl[i].usageIndex != n; i++)
				;
			instTexCoords(vertFormatMap[dcl[i].type], verts + dcl[i].offset,
				geo->texCoords[n],
				header->totalNumVertex,
				header->vertexStream[dcl[i].stream].stride);
		}
	}

	// Instance normals
	if(hasNormals && (!reinstance || geo->lockedSinceInst&Geometry::LOCKNORMALS)){
		for(i = 0; dcl[i].usage != D3DDECLUSAGE_NORMAL || dcl[i].usageIndex != 0; i++)
			;
		instV3d(vertFormatMap[dcl[i].type], verts + dcl[i].offset,
			geo->morphTargets[0].normals,
			header->totalNumVertex,
			header->vertexStream[dcl[i].stream].stride);
	}

	// Instance skin weights
	if(!reinstance){
		for(i = 0; dcl[i].usage != D3DDECLUSAGE_BLENDWEIGHT || dcl[i].usageIndex != 0; i++)
			;
		instV4d(vertFormatMap[dcl[i].type], verts + dcl[i].offset,
			(V4d*)skin->weights,
			header->totalNumVertex,
			header->vertexStream[dcl[i].stream].stride);
	}

	// Instance skin indices
	if(!reinstance){
		for(i = 0; dcl[i].usage != D3DDECLUSAGE_BLENDINDICES || dcl[i].usageIndex != 0; i++)
			;
		// not really colors of course but what the heck
		instColor(vertFormatMap[dcl[i].type], verts + dcl[i].offset,
			  (RGBA*)skin->indices,
			  header->totalNumVertex,
			  header->vertexStream[dcl[i].stream].stride);
	}

	unlockVertices(s->vertexBuffer);
}

enum
{
	VSLOC_boneMatrices = VSLOC_afterLights
};

static float skinMatrices[64*16];

void
uploadSkinMatrices(Atomic *a)
{
	int i;
	Skin *skin = Skin::get(a->geometry);
	float *m = skinMatrices;
	HAnimHierarchy *hier = Skin::getHierarchy(a);

	if(hier){
		Matrix *invMats = (Matrix*)skin->inverseMatrices;
		Matrix tmp, tmp2;

		assert(skin->numBones == hier->numNodes);
		if(hier->flags & HAnimHierarchy::LOCALSPACEMATRICES){
			for(i = 0; i < hier->numNodes; i++){
				invMats[i].flags = 0;
				Matrix::mult(&tmp, &invMats[i], &hier->matrices[i]);
				RawMatrix::transpose((RawMatrix*)m, (RawMatrix*)&tmp);
				m += 12;
			}
		}else{
			Matrix invAtmMat;
			Matrix::invert(&invAtmMat, a->getFrame()->getLTM());
			for(i = 0; i < hier->numNodes; i++){
				invMats[i].flags = 0;
				Matrix::mult(&tmp, &hier->matrices[i], &invAtmMat);
				Matrix::mult(&tmp2, &invMats[i], &tmp);
				RawMatrix::transpose((RawMatrix*)m, (RawMatrix*)&tmp2);
				m += 12;
			}
		}
	}else{
		for(i = 0; i < skin->numBones; i++){
			m[0] = 1.0f;
			m[1] = 0.0f;
			m[2] = 0.0f;
			m[3] = 0.0f;

			m[4] = 0.0f;
			m[5] = 1.0f;
			m[6] = 0.0f;
			m[7] = 0.0f;

			m[8] = 0.0f;
			m[9] = 0.0f;
			m[10] = 1.0f;
			m[11] = 0.0f;

			m += 12;
		}
	}
	d3ddevice->SetVertexShaderConstantF(VSLOC_boneMatrices, skinMatrices, skin->numBones*3);
}

void
skinRenderCB(Atomic *atomic, InstanceDataHeader *header)
{
	int vsBits;
	uint32 flags = atomic->geometry->flags;
	setStreamSource(0, (IDirect3DVertexBuffer9*)header->vertexStream[0].vertexBuffer,
	                           0, header->vertexStream[0].stride);
	setIndices((IDirect3DIndexBuffer9*)header->indexBuffer);
	setVertexDeclaration((IDirect3DVertexDeclaration9*)header->vertexDeclaration);

	vsBits = lightingCB_Shader(atomic);
	uploadMatrices(atomic->getFrame()->getLTM());

	uploadSkinMatrices(atomic);

	// Pick a shader
	if((vsBits & VSLIGHT_MASK) == 0)
		setVertexShader(skin_amb_VS);
	else if((vsBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(skin_amb_dir_VS);
	else
		setVertexShader(skin_all_VS);

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		Material *m = inst->material;

		SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);

		setMaterial(flags, m->color, m->surfaceProps);

		if(inst->material->texture){
			d3d::setTexture(0, m->texture);
			setPixelShader(default_tex_PS);
		}else
			setPixelShader(default_PS);

		drawInst(header, inst);
		inst++;
	}
}

#define VS_NAME g_vs20_main
#define PS_NAME g_ps20_main

void
createSkinShaders(void)
{
	{
		static
#include "shaders/skin_amb_VS.h"
		skin_amb_VS = createVertexShader((void*)VS_NAME);
		assert(skin_amb_VS);
	}
	{
		static
#include "shaders/skin_amb_dir_VS.h"
		skin_amb_dir_VS = createVertexShader((void*)VS_NAME);
		assert(skin_amb_dir_VS);
	}
	// Skinning takes a lot of instructions....lighting may be not possible
	// TODO: should do something about this
	{
		static
#include "shaders/skin_all_VS.h"
		skin_all_VS = createVertexShader((void*)VS_NAME);
//		assert(skin_all_VS);
	}
}

void
destroySkinShaders(void)
{
	destroyVertexShader(skin_amb_VS);
	skin_amb_VS = nil;

	destroyVertexShader(skin_amb_dir_VS);
	skin_amb_dir_VS = nil;

	if(skin_all_VS){
		destroyVertexShader(skin_all_VS);
		skin_all_VS = nil;
	}
}

#endif

static void*
skinOpen(void *o, int32, int32)
{
#ifdef RW_D3D9
	createSkinShaders();
#endif

	skinGlobals.pipelines[PLATFORM_D3D9] = makeSkinPipeline();
	return o;
}

static void*
skinClose(void *o, int32, int32)
{
#ifdef RW_D3D9
	destroySkinShaders();
#endif

	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_D3D9])->destroy();
	skinGlobals.pipelines[PLATFORM_D3D9] = nil;
	return o;
}

void
initSkin(void)
{
	Driver::registerPlugin(PLATFORM_D3D9, 0, ID_SKIN,
	                       skinOpen, skinClose);
}

ObjPipeline*
makeSkinPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = skinInstanceCB;
	pipe->uninstanceCB = nil;
	pipe->renderCB = skinRenderCB;
	pipe->pluginID = ID_SKIN;
	pipe->pluginData = 1;
	return pipe;
}

}
}

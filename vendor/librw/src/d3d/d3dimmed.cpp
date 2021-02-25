#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WITH_D3D
#include "../rwbase.h"
#include "../rwplg.h"
#include "../rwrender.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "rwd3d.h"
#include "rwd3d9.h"
#include "rwd3dimpl.h"

namespace rw {
namespace d3d {

#ifdef RW_D3D9

// might want to tweak this
#define NUMINDICES 10000
#define NUMVERTICES 10000

static int primTypeMap[] = {
	D3DPT_POINTLIST,	// invalid
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	D3DPT_POINTLIST,	// actually not supported!
};

static IDirect3DVertexDeclaration9 *im2ddecl;
static IDirect3DVertexBuffer9 *im2dvertbuf;
static IDirect3DIndexBuffer9 *im2dindbuf;

void *im2dOverridePS;

void
openIm2D(void)
{
	D3DVERTEXELEMENT9 elements[4] = {
// can't get proper fog with this :(
//		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, offsetof(Im2DVertex, color), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, offsetof(Im2DVertex, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	assert(im2ddecl == nil);
	im2ddecl = (IDirect3DVertexDeclaration9*)d3d9::createVertexDeclaration((d3d9::VertexElement*)elements);
	assert(im2ddecl);

	assert(im2dvertbuf == nil);
	im2dvertbuf = (IDirect3DVertexBuffer9*)createVertexBuffer(NUMVERTICES*sizeof(Im2DVertex), 0, true);
	assert(im2dvertbuf);
	addDynamicVB(NUMVERTICES*sizeof(Im2DVertex), 0, &im2dvertbuf);

	assert(im2dindbuf == nil);
	im2dindbuf = (IDirect3DIndexBuffer9*)createIndexBuffer(NUMINDICES*sizeof(uint16), true);
	assert(im2dindbuf);
	addDynamicIB(NUMINDICES*sizeof(uint16), &im2dindbuf);
}

void
closeIm2D(void)
{
	d3d9::destroyVertexDeclaration(im2ddecl);
	im2ddecl = nil;

	removeDynamicVB(&im2dvertbuf);
	destroyVertexBuffer(im2dvertbuf);
	im2dvertbuf = nil;

	removeDynamicIB(&im2dindbuf);
	destroyIndexBuffer(im2dindbuf);
	im2dindbuf = nil;
}

static Im2DVertex tmpprimbuf[3];

void
im2DRenderLine(void *vertices, int32 numVertices, int32 vert1, int32 vert2)
{
	Im2DVertex *verts = (Im2DVertex*)vertices;
	tmpprimbuf[0] = verts[vert1];
	tmpprimbuf[1] = verts[vert2];
	im2DRenderPrimitive(PRIMTYPELINELIST, tmpprimbuf, 2);
}

void
im2DRenderTriangle(void *vertices, int32 numVertices, int32 vert1, int32 vert2, int32 vert3)
{
	Im2DVertex *verts = (Im2DVertex*)vertices;
	tmpprimbuf[0] = verts[vert1];
	tmpprimbuf[1] = verts[vert2];
	tmpprimbuf[2] = verts[vert3];
	im2DRenderPrimitive(PRIMTYPETRILIST, tmpprimbuf, 3);
}

void
im2DSetXform(void)
{
	float xform[4];
	Camera *cam;
	cam = (Camera*)engine->currentCamera;
	xform[0] = 2.0f/cam->frameBuffer->width;
	xform[1] = -2.0f/cam->frameBuffer->height;
	xform[2] = -1.0f;
	xform[3] = 1.0f;
	// TODO: should cache this...
	d3ddevice->SetVertexShaderConstantF(VSLOC_afterLights, xform, 1);
}

void
im2DRenderPrimitive(PrimitiveType primType, void *vertices, int32 numVertices)
{
	if(numVertices > NUMVERTICES){
		// TODO: error
		return;
	}
	uint8 *lockedvertices = lockVertices(im2dvertbuf, 0, numVertices*sizeof(Im2DVertex), D3DLOCK_DISCARD);
	memcpy(lockedvertices, vertices, numVertices*sizeof(Im2DVertex));
	unlockVertices(im2dvertbuf);

	setStreamSource(0, im2dvertbuf, 0, sizeof(Im2DVertex));
	setVertexDeclaration(im2ddecl);

	im2DSetXform();

	setVertexShader(im2d_VS);
	if(im2dOverridePS)
		setPixelShader(im2dOverridePS);
	else if(engine->device.getRenderState(TEXTURERASTER))
		setPixelShader(im2d_tex_PS);
	else
		setPixelShader(im2d_PS);

	d3d::flushCache();

	uint32 primCount = 0;
	switch(primType){
	case PRIMTYPELINELIST:
		primCount = numVertices/2;
		break;
	case PRIMTYPEPOLYLINE:
		primCount = numVertices-1;
		break;
	case PRIMTYPETRILIST:
		primCount = numVertices/3;
		break;
	case PRIMTYPETRISTRIP:
		primCount = numVertices-2;
		break;
	case PRIMTYPETRIFAN:
		primCount = numVertices-2;
		break;
	case PRIMTYPEPOINTLIST:
		primCount = numVertices;
		break;
	}
	d3ddevice->DrawPrimitive((D3DPRIMITIVETYPE)primTypeMap[primType], 0, primCount);
}

void
im2DRenderIndexedPrimitive(PrimitiveType primType,
   void *vertices, int32 numVertices, void *indices, int32 numIndices)
{
	if(numVertices > NUMVERTICES ||
	   numIndices > NUMINDICES){
		// TODO: error
		return;
	}
	uint16 *lockedindices = lockIndices(im2dindbuf, 0, numIndices*sizeof(uint16), D3DLOCK_DISCARD);
	memcpy(lockedindices, indices, numIndices*sizeof(uint16));
	unlockIndices(im2dindbuf);

	uint8 *lockedvertices = lockVertices(im2dvertbuf, 0, numVertices*sizeof(Im2DVertex), D3DLOCK_DISCARD);
	memcpy(lockedvertices, vertices, numVertices*sizeof(Im2DVertex));
	unlockVertices(im2dvertbuf);

	setStreamSource(0, im2dvertbuf, 0, sizeof(Im2DVertex));
	setIndices(im2dindbuf);
	setVertexDeclaration(im2ddecl);

	im2DSetXform();

	setVertexShader(im2d_VS);
	if(im2dOverridePS)
		setPixelShader(im2dOverridePS);
	else if(engine->device.getRenderState(TEXTURERASTER))
		setPixelShader(im2d_tex_PS);
	else
		setPixelShader(im2d_PS);

	d3d::flushCache();

	uint32 primCount = 0;
	switch(primType){
	case PRIMTYPELINELIST:
		primCount = numIndices/2;
		break;
	case PRIMTYPEPOLYLINE:
		primCount = numIndices-1;
		break;
	case PRIMTYPETRILIST:
		primCount = numIndices/3;
		break;
	case PRIMTYPETRISTRIP:
		primCount = numIndices-2;
		break;
	case PRIMTYPETRIFAN:
		primCount = numIndices-2;
		break;
	case PRIMTYPEPOINTLIST:
		primCount = numIndices;
		break;
	}
	d3ddevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primTypeMap[primType], 0,
	                                0, numVertices,
	                                0, primCount);
}


// Im3D


static IDirect3DVertexDeclaration9 *im3ddecl;
static IDirect3DVertexBuffer9 *im3dvertbuf;
static IDirect3DIndexBuffer9 *im3dindbuf;
static int32 num3DVertices;

void
openIm3D(void)
{
	D3DVERTEXELEMENT9 elements[4] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, offsetof(Im3DVertex, color), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, offsetof(Im3DVertex, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	assert(im3ddecl == nil);
	im3ddecl = (IDirect3DVertexDeclaration9*)d3d9::createVertexDeclaration((d3d9::VertexElement*)elements);
	assert(im3ddecl);

	assert(im3dvertbuf == nil);
	im3dvertbuf = (IDirect3DVertexBuffer9*)createVertexBuffer(NUMVERTICES*sizeof(Im3DVertex), 0, true);
	assert(im3dvertbuf);
	addDynamicVB(NUMVERTICES*sizeof(Im3DVertex), 0, &im3dvertbuf);

	assert(im3dindbuf == nil);
	im3dindbuf = (IDirect3DIndexBuffer9*)createIndexBuffer(NUMINDICES*sizeof(uint16), true);
	assert(im3dindbuf);
	addDynamicIB(NUMINDICES*sizeof(uint16), &im3dindbuf);
}

void
closeIm3D(void)
{
	d3d9::destroyVertexDeclaration(im3ddecl);
	im3ddecl = nil;

	removeDynamicVB(&im3dvertbuf);
	destroyVertexBuffer(im3dvertbuf);
	im3dvertbuf = nil;

	removeDynamicIB(&im3dindbuf);
	destroyIndexBuffer(im3dindbuf);
	im3dindbuf = nil;
}

void
im3DTransform(void *vertices, int32 numVertices, Matrix *world, uint32 flags)
{
	if(world == nil)
		uploadMatrices();
	else
		uploadMatrices(world);

	if((flags & im3d::VERTEXUV) == 0)
		SetRenderStatePtr(TEXTURERASTER, nil);

	static RGBA white = { 255, 255, 255, 255 };
	static SurfaceProperties surfprops = { 0.0f, 0.0f, 0.0f };
	setMaterial(white, surfprops);

	uint8 *lockedvertices = lockVertices(im3dvertbuf, 0, numVertices*sizeof(Im3DVertex), D3DLOCK_DISCARD);
	memcpy(lockedvertices, vertices, numVertices*sizeof(Im3DVertex));
	unlockVertices(im3dvertbuf);

	setStreamSource(0, im3dvertbuf, 0, sizeof(Im3DVertex));
	setVertexDeclaration(im3ddecl);

	setVertexShader(default_amb_VS);

	num3DVertices = numVertices;
}

void
im3DRenderPrimitive(PrimitiveType primType)
{
	if(engine->device.getRenderState(TEXTURERASTER))
		setPixelShader(default_tex_PS);
	else
		setPixelShader(default_PS);

	d3d::flushCache();

	uint32 primCount = 0;
	switch(primType){
	case PRIMTYPELINELIST:
		primCount = num3DVertices/2;
		break;
	case PRIMTYPEPOLYLINE:
		primCount = num3DVertices-1;
		break;
	case PRIMTYPETRILIST:
		primCount = num3DVertices/3;
		break;
	case PRIMTYPETRISTRIP:
		primCount = num3DVertices-2;
		break;
	case PRIMTYPETRIFAN:
		primCount = num3DVertices-2;
		break;
	case PRIMTYPEPOINTLIST:
		primCount = num3DVertices;
		break;
	}
	d3ddevice->DrawPrimitive((D3DPRIMITIVETYPE)primTypeMap[primType], 0, primCount);
}

void
im3DRenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices)
{
	uint16 *lockedindices = lockIndices(im3dindbuf, 0, numIndices*sizeof(uint16), D3DLOCK_DISCARD);
	memcpy(lockedindices, indices, numIndices*sizeof(uint16));
	unlockIndices(im3dindbuf);

	setIndices(im3dindbuf);

	if(engine->device.getRenderState(TEXTURERASTER))
		setPixelShader(default_tex_PS);
	else
		setPixelShader(default_PS);

	d3d::flushCache();

	uint32 primCount = 0;
	switch(primType){
	case PRIMTYPELINELIST:
		primCount = numIndices/2;
		break;
	case PRIMTYPEPOLYLINE:
		primCount = numIndices-1;
		break;
	case PRIMTYPETRILIST:
		primCount = numIndices/3;
		break;
	case PRIMTYPETRISTRIP:
		primCount = numIndices-2;
		break;
	case PRIMTYPETRIFAN:
		primCount = numIndices-2;
		break;
	case PRIMTYPEPOINTLIST:
		primCount = numIndices;
		break;
	}
	d3ddevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primTypeMap[primType], 0,
	                                0, num3DVertices,
	                                0, primCount);
}

void
im3DEnd(void)
{
}

#endif
}
}

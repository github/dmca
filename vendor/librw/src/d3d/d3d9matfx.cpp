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
#include "../rwanim.h"
#include "../rwengine.h"
#include "../rwrender.h"
#include "../rwplugins.h"
#include "rwd3d.h"
#include "rwd3d9.h"

namespace rw {
namespace d3d9 {
using namespace d3d;

#ifndef RW_D3D9
void matfxRenderCB_Shader(Atomic *atomic, InstanceDataHeader *header) {}
#else

static void *matfx_env_amb_VS;
static void *matfx_env_amb_dir_VS;
static void *matfx_env_all_VS;
static void *matfx_env_PS;
static void *matfx_env_tex_PS;

enum
{
	VSLOC_texMat = VSLOC_afterLights,

	PSLOC_shininess = 1,
	PSLOC_colorClamp = 2
};

void
matfxRender_Default(InstanceDataHeader *header, InstanceData *inst, int32 lightBits)
{
	Material *m = inst->material;

	// Pick a shader
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(default_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(default_amb_dir_VS);
	else
		setVertexShader(default_all_VS);

	SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);

	if(inst->material->texture){
		d3d::setTexture(0, m->texture);
		setPixelShader(default_tex_PS);
	}else
		setPixelShader(default_PS);

	drawInst(header, inst);
}

static Frame *lastEnvFrame;

static RawMatrix normal2texcoord = {
	{ 0.5f,  0.0f, 0.0f }, 0.0f,
	{ 0.0f, -0.5f, 0.0f }, 0.0f,
	{ 0.0f,  0.0f, 1.0f }, 0.0f,
	{ 0.5f,  0.5f, 0.0f }, 1.0f
};

void
uploadEnvMatrix(Frame *frame)
{
	Matrix invMat;
	if(frame == nil)
		frame = engine->currentCamera->getFrame();

	// cache the matrix across multiple meshes
// can't do it, frame matrix may change
//	if(frame == lastEnvFrame)
//		return;
//	lastEnvFrame = frame;

	RawMatrix envMtx, invMtx;
	Matrix::invert(&invMat, frame->getLTM());
	convMatrix(&invMtx, &invMat);
	invMtx.pos.set(0.0f, 0.0f, 0.0f);
	RawMatrix::mult(&envMtx, &invMtx, &normal2texcoord);
	d3ddevice->SetVertexShaderConstantF(VSLOC_texMat, (float*)&envMtx, 4);
}

void
matfxRender_EnvMap(InstanceDataHeader *header, InstanceData *inst, int32 lightBits, MatFX::Env *env)
{
	Material *m = inst->material;

	if(env->tex == nil || env->coefficient == 0.0f){
		matfxRender_Default(header, inst, lightBits);
		return;
	}

	d3d::setTexture(1, env->tex);
	uploadEnvMatrix(env->frame);

	SetRenderState(SRCBLEND, BLENDONE);

	static float zero[4];
	static float one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	struct  {
		float shininess;
		float disableFBA;
		float unused[2];
	} fxparams;
	fxparams.shininess = env->coefficient;
	fxparams.disableFBA = env->fbAlpha ? 0.0f : 1.0f;
	d3ddevice->SetPixelShaderConstantF(PSLOC_shininess, (float*)&fxparams, 1);
	// This clamps the vertex color below. With it we can achieve both PC and PS2 style matfx
	if(MatFX::modulateEnvMap)
		d3ddevice->SetPixelShaderConstantF(PSLOC_colorClamp, zero, 1);
	else
		d3ddevice->SetPixelShaderConstantF(PSLOC_colorClamp, one, 1);

	// Pick a shader
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(matfx_env_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(matfx_env_amb_dir_VS);
	else
		setVertexShader(matfx_env_all_VS);

	bool32 texAlpha = GETD3DRASTEREXT(env->tex->raster)->hasAlpha;

	if(inst->material->texture){
		d3d::setTexture(0, m->texture);
		setPixelShader(matfx_env_tex_PS);
	}else
		setPixelShader(matfx_env_PS);

	SetRenderState(VERTEXALPHA, texAlpha || inst->vertexAlpha || m->color.alpha != 255);

	drawInst(header, inst);

	SetRenderState(SRCBLEND, BLENDSRCALPHA);
}

void
matfxRenderCB_Shader(Atomic *atomic, InstanceDataHeader *header)
{
	int vsBits;
	uint32 flags = atomic->geometry->flags;
	setStreamSource(0, (IDirect3DVertexBuffer9*)header->vertexStream[0].vertexBuffer,
	                           0, header->vertexStream[0].stride);
	setIndices((IDirect3DIndexBuffer9*)header->indexBuffer);
	setVertexDeclaration((IDirect3DVertexDeclaration9*)header->vertexDeclaration);

	lastEnvFrame = nil;

	vsBits = lightingCB_Shader(atomic);
	uploadMatrices(atomic->getFrame()->getLTM());

	bool normals = !!(atomic->geometry->flags & Geometry::NORMALS);

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		Material *m = inst->material;

		setMaterial(flags, m->color, m->surfaceProps);

		MatFX *matfx = MatFX::get(m);
		if(matfx == nil)
			matfxRender_Default(header, inst, vsBits);
		else switch(matfx->type){
		case MatFX::ENVMAP:
			if(normals)
				matfxRender_EnvMap(header, inst, vsBits, &matfx->fx[0].env);
			else
				matfxRender_Default(header, inst, vsBits);
			break;
		case MatFX::NOTHING:
		case MatFX::BUMPMAP:
		case MatFX::BUMPENVMAP:
		case MatFX::DUAL:
		case MatFX::UVTRANSFORM:
		case MatFX::DUALUVTRANSFORM:
			// not supported yet
			matfxRender_Default(header, inst, vsBits);
			break;			
		}

		inst++;
	}
	d3d::setTexture(1, nil);
}

#define VS_NAME g_vs20_main
#define PS_NAME g_ps20_main

void
createMatFXShaders(void)
{
	{
		static
#include "shaders/matfx_env_amb_VS.h"
		matfx_env_amb_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_amb_VS);
	}
	{
		static
#include "shaders/matfx_env_amb_dir_VS.h"
		matfx_env_amb_dir_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_amb_dir_VS);
	}
	{
		static
#include "shaders/matfx_env_all_VS.h"
		matfx_env_all_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_all_VS);
	}


	{
		static
#include "shaders/matfx_env_PS.h"
		matfx_env_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_env_PS);
	}
	{
		static
#include "shaders/matfx_env_tex_PS.h"
		matfx_env_tex_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_env_tex_PS);
	}

}

void
destroyMatFXShaders(void)
{
	destroyVertexShader(matfx_env_amb_VS);
	matfx_env_amb_VS = nil;

	destroyVertexShader(matfx_env_amb_dir_VS);
	matfx_env_amb_dir_VS = nil;

	destroyVertexShader(matfx_env_all_VS);
	matfx_env_all_VS = nil;


	destroyPixelShader(matfx_env_PS);
	matfx_env_PS = nil;

	destroyPixelShader(matfx_env_tex_PS);
	matfx_env_tex_PS = nil;
}

#endif

static void*
matfxOpen(void *o, int32, int32)
{
#ifdef RW_D3D9
	createMatFXShaders();
#endif

	matFXGlobals.pipelines[PLATFORM_D3D9] = makeMatFXPipeline();
	return o;
}

static void*
matfxClose(void *o, int32, int32)
{
#ifdef RW_D3D9
	destroyMatFXShaders();
#endif

	((ObjPipeline*)matFXGlobals.pipelines[PLATFORM_D3D9])->destroy();
	matFXGlobals.pipelines[PLATFORM_D3D9] = nil;
	return o;
}

void
initMatFX(void)
{
	Driver::registerPlugin(PLATFORM_D3D9, 0, ID_MATFX,
	                       matfxOpen, matfxClose);
}

ObjPipeline*
makeMatFXPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->renderCB = matfxRenderCB_Shader;
	pipe->pluginID = ID_MATFX;
	pipe->pluginData = 0;
	return pipe;
}

}
}

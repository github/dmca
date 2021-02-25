#ifndef LIBRW

#define WITHD3D
#include "common.h"
#include "rpmatfx.h"

struct MatFXNothing { int pad[5]; int effect; };

struct MatFXBump
{
	RwFrame *bumpFrame;
	RwTexture *bumpedTex;
	RwTexture *bumpTex;
	float negBumpCoefficient;
	int pad;
	int effect;
};

struct MatFXEnv
{
	RwFrame *envFrame;
	RwTexture *envTex;
	float envCoeff;
	int envFBalpha;
	int pad;
	int effect;
};

struct MatFXDual
{
	RwTexture *dualTex;
	RwInt32 srcBlend;
	RwInt32 dstBlend;
};


struct MatFX
{
	union {
		MatFXNothing n;
		MatFXBump b;
		MatFXEnv e;
		MatFXDual d;
	} fx[2];
	int effects;
};

extern "C" {
	extern int MatFXMaterialDataOffset;
	extern int MatFXAtomicDataOffset;

	void _rpMatFXD3D8AtomicMatFXEnvRender(RxD3D8InstanceData* inst, int flags, int sel, RwTexture* texture, RwTexture* envMap);
	void _rpMatFXD3D8AtomicMatFXRenderBlack(RxD3D8InstanceData *inst);
	void _rpMatFXD3D8AtomicMatFXBumpMapRender(RxD3D8InstanceData *inst, int flags, RwTexture *texture, RwTexture *bumpMap, RwTexture *envMap);
	void _rpMatFXD3D8AtomicMatFXDualPassRender(RxD3D8InstanceData *inst, int flags, RwTexture *texture, RwTexture *dualTexture);
}


#ifdef PS2_MATFX

void
_rpMatFXD3D8AtomicMatFXDefaultRender(RxD3D8InstanceData *inst, int flags, RwTexture *texture)
{
	if(flags & (rpGEOMETRYTEXTURED|rpGEOMETRYTEXTURED2) && texture)
		RwD3D8SetTexture(texture, 0);
	else
		RwD3D8SetTexture(nil, 0);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)(inst->vertexAlpha || inst->material->color.alpha != 0xFF));
	RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, inst->vertexAlpha != 0);
	RwD3D8SetPixelShader(0);
	RwD3D8SetVertexShader(inst->vertexShader);
	RwD3D8SetStreamSource(0, inst->vertexBuffer, inst->stride);

	if(inst->indexBuffer){
		RwD3D8SetIndices(inst->indexBuffer, inst->baseIndex);
		RwD3D8DrawIndexedPrimitive(inst->primType, 0, inst->numVertices, 0, inst->numIndices);
	}else
		RwD3D8DrawPrimitive(inst->primType, inst->baseIndex, inst->numVertices);
}

// map [-1; -1] -> [0; 1], flip V
static RwMatrix scalenormal = {
	{ 0.5f, 0.0f, 0.0f }, 0,
	{ 0.0f, -0.5f, 0.0f }, 0,
	{ 0.0f, 0.0f, 1.0f }, 0,
	{ 0.5f, 0.5f, 0.0f }, 0,
	
};

// flipped U for PS2
static RwMatrix scalenormal_flipU = {
	{ -0.5f, 0.0f, 0.0f }, 0,
	{ 0.0f, -0.5f, 0.0f }, 0,
	{ 0.0f, 0.0f, 1.0f }, 0,
	{ 0.5f, 0.5f, 0.0f }, 0,
	
};

void
ApplyEnvMapTextureMatrix(RwTexture *tex, int n, RwFrame *frame)
{
	RwD3D8SetTexture(tex, n);
	RwD3D8SetTextureStageState(n, D3DRS_ALPHAREF, 2);
	RwD3D8SetTextureStageState(n, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
	if(frame){
		RwMatrix *envframemat = RwMatrixCreate();
		RwMatrix *tmpmat = RwMatrixCreate();
		RwMatrix *envmat = RwMatrixCreate();

		RwMatrixInvert(envframemat, RwFrameGetLTM(frame));
		// PS2
		// can this be simplified?
		*tmpmat = *RwFrameGetLTM(RwCameraGetFrame((RwCamera*)RWSRCGLOBAL(curCamera)));
		RwV3dNegate(&tmpmat->right, &tmpmat->right);
		tmpmat->flags = 0;
		tmpmat->pos.x = 0.0f;
		tmpmat->pos.y = 0.0f;
		tmpmat->pos.z = 0.0f;
		RwMatrixMultiply(envmat, tmpmat, envframemat);
		*tmpmat = *envmat;
		// important because envframemat can have a translation that we don't like
		tmpmat->pos.x = 0.0f;
		tmpmat->pos.y = 0.0f;
		tmpmat->pos.z = 0.0f;
		// for some reason we flip in U as well
		RwMatrixMultiply(envmat, tmpmat, &scalenormal_flipU);

		RwD3D8SetTransform(D3DTS_TEXTURE0+n, envmat);

		RwMatrixDestroy(envmat);
		RwMatrixDestroy(tmpmat);
		RwMatrixDestroy(envframemat);
	}else
		RwD3D8SetTransform(D3DTS_TEXTURE0+n, &scalenormal);
}

void
_rpMatFXD3D8AtomicMatFXEnvRender_ps2(RxD3D8InstanceData *inst, int flags, int sel, RwTexture *texture, RwTexture *envMap)
{
	MatFX *matfx = *RWPLUGINOFFSET(MatFX*, inst->material, MatFXMaterialDataOffset);
	MatFXEnv *env = &matfx->fx[sel].e;

	uint8 intens = (uint8)(env->envCoeff*255.0f);

	if(intens == 0 || envMap == nil){
		if(sel == 0)
			_rpMatFXD3D8AtomicMatFXDefaultRender(inst, flags, texture);
		return;
	}

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)(inst->vertexAlpha || inst->material->color.alpha != 0xFF));
	if(flags & (rpGEOMETRYTEXTURED|rpGEOMETRYTEXTURED2) && texture)
		RwD3D8SetTexture(texture, 0);
	else
		RwD3D8SetTexture(nil, 0);
	RwD3D8SetPixelShader(0);
	RwD3D8SetVertexShader(inst->vertexShader);
	RwD3D8SetStreamSource(0, inst->vertexBuffer, inst->stride);
	RwD3D8SetIndices(inst->indexBuffer, inst->baseIndex);
	if(inst->indexBuffer)
		RwD3D8DrawIndexedPrimitive(inst->primType, 0, inst->numVertices, 0, inst->numIndices);
	else
		RwD3D8DrawPrimitive(inst->primType, inst->baseIndex, inst->numVertices);

	// Effect pass
	
	ApplyEnvMapTextureMatrix(envMap, 0, env->envFrame);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwUInt32 src, dst, lighting, zwrite, fog, fogcol;
	RwRenderStateGet(rwRENDERSTATESRCBLEND, &src);
	RwRenderStateGet(rwRENDERSTATEDESTBLEND, &dst);

	// This is of course not using framebuffer alpha,
	// but if the diffuse texture had no alpha, the result should actually be rather the same
	if(env->envFBalpha)
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	else
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwD3D8GetRenderState(D3DRS_LIGHTING, &lighting);
	RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
	RwD3D8GetRenderState(D3DRS_FOGENABLE, &fog);
	RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	if(fog){
		RwD3D8GetRenderState(D3DRS_FOGCOLOR, &fogcol);
		RwD3D8SetRenderState(D3DRS_FOGCOLOR, 0);
	}

	D3DCOLOR texfactor = D3DCOLOR_RGBA(intens, intens, intens, intens);
	RwD3D8SetRenderState(D3DRS_TEXTUREFACTOR, texfactor);
	RwD3D8SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RwD3D8SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	RwD3D8SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	// alpha unused
	//RwD3D8SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	//RwD3D8SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	//RwD3D8SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	if(inst->indexBuffer)
		RwD3D8DrawIndexedPrimitive(inst->primType, 0, inst->numVertices, 0, inst->numIndices);
	else
		RwD3D8DrawPrimitive(inst->primType, inst->baseIndex, inst->numVertices);

	// Reset states

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)src);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)dst);
	RwD3D8SetRenderState(D3DRS_LIGHTING, lighting);
	RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
	if(fog)
		RwD3D8SetRenderState(D3DRS_FOGCOLOR, fogcol);
	RwD3D8SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RwD3D8SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	RwD3D8SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	RwD3D8SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
}

void
_rwD3D8EnableClippingIfNeeded(void *object, RwUInt8 type)
{
	int clip;
	if (type == rpATOMIC)
		clip = !RwD3D8CameraIsSphereFullyInsideFrustum(RwCameraGetCurrentCameraMacro(), RpAtomicGetWorldBoundingSphere((RpAtomic *)object));
	else
		clip = !RwD3D8CameraIsBBoxFullyInsideFrustum(RwCameraGetCurrentCameraMacro(), &((RpWorldSector *)object)->tightBoundingBox);
	RwD3D8SetRenderState(D3DRS_CLIPPING, clip);
}

void
_rwD3D8AtomicMatFXRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	RwBool lighting;
	RwBool forceBlack;
	RxD3D8ResEntryHeader *header;
	RxD3D8InstanceData *inst;
	RwInt32 i;

	if (flags & rpGEOMETRYPRELIT) {
		RwD3D8SetRenderState(D3DRS_COLORVERTEX, 1);
		RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	} else {
		RwD3D8SetRenderState(D3DRS_COLORVERTEX, 0);
		RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	}

	_rwD3D8EnableClippingIfNeeded(object, type);

	RwD3D8GetRenderState(D3DRS_LIGHTING, &lighting);
	if (lighting || flags & rpGEOMETRYPRELIT) {
		forceBlack = FALSE;
	} else {
		forceBlack = TRUE;
		RwD3D8SetTexture(nil, 0);
		RwD3D8SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 255));
		RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	}

	header = (RxD3D8ResEntryHeader *)(repEntry + 1);
	inst = (RxD3D8InstanceData *)(header + 1);
	for (i = 0; i < header->numMeshes; i++) {
		if (forceBlack)
			_rpMatFXD3D8AtomicMatFXRenderBlack(inst);
		else {
			if (lighting)
				RwD3D8SetSurfaceProperties(&inst->material->color, &inst->material->surfaceProps, flags & rpGEOMETRYMODULATEMATERIALCOLOR);
			MatFX *matfx = *RWPLUGINOFFSET(MatFX *, inst->material, MatFXMaterialDataOffset);
			int effect = matfx ? matfx->effects : rpMATFXEFFECTNULL;
			switch (effect) {
			case rpMATFXEFFECTNULL:
			default:
				_rpMatFXD3D8AtomicMatFXDefaultRender(inst, flags, inst->material->texture);
				break;
			case rpMATFXEFFECTBUMPMAP:
				_rpMatFXD3D8AtomicMatFXBumpMapRender(inst, flags, inst->material->texture, matfx->fx[0].b.bumpedTex, nil);
				break;
			case rpMATFXEFFECTENVMAP:
			{
				// TODO: matfx switch in the settings
				//_rpMatFXD3D8AtomicMatFXEnvRender(inst, flags, 0, inst->material->texture, matfx->fx[0].e.envTex);
				_rpMatFXD3D8AtomicMatFXEnvRender_ps2(inst, flags, 0, inst->material->texture, matfx->fx[0].e.envTex);
				break;
			}
			case rpMATFXEFFECTBUMPENVMAP:
				_rpMatFXD3D8AtomicMatFXBumpMapRender(inst, flags, inst->material->texture, matfx->fx[0].b.bumpedTex, matfx->fx[1].e.envTex);
				break;
			case rpMATFXEFFECTDUAL:
				_rpMatFXD3D8AtomicMatFXDualPassRender(inst, flags, inst->material->texture, matfx->fx[0].d.dualTex);
				break;
			}
		}
		inst++;
	}

	if (forceBlack) {
		RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}
}

void
ReplaceMatFxCallback()
{
	RxD3D8AllInOneSetRenderCallBack(
	    RxPipelineFindNodeByName(RpMatFXGetD3D8Pipeline(rpMATFXD3D8ATOMICPIPELINE), RxNodeDefinitionGetD3D8AtomicAllInOne()->name, nil, nil),
	    _rwD3D8AtomicMatFXRenderCallback);
	
}
#endif // PS2_MATFX

#endif // !LIBRW

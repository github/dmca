#ifndef LIBRW

#define WITHD3D
#include "common.h"
#ifdef PS2_ALPHA_TEST
#include "rwcore.h"

extern "C" {
RwBool _rwD3D8RenderStateIsVertexAlphaEnable(void);
RwBool _rwD3D8RenderStateVertexAlphaEnable(RwBool enable);
RwRaster *_rwD3D8RWGetRasterStage(RwUInt32 stage);
}

extern bool gPS2alphaTest;

void
_rxD3D8DualPassRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	RxD3D8ResEntryHeader *resEntryHeader;
	RxD3D8InstanceData *instancedData;
	RwInt32 numMeshes;
	RwBool lighting;
	RwBool vertexAlphaBlend;
	RwBool forceBlack;
	RwUInt32 ditherEnable;
	RwUInt32 shadeMode;
	void *lastVertexBuffer;

	/* Get lighting state */
	RwD3D8GetRenderState(D3DRS_LIGHTING, &lighting);

	forceBlack = FALSE;

	if (lighting) {
		if (flags & rxGEOMETRY_PRELIT) {
			/* Emmisive color from the vertex colors */
			RwD3D8SetRenderState(D3DRS_COLORVERTEX, TRUE);
			RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
		} else {
			/* Emmisive color from material, set to black in the submit node */
			RwD3D8SetRenderState(D3DRS_COLORVERTEX, FALSE);
			RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		}
	} else {
		if ((flags & rxGEOMETRY_PRELIT) == 0) {
			forceBlack = TRUE;

			RwD3D8GetRenderState(D3DRS_DITHERENABLE, &ditherEnable);
			RwD3D8GetRenderState(D3DRS_SHADEMODE, &shadeMode);

			RwD3D8SetRenderState(D3DRS_TEXTUREFACTOR, 0xff000000);
			RwD3D8SetRenderState(D3DRS_DITHERENABLE, FALSE);
			RwD3D8SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		}
	}

	/* Enable clipping */
	if (type == rpATOMIC) {
		RpAtomic *atomic;
		RwCamera *cam;

		atomic = (RpAtomic *)object;

		cam = RwCameraGetCurrentCamera();
		// RWASSERT(cam);

		if (RwD3D8CameraIsSphereFullyInsideFrustum(cam, RpAtomicGetWorldBoundingSphere(atomic))) {
			RwD3D8SetRenderState(D3DRS_CLIPPING, FALSE);
		} else {
			RwD3D8SetRenderState(D3DRS_CLIPPING, TRUE);
		}
	} else {
		RpWorldSector *worldSector;
		RwCamera *cam;

		worldSector = (RpWorldSector *)object;

		cam = RwCameraGetCurrentCamera();
		// RWASSERT(cam);

		if (RwD3D8CameraIsBBoxFullyInsideFrustum(cam, RpWorldSectorGetTightBBox(worldSector))) {
			RwD3D8SetRenderState(D3DRS_CLIPPING, FALSE);
		} else {
			RwD3D8SetRenderState(D3DRS_CLIPPING, TRUE);
		}
	}

	/* Set texture to NULL if hasn't any texture flags */
	if ((flags & (rxGEOMETRY_TEXTURED | rpGEOMETRYTEXTURED2)) == 0) {
		RwD3D8SetTexture(NULL, 0);

		if (forceBlack) {
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

			RwD3D8SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	/* Get vertex alpha Blend state */
	vertexAlphaBlend = _rwD3D8RenderStateIsVertexAlphaEnable();

	/* Set Last vertex buffer to force the call */
	lastVertexBuffer = (void *)0xffffffff;

	/* Get the instanced data */
	resEntryHeader = (RxD3D8ResEntryHeader *)(repEntry + 1);
	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);

	/*
	 * Data shared between meshes
	 */

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(0);

	/*
	 * Vertex shader
	 */
	RwD3D8SetVertexShader(instancedData->vertexShader);

	/* Get the number of meshes */
	numMeshes = resEntryHeader->numMeshes;
	while (numMeshes--) {
		// RWASSERT(instancedData->material != NULL);

		if ((flags & (rxGEOMETRY_TEXTURED | rpGEOMETRYTEXTURED2))) {
			RwD3D8SetTexture(instancedData->material->texture, 0);

			if (forceBlack) {
				/* Only change the colorop, we need to use the texture alpha channel */
				RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
				RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			}
		}

		if (instancedData->vertexAlpha || (0xFF != instancedData->material->color.alpha)) {
			if (!vertexAlphaBlend) {
				vertexAlphaBlend = TRUE;

				_rwD3D8RenderStateVertexAlphaEnable(TRUE);
			}
		} else {
			if (vertexAlphaBlend) {
				vertexAlphaBlend = FALSE;

				_rwD3D8RenderStateVertexAlphaEnable(FALSE);
			}
		}

		if (lighting) {
			if (instancedData->vertexAlpha) {
				RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			} else {
				RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
			}

			RwD3D8SetSurfaceProperties(&instancedData->material->color, &instancedData->material->surfaceProps, (flags & rxGEOMETRY_MODULATE));
		}

		/*
		 * Render
		 */

		/* Set the stream source */
		if (lastVertexBuffer != instancedData->vertexBuffer) {
			RwD3D8SetStreamSource(0, instancedData->vertexBuffer, instancedData->stride);

			lastVertexBuffer = instancedData->vertexBuffer;
		}
		if (!gPS2alphaTest) {
			/* Set the Index buffer */
			if (instancedData->indexBuffer != NULL) {
				RwD3D8SetIndices(instancedData->indexBuffer, instancedData->baseIndex);

				/* Draw the indexed primitive */
				RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedData->primType, 0, instancedData->numVertices, 0, instancedData->numIndices);
			} else {
				RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedData->primType, instancedData->baseIndex, instancedData->numVertices);
			}
		} else {
			RwD3D8SetIndices(instancedData->indexBuffer, instancedData->baseIndex);

			int hasAlpha, alphafunc, alpharef, zwrite;
			RwD3D8GetRenderState(D3DRS_ALPHABLENDENABLE, &hasAlpha);
			RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
			if (hasAlpha && zwrite) {
				RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
				RwD3D8GetRenderState(D3DRS_ALPHAREF, &alpharef);

				RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
				RwD3D8SetRenderState(D3DRS_ALPHAREF, 128);

				if (instancedData->indexBuffer)
					RwD3D8DrawIndexedPrimitive(instancedData->primType, 0, instancedData->numVertices, 0, instancedData->numIndices);
				else
					RwD3D8DrawPrimitive(instancedData->primType, instancedData->baseIndex, instancedData->numVertices);

				RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);

				if (instancedData->indexBuffer)
					RwD3D8DrawIndexedPrimitive(instancedData->primType, 0, instancedData->numVertices, 0, instancedData->numIndices);
				else
					RwD3D8DrawPrimitive(instancedData->primType, instancedData->baseIndex, instancedData->numVertices);

				RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
				RwD3D8SetRenderState(D3DRS_ALPHAREF, alpharef);
				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
			} else {
				if (instancedData->indexBuffer)
					RwD3D8DrawIndexedPrimitive(instancedData->primType, 0, instancedData->numVertices, 0, instancedData->numIndices);
				else
					RwD3D8DrawPrimitive(instancedData->primType, instancedData->baseIndex, instancedData->numVertices);
			}
		}

		/* Move onto the next instancedData */
		instancedData++;
	}

	if (forceBlack) {
		RwD3D8SetRenderState(D3DRS_DITHERENABLE, ditherEnable);
		RwD3D8SetRenderState(D3DRS_SHADEMODE, shadeMode);

		if (_rwD3D8RWGetRasterStage(0)) {
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		} else {
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		}
	}
}

void
ReplaceAtomicPipeCallback()
{
	RxD3D8AllInOneSetRenderCallBack(RxPipelineFindNodeByName(RXPIPELINEGLOBAL(platformAtomicPipeline), RxNodeDefinitionGetD3D8AtomicAllInOne()->name, nil, nil),
	                                _rxD3D8DualPassRenderCallback);
}

#endif // PS2_ALPHA_TEST

#endif // !LIBRW
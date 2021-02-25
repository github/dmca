#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WITH_D3D
#include "../rwbase.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "rwd3d.h"
#include "rwd3d8.h"

namespace rw {
namespace d3d8 {
using namespace d3d;

#ifndef RW_D3D9
void defaultRenderCB(Atomic*, InstanceDataHeader*) {}
#else

// This is a bit abandoned, use d3d9 instead

void
defaultRenderCB(Atomic *atomic, InstanceDataHeader *header)
{
	RawMatrix world;

	d3d::lightingCB_Fix(atomic);

	uint32 flags = atomic->geometry->flags;
	d3d::setRenderState(D3DRS_LIGHTING, !!(flags & rw::Geometry::LIGHT));

	Frame *f = atomic->getFrame();
	convMatrix(&world, f->getLTM());
	d3ddevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&world);

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		d3d::setTexture(0, inst->material->texture);
		d3d::setMaterial(flags, inst->material->color, inst->material->surfaceProps);


		d3d::setRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		d3d::setRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		if(flags & Geometry::PRELIT)
			d3d::setRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
		else
			d3d::setRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

		d3ddevice->SetFVF(inst->vertexShader);
		setStreamSource(0, (IDirect3DVertexBuffer9*)inst->vertexBuffer, 0, inst->stride);
		setIndices((IDirect3DIndexBuffer9*)inst->indexBuffer);
		uint32 numPrim = inst->primType == D3DPT_TRIANGLESTRIP ? inst->numIndices-2 : inst->numIndices/3;
		d3d::flushCache();
		d3ddevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)inst->primType, inst->baseIndex,
		                                0, inst->numVertices, 0, numPrim);
		inst++;
	}
}

#endif
}
}

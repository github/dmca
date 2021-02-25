#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "../rwanim.h"
#include "../rwplugins.h"
#include "rwps2.h"
#include "rwps2plg.h"

namespace rw {
namespace ps2 {

struct PdsGlobals
{
	Pipeline **pipes;
	int32 maxPipes;
	int32 numPipes;
};
static PdsGlobals pdsGlobals;

Pipeline*
getPDSPipe(uint32 data)
{
	for(int32 i = 0; i < pdsGlobals.numPipes; i++)
		if(pdsGlobals.pipes[i]->pluginData == data)
			return pdsGlobals.pipes[i];
	return nil;
}

void
registerPDSPipe(Pipeline *pipe)
{
	if(pdsGlobals.pipes == nil)
		pdsGlobals.pipes = rwNewT(Pipeline*, pdsGlobals.maxPipes, MEMDUR_GLOBAL | ID_PDS);
	assert(pdsGlobals.numPipes < pdsGlobals.maxPipes);
	pdsGlobals.pipes[pdsGlobals.numPipes++] = pipe;
}

static void
atomicPDSRights(void *object, int32, int32, uint32 data)
{
	Atomic *a = (Atomic*)object;
	a->pipeline = (ObjPipeline*)getPDSPipe(data);
//	printf("atm pds: %x %x %x\n", data, a->pipeline->pluginID, a->pipeline->pluginData);
}

static void
materialPDSRights(void *object, int32, int32, uint32 data)
{
	Material *m = (Material*)object;
	m->pipeline = (ObjPipeline*)getPDSPipe(data);
//	printf("mat pds: %x %x %x\n", data, m->pipeline->pluginID, m->pipeline->pluginData);
}

static void *pdsOpen(void *object, int32 offset, int32 size) { return object; }
static void*
pdsClose(void *object, int32 offset, int32 size)
{
	// TODO MEMORY: free registered pipelines
	rwFree(pdsGlobals.pipes);
	return object;
}

void
registerPDSPlugin(int32 n)
{
	pdsGlobals.maxPipes = n;
	pdsGlobals.numPipes = 0;
	pdsGlobals.pipes = nil;
	Engine::registerPlugin(0, ID_PDS, pdsOpen, pdsClose);
	Atomic::registerPlugin(0, ID_PDS, nil, nil, nil);
	Atomic::setStreamRightsCallback(ID_PDS, atomicPDSRights);

	Material::registerPlugin(0, ID_PDS, nil, nil, nil);
	Material::setStreamRightsCallback(ID_PDS, materialPDSRights);
}

void
registerPluginPDSPipes(void)
{
	// TODO: how do we destroy them?

	// rwPDS_G3_Skin_GrpMatPipeID
	MatPipeline *pipe = MatPipeline::create();
	pipe->pluginID = ID_PDS;
	pipe->pluginData = 0x11001;
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
	registerPDSPipe(pipe);

	// rwPDS_G3_Skin_GrpAtmPipeID
	ObjPipeline *opipe = ObjPipeline::create();
	opipe->pluginID = ID_PDS;
	opipe->pluginData = 0x11002;
	opipe->groupPipeline = pipe;
	registerPDSPipe(opipe);

	// rwPDS_G3_MatfxUV1_GrpMatPipeID
	pipe = MatPipeline::create();
	pipe->pluginID = ID_PDS;
	pipe->pluginData = 0x1100b;
	pipe->attribs[AT_XYZ] = &attribXYZ;
	pipe->attribs[AT_UV] = &attribUV;
	pipe->attribs[AT_RGBA] = &attribRGBA;
	pipe->attribs[AT_NORMAL] = &attribNormal;
	vertCount = MatPipeline::getVertCount(0x3C5, 4, 3, 3);
	pipe->setTriBufferSizes(4, vertCount);
	pipe->vifOffset = pipe->inputStride*vertCount;
	pipe->uninstanceCB = genericUninstanceCB;
	registerPDSPipe(pipe);

	// rwPDS_G3_MatfxUV1_GrpAtmPipeID
	opipe = ObjPipeline::create();
	opipe->pluginID = ID_PDS;
	opipe->pluginData = 0x1100d;
	opipe->groupPipeline = pipe;
	registerPDSPipe(opipe);

	// rwPDS_G3_MatfxUV2_GrpMatPipeID
	pipe = MatPipeline::create();
	pipe->pluginID = ID_PDS;
	pipe->pluginData = 0x1100c;
	pipe->attribs[AT_XYZ] = &attribXYZ;
	pipe->attribs[AT_UV] = &attribUV2;
	pipe->attribs[AT_RGBA] = &attribRGBA;
	pipe->attribs[AT_NORMAL] = &attribNormal;
	vertCount = MatPipeline::getVertCount(0x3C5, 4, 3, 3);
	pipe->setTriBufferSizes(4, vertCount);
	pipe->vifOffset = pipe->inputStride*vertCount;
	pipe->uninstanceCB = genericUninstanceCB;
	registerPDSPipe(pipe);

	// rwPDS_G3_MatfxUV2_GrpAtmPipeID
	opipe = ObjPipeline::create();
	opipe->pluginID = ID_PDS;
	opipe->pluginData = 0x1100e;
	opipe->groupPipeline = pipe;
	registerPDSPipe(opipe);

	// RW World plugin

	// rwPDS_G3x_Generic_AtmPipeID
	opipe = ObjPipeline::create();
	opipe->pluginID = ID_PDS;
	opipe->pluginData = 0x50001;
	registerPDSPipe(opipe);

	// rwPDS_G3x_Skin_AtmPipeID
	opipe = ObjPipeline::create();
	opipe->pluginID = ID_PDS;
	opipe->pluginData = 0x5000b;
	registerPDSPipe(opipe);

	// rwPDS_G3xd_A4D_MatPipeID
	pipe = MatPipeline::create();
	pipe->pluginID = ID_PDS;
	pipe->pluginData = 0x5002f;
	pipe->attribs[0] = &attribXYZW;
	pipe->attribs[1] = &attribUV;
	pipe->attribs[2] = &attribNormal;
	vertCount = 0x50;
	pipe->setTriBufferSizes(3, vertCount);
	pipe->vifOffset = pipe->inputStride*vertCount;	// 0xF0
	pipe->uninstanceCB = genericUninstanceCB;
	pipe->preUninstCB = genericPreCB;
	registerPDSPipe(pipe);

	// rwPDS_G3xd_A4DSkin_MatPipeID
	pipe = MatPipeline::create();
	pipe->pluginID = ID_PDS;
	pipe->pluginData = 0x5003e;
	pipe->attribs[0] = &attribXYZW;
	pipe->attribs[1] = &attribUV;
	pipe->attribs[2] = &attribNormal;
	pipe->attribs[3] = &attribWeights;
	vertCount = 0x30;
	pipe->setTriBufferSizes(4, vertCount);	// 0xC0
	pipe->vifOffset = pipe->inputStride*vertCount;
	pipe->instanceCB = skinInstanceCB;
	pipe->uninstanceCB = genericUninstanceCB;
	pipe->preUninstCB = genericPreCB;
	pipe->postUninstCB = skinPostCB;
	registerPDSPipe(pipe);
}

}
}

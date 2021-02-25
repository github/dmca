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

#define PLUGIN_ID ID_MATFX

namespace rw {
namespace ps2 {

static void*
matfxOpen(void *o, int32, int32)
{
	matFXGlobals.pipelines[PLATFORM_PS2] = makeMatFXPipeline();
	return o;
}

static void*
matfxClose(void *o, int32, int32)
{
	((ObjPipeline*)matFXGlobals.pipelines[PLATFORM_PS2])->groupPipeline->destroy();
	((ObjPipeline*)matFXGlobals.pipelines[PLATFORM_PS2])->destroy();
	matFXGlobals.pipelines[PLATFORM_PS2] = nil;
	return o;
}

void
initMatFX(void)
{
	Driver::registerPlugin(PLATFORM_PS2, 0, ID_MATFX,
	                       matfxOpen, matfxClose);
}

ObjPipeline*
makeMatFXPipeline(void)
{
	MatPipeline *pipe = MatPipeline::create();
	pipe->pluginID = ID_MATFX;
	pipe->pluginData = 0;
	pipe->attribs[AT_XYZ] = &attribXYZ;
	pipe->attribs[AT_UV] = &attribUV;
	pipe->attribs[AT_RGBA] = &attribRGBA;
	pipe->attribs[AT_NORMAL] = &attribNormal;
	uint32 vertCount = MatPipeline::getVertCount(0x3C5, 4, 3, 3);
	pipe->setTriBufferSizes(4, vertCount);
	pipe->vifOffset = pipe->inputStride*vertCount;
	pipe->uninstanceCB = genericUninstanceCB;

	ObjPipeline *opipe = ObjPipeline::create();
	opipe->pluginID = ID_MATFX;
	opipe->pluginData = 0;
	opipe->groupPipeline = pipe;
	return opipe;
}

}
}

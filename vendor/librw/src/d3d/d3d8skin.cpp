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
#include "rwd3d.h"
#include "rwd3d8.h"

namespace rw {
namespace d3d8 {
using namespace d3d;

static void*
skinOpen(void *o, int32, int32)
{
	skinGlobals.pipelines[PLATFORM_D3D8] = makeSkinPipeline();
	return o;
}

static void*
skinClose(void *o, int32, int32)
{
	((ObjPipeline*)skinGlobals.pipelines[PLATFORM_D3D8])->destroy();
	skinGlobals.pipelines[PLATFORM_D3D8] = nil;
	return o;
}

void
initSkin(void)
{
	Driver::registerPlugin(PLATFORM_D3D8, 0, ID_SKIN,
	                       skinOpen, skinClose);
}

ObjPipeline*
makeSkinPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->renderCB = defaultRenderCB;
	pipe->pluginID = ID_SKIN;
	pipe->pluginData = 1;
	return pipe;
}

}
}

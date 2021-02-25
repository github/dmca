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
#include "rwxbox.h"

namespace rw {
namespace xbox {

static void*
matfxOpen(void *o, int32, int32)
{
	matFXGlobals.pipelines[PLATFORM_XBOX] = makeMatFXPipeline();
	return o;
}

static void*
matfxClose(void *o, int32, int32)
{
	((ObjPipeline*)matFXGlobals.pipelines[PLATFORM_XBOX])->destroy();
	matFXGlobals.pipelines[PLATFORM_XBOX] = nil;
	return o;
}

void
initMatFX(void)
{
	Driver::registerPlugin(PLATFORM_XBOX, 0, ID_MATFX,
	                       matfxOpen, matfxClose);
}

ObjPipeline*
makeMatFXPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->pluginID = ID_MATFX;
	pipe->pluginData = 0;
	return pipe;
}

}
}

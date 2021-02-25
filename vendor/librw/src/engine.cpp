#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <new>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"
#include "ps2/rwps2.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"
#include "gl/rwgl3.h"
#include "gl/rwwdgl.h"

#define PLUGIN_ID 0

// on windows
//#ifdef DEBUG
//#include <crtdbg.h>
//#define free(p) _free_dbg(p, _NORMAL_BLOCK);
//#define malloc(sz) _malloc_dbg(sz, _NORMAL_BLOCK, __FILE__, __LINE__)
//#define realloc(p, sz) _realloc_dbg(p, sz, _NORMAL_BLOCK, __FILE__, __LINE__)
//#endif

namespace rw {

Engine *engine;
PluginList Engine::s_plglist(sizeof(Engine));
Engine::State Engine::state = Dead;
MemoryFunctions Engine::memfuncs;
PluginList Driver::s_plglist[NUM_PLATFORMS];

const char *allocLocation;

void *malloc_h(size_t sz, uint32 hint) { if(sz == 0) return nil; return malloc(sz); }
void *realloc_h(void *p, size_t sz, uint32 hint) { return realloc(p, sz); }

struct MemoryBlock
{
	size_t sz;
	uint32 hint;
	void *origPtr;
	const char *codeline;
	LLLink inAllocList;
};
LinkList allocations;
size_t totalMemoryAllocated;

// We align managed memory blocks on a 16 byte boundary

#define ALIGN16(x) ((x) + 0xF & ~0xF)
void*
malloc_managed(size_t sz, uint32 hint)
{
	void *origPtr;
	uint8 *data;
	MemoryBlock *mem;

	if(sz == 0) return nil;
	origPtr = malloc(sz + sizeof(MemoryBlock) + 15);
	if(origPtr == nil)
		return nil;
	totalMemoryAllocated += sz;
	data = (uint8*)origPtr;
	data += sizeof(MemoryBlock);
	data = (uint8*)ALIGN16((uintptr)data);
	mem = (MemoryBlock*)(data-sizeof(MemoryBlock));

	mem->sz = sz;
	mem->hint = hint;
	mem->origPtr = origPtr;
	mem->codeline = allocLocation;
	allocations.add(&mem->inAllocList);

	return data;
}

void*
realloc_managed(void *p, size_t sz, uint32 hint)
{
	void *origPtr;
	MemoryBlock *mem;
	uint32 offset;

	if(p == nil)
		return malloc_managed(sz, hint);

	mem = (MemoryBlock*)((uint8*)p-sizeof(MemoryBlock));
	offset = (uint8*)p - (uint8*)mem->origPtr;

	mem->inAllocList.remove();

	origPtr = realloc(mem->origPtr, sz + sizeof(MemoryBlock) + 15);
	if(origPtr == nil){
		allocations.add(&mem->inAllocList);
		return nil;
	}
	p = (uint8*)origPtr + offset;
	mem = (MemoryBlock*)((uint8*)p-sizeof(MemoryBlock));
	totalMemoryAllocated -= mem->sz;
	mem->sz = sz;
	mem->hint = hint;
	mem->origPtr = origPtr;
	mem->codeline = allocLocation;
	allocations.add(&mem->inAllocList);
	totalMemoryAllocated += mem->sz;

	return p;
}

void
free_managed(void *p)
{
	MemoryBlock *mem;
	if(p == nil)
		return;
	mem = (MemoryBlock*)((uint8*)p-sizeof(MemoryBlock));
	mem->inAllocList.remove();
	totalMemoryAllocated -= mem->sz;
	free(mem->origPtr);
}

void
printleaks(void)
{
	FORLIST(lnk, allocations){
		MemoryBlock *mem = LLLinkGetData(lnk, MemoryBlock, inAllocList);
		printf("sz %zu hint %X\n   %s\n", mem->sz, mem->hint, mem->codeline);
	}
}

// TODO: make the debug out configurable
void *mustmalloc_h(size_t sz, uint32 hint)
{
	void *ret;
	ret = Engine::memfuncs.rwmalloc(sz, hint);
	if(ret || sz == 0)
		return ret;
	fprintf(stderr, "Error: out of memory\n");
	exit(1);
	return nil;
}
void *mustrealloc_h(void *p, size_t sz, uint32 hint)
{
	void *ret;
	ret = Engine::memfuncs.rwrealloc(p, sz, hint);
	if(ret || sz == 0)
		return ret;
	fprintf(stderr, "Error: out of memory\n");
	exit(1);
	return nil;
}

char *strdup_LOC(const char *s, uint32 hint, const char *here) {
	char *t;
	size_t sz = strlen(s)+1;
	t = (char*)malloc_LOC(sz, hint, here);
	if(t)
		memcpy(t, s, sz);
	return t;
}

MemoryFunctions defaultMemfuncs = {
	malloc_h,
	realloc_h,
	free,
	nil,
	nil
};

MemoryFunctions managedMemfuncs = {
	malloc_managed,
	realloc_managed,
	free_managed,
	nil,
	nil
};

// This function mainly registers engine plugins
bool32
Engine::init(MemoryFunctions *memfuncs)
{
	if(engine || Engine::state != Dead){
		RWERROR((ERR_ENGINEINIT));
		return 0;
	}

	totalMemoryAllocated = 0;
	allocations.init();

	if(memfuncs)
		Engine::memfuncs = *memfuncs;
	else
		Engine::memfuncs = defaultMemfuncs;

	if(Engine::memfuncs.rwmustmalloc == nil)
		Engine::memfuncs.rwmustmalloc = mustmalloc_h;
	if(Engine::memfuncs.rwmustrealloc == nil)
		Engine::memfuncs.rwmustrealloc = mustrealloc_h;

	PluginList::open();

	for(uint i = 0; i < NUM_PLATFORMS; i++)
		new (&Driver::s_plglist[i]) PluginList(sizeof(Driver));

	// core plugin attach here
	Frame::registerModule();
	Image::registerModule();
	Raster::registerModule();
	Texture::registerModule();

	// TODO: reset all allocation counts here. or maybe do that in modules?
	Frame::numAllocated = 0;
	Image::numAllocated = 0;
	Raster::numAllocated = 0;
	Texture::numAllocated = 0;
	TexDictionary::numAllocated = 0;
	Geometry::numAllocated = 0;
	Material::numAllocated = 0;
	Atomic::numAllocated = 0;
	Light::numAllocated = 0;
	Camera::numAllocated = 0;
	Clump::numAllocated = 0;
	World::numAllocated = 0;

	// driver plugin attach
	ps2::registerPlatformPlugins();
	xbox::registerPlatformPlugins();
	d3d8::registerPlatformPlugins();
	d3d9::registerPlatformPlugins();
	wdgl::registerPlatformPlugins();
	gl3::registerPlatformPlugins();

	Engine::state = Initialized;
	return 1;
}

// This is where RW allocates the engine and e.g. opens d3d
bool32
Engine::open(EngineOpenParams *p)
{
	if(engine || Engine::state != Initialized){
		RWERROR((ERR_ENGINEOPEN));
		return 0;
	}

	// Allocate engine
	engine = (Engine*)rwNew(Engine::s_plglist.size, MEMDUR_GLOBAL);
	engine->currentCamera = nil;
	engine->currentWorld = nil;

	// Initialize device
	// Device and possibly OS specific!
#ifdef RW_PS2
	engine->device = ps2::renderdevice;
#elif RW_GL3
	engine->device = gl3::renderdevice;
#elif RW_D3D9
	engine->device = d3d::renderdevice;
#else
	engine->device = null::renderdevice;
#endif

	engine->device.system(DEVICEOPEN, (void*)p, 0);

	engine->dummyDefaultPipeline = ObjPipeline::create();
	for(uint i = 0; i < NUM_PLATFORMS; i++){
		rw::engine->driver[i] = (Driver*)rwNew(Driver::s_plglist[i].size,
			MEMDUR_GLOBAL);

		engine->driver[i]->defaultPipeline = engine->dummyDefaultPipeline;

		engine->driver[i]->rasterCreate = null::rasterCreate;
		engine->driver[i]->rasterLock = null::rasterLock;
		engine->driver[i]->rasterUnlock = null::rasterUnlock;
		engine->driver[i]->rasterLockPalette = null::rasterLockPalette;
		engine->driver[i]->rasterUnlockPalette = null::rasterUnlockPalette;
		engine->driver[i]->rasterNumLevels = null::rasterNumLevels;
		engine->driver[i]->imageFindRasterFormat = null::imageFindRasterFormat;
		engine->driver[i]->rasterFromImage = null::rasterFromImage;
		engine->driver[i]->rasterToImage = null::rasterToImage;
	}

	Engine::state = Opened;
	return 1;
}

// This is where RW creates the actual rendering device
// and calls the engine plugin ctors
bool32
Engine::start(void)
{
	if(engine == nil || Engine::state != Opened){
		RWERROR((ERR_ENGINESTART));
		return 0;
	}

	engine->device.system(DEVICEINIT, nil, 0);

	Engine::s_plglist.construct(engine);
	for(uint i = 0; i < NUM_PLATFORMS; i++)
		Driver::s_plglist[i].construct(rw::engine->driver[i]);

	engine->device.system(DEVICEFINALIZE, nil, 0);

	// Register some image formats. Or should we leave that to the user?
	Image::registerFileFormat("tga", readTGA, writeTGA);
	Image::registerFileFormat("bmp", readBMP, writeBMP);
	Image::registerFileFormat("png", readPNG, writePNG);

	Engine::state = Started;
	return 1;
}

void
Engine::term(void)
{
	if(engine || Engine::state != Initialized){
		RWERROR((ERR_GENERAL));
		return;
	}

	PluginList::close();

	// This has to be reset because it won't be opened again otherwise
	// TODO: maybe reset more stuff here?
	d3d::nativeRasterOffset = 0;

	Engine::state = Dead;
}

void
Engine::close(void)
{
	if(engine == nil || Engine::state != Opened){
		RWERROR((ERR_GENERAL));
		return;
	}

	engine->device.system(DEVICECLOSE, nil, 0);
	for(uint i = 0; i < NUM_PLATFORMS; i++)
		rwFree(rw::engine->driver[i]);
	engine->dummyDefaultPipeline->destroy();
	rwFree(engine);
	engine = nil;
	Engine::state = Initialized;
}

void
Engine::stop(void)
{
	if(engine == nil || Engine::state != Started){
		RWERROR((ERR_GENERAL));
		return;
	}

	for(uint i = 0; i < NUM_PLATFORMS; i++)
		Driver::s_plglist[i].destruct(rw::engine->driver[i]);
	Engine::s_plglist.destruct(engine);

	engine->device.system(DEVICETERM, nil, 0);

	Engine::state = Opened;
}


int32
Engine::getNumSubSystems(void)
{
	return engine->device.system(DEVICEGETNUMSUBSYSTEMS, nil, 0);
}

int32
Engine::getCurrentSubSystem(void)
{
	return engine->device.system(DEVICEGETCURRENTSUBSYSTEM, nil, 0);
}

bool32
Engine::setSubSystem(int32 subsys)
{
	return engine->device.system(DEVICESETSUBSYSTEM, nil, subsys);
}

SubSystemInfo*
Engine::getSubSystemInfo(SubSystemInfo *info, int32 subsys)
{
	if(engine->device.system(DEVICEGETSUBSSYSTEMINFO, info, subsys))
		return info;
	return nil;
}


int32
Engine::getNumVideoModes(void)
{
	return engine->device.system(DEVICEGETNUMVIDEOMODES, nil, 0);
}

int32
Engine::getCurrentVideoMode(void)
{
	return engine->device.system(DEVICEGETCURRENTVIDEOMODE, nil, 0);
}

bool32
Engine::setVideoMode(int32 mode)
{
	return engine->device.system(DEVICESETVIDEOMODE, nil, mode);
}

VideoMode*
Engine::getVideoModeInfo(VideoMode *info, int32 mode)
{
	if(engine->device.system(DEVICEGETVIDEOMODEINFO, info, mode))
		return info;
	return nil;
}


uint32
Engine::getMaxMultiSamplingLevels(void)
{
	return engine->device.system(DEVICEGETMAXMULTISAMPLINGLEVELS, nil, 0);
}

uint32
Engine::getMultiSamplingLevels(void)
{
	return engine->device.system(DEVICEGETMULTISAMPLINGLEVELS, nil, 0);
}

bool32
Engine::setMultiSamplingLevels(uint32 levels)
{
	return engine->device.system(DEVICESETMULTISAMPLINGLEVELS, nil, levels);
}


namespace null {

void beginUpdate(Camera*) { }
void endUpdate(Camera*) { }
void clearCamera(Camera*,RGBA*,uint32) { }
void showRaster(Raster*,uint32) { }

void   setRenderState(int32, void*) { }
void  *getRenderState(int32) { return 0; }

bool32 rasterRenderFast(Raster *raster, int32 x, int32 y) { return 0; }

void im2DRenderLine(void*, int32, int32, int32) { }
void im2DRenderTriangle(void*, int32, int32, int32, int32) { }
void im2DRenderPrimitive(PrimitiveType, void*, int32) { }
void im2DRenderIndexedPrimitive(PrimitiveType, void*, int32, void*, int32) { }

void im3DTransform(void *vertices, int32 numVertices, Matrix *world, uint32 flags) { }
void im3DRenderPrimitive(PrimitiveType primType) { }
void im3DRenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices) { }
void im3DEnd(void) { }

Raster*
rasterCreate(Raster*)
{
	assert(0 && "rasterCreate not implemented");
	return nil;
}

uint8*
rasterLock(Raster*, int32, int32)
{
	assert(0 && "lockRaster not implemented");
	return nil;
}

void
rasterUnlock(Raster*, int32)
{
	assert(0 && "unlockRaster not implemented");
}

uint8*
rasterLockPalette(Raster*, int32)
{
	assert(0 && "rasterLockPalette not implemented");
	return nil;
}

void
rasterUnlockPalette(Raster*)
{
	assert(0 && "rasterUnlockPalette not implemented");
}

int32
rasterNumLevels(Raster*)
{
	assert(0 && "rasterNumLevels not implemented");
	return 0;
}

bool32
imageFindRasterFormat(Image *img, int32 type,
	int32 *width, int32 *height, int32 *depth, int32 *format)
{
	assert(0 && "imageFindRasterFormat not implemented");
	return 0;
}

bool32
rasterFromImage(Raster*, Image*)
{
	assert(0 && "rasterFromImage not implemented");
	return 0;
}

Image*
rasterToImage(Raster*)
{
	assert(0 && "rasterToImage not implemented");
	return nil;
}

int
deviceSystem(DeviceReq req, void *arg0, int32 n)
{
	switch(req){
	case DEVICEGETNUMSUBSYSTEMS:
		return 0;
	case DEVICEGETCURRENTSUBSYSTEM:
		return 0;
	case DEVICEGETSUBSSYSTEMINFO:
		return 0;
	default: break;
	}
	return 1;
}

Device renderdevice = {
	0.0f, 1.0f,
	null::beginUpdate,
	null::endUpdate,
	null::clearCamera,
	null::showRaster,
	null::rasterRenderFast,
	null::setRenderState,
	null::getRenderState,
	null::im2DRenderLine,
	null::im2DRenderTriangle,
	null::im2DRenderPrimitive,
	null::im2DRenderIndexedPrimitive,
	null::im3DTransform,
	null::im3DRenderPrimitive,
	null::im3DRenderIndexedPrimitive,
	null::im3DEnd,
	null::deviceSystem
};

}
}

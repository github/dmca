namespace rw {

// uhhhm..... why are these not actual functions?
enum DeviceReq
{
	// Device initialization before Engine/Driver plugins are opened
	DEVICEINIT,
	// Device de-initialization after Engine/Driver plugins are closed
	DEVICETERM,

	// Device/Context creation
	DEVICEOPEN,
	// Device/Context shutdown
	DEVICECLOSE,

	// Device initialization after Engine/Driver plugins are opened
	DEVICEFINALIZE,
	// TODO? counterpart to FINALIZE?

	// Video adapters
	DEVICEGETNUMSUBSYSTEMS,
	DEVICEGETCURRENTSUBSYSTEM,
	DEVICESETSUBSYSTEM,
	DEVICEGETSUBSSYSTEMINFO,

	// Video modes
	DEVICEGETNUMVIDEOMODES,
	DEVICEGETCURRENTVIDEOMODE,
	DEVICESETVIDEOMODE,
	DEVICEGETVIDEOMODEINFO,

	// Multisampling
	DEVICEGETMAXMULTISAMPLINGLEVELS,
	DEVICEGETMULTISAMPLINGLEVELS,
	DEVICESETMULTISAMPLINGLEVELS,

};

typedef int DeviceSystem(DeviceReq req, void *arg, int32 n);

struct Camera;
struct Image;
struct Texture;
struct Raster;
class ObjPipeline;

// This is for the render device, we only have one
struct Device
{
	float32 zNear, zFar;
	void  (*beginUpdate)(Camera*);
	void  (*endUpdate)(Camera*);
	void  (*clearCamera)(Camera*, RGBA *col, uint32 mode);
	void  (*showRaster)(Raster *raster, uint32 flags);
	bool32 (*rasterRenderFast)(Raster *raster, int32 x, int32 y);
	void  (*setRenderState)(int32 state, void *value);
	void *(*getRenderState)(int32 state);

	void   (*im2DRenderLine)(void *vertices, int32 numVertices, int32 vert1, int32 vert2);
	void   (*im2DRenderTriangle)(void *vertices, int32 numVertices, int32 vert1, int32 vert2, int32 vert3);
	void   (*im2DRenderPrimitive)(PrimitiveType primType, void *vertices, int32 numVertices);
	void   (*im2DRenderIndexedPrimitive)(PrimitiveType primType, void *vertices, int32 numVertices, void *indices, int32 numIndices);

	void (*im3DTransform)(void *vertices, int32 numVertices, Matrix *world, uint32 flags);
	void (*im3DRenderPrimitive)(PrimitiveType primType);
	void (*im3DRenderIndexedPrimitive)(PrimitiveType primType, void *indices, int32 numIndices);
	void (*im3DEnd)(void);

	DeviceSystem *system;
};

// This is for platform-dependent but portable things
// so the engine has one for every platform
struct Driver
{
	ObjPipeline *defaultPipeline;
	int32 rasterNativeOffset;

	Raster* (*rasterCreate)(Raster*);
	uint8 *(*rasterLock)(Raster*, int32 level, int32 lockMode);
	void   (*rasterUnlock)(Raster*, int32 level);
	uint8 *(*rasterLockPalette)(Raster*, int32 lockMode);
	void   (*rasterUnlockPalette)(Raster*);
	int32  (*rasterNumLevels)(Raster*);
	bool32 (*imageFindRasterFormat)(Image *img, int32 type,
		int32 *width, int32 *height, int32 *depth, int32 *format);
	bool32 (*rasterFromImage)(Raster*, Image*);
	Image *(*rasterToImage)(Raster*);

	static PluginList s_plglist[NUM_PLATFORMS];
	static int32 registerPlugin(int32 platform, int32 size, uint32 id,
	                            Constructor ctor, Destructor dtor){
		return s_plglist[platform].registerPlugin(size, id,
		                                          ctor, dtor, nil);
	}
};

struct EngineOpenParams;

enum MemHint
{
	MEMDUR_NA = 0,
	// used inside function
	MEMDUR_FUNCTION = 0x10000,
	// used for one frame
	MEMDUR_FRAME = 0x20000,
	// used for longer time
	MEMDUR_EVENT = 0x30000,
	// used while the engine is running
	MEMDUR_GLOBAL = 0x40000
};

struct MemoryFunctions
{
	void *(*rwmalloc)(size_t sz, uint32 hint);
	void *(*rwrealloc)(void *p, size_t sz, uint32 hint);
	void  (*rwfree)(void *p);
	// These are temporary until we have better error handling
	// TODO: Maybe don't put them here since they shouldn't really be switched out
	void *(*rwmustmalloc)(size_t sz, uint32 hint);
	void *(*rwmustrealloc)(void *p, size_t sz, uint32 hint);
};

struct SubSystemInfo
{
	char name[80];
};

enum VideoModeFlags
{
	VIDEOMODEEXCLUSIVE = 1
};

struct VideoMode
{
	int32 width;
	int32 height;
	int32 depth;
	uint32 flags;
};

struct Camera;
struct World;

// This is for platform independent things
// TODO: move more stuff into this
struct Engine
{
	enum State {
		Dead = 0,
		Initialized,
		Opened,
		Started
	};
	Camera *currentCamera;
	World *currentWorld;
	LinkList frameDirtyList;

	// Dynamically allocated because of plugins
	Driver *driver[NUM_PLATFORMS];
	Device device;
	ObjPipeline *dummyDefaultPipeline;

	// These must always be available
	static MemoryFunctions memfuncs;
	static State state;

	static bool32 init(MemoryFunctions *memfuncs = nil);
	static bool32 open(EngineOpenParams*);
	static bool32 start(void);
	static void term(void);
	static void close(void);
	static void stop(void);

	static int32 getNumSubSystems(void);
	static int32 getCurrentSubSystem(void);
	static bool32 setSubSystem(int32 subsys);
	static SubSystemInfo *getSubSystemInfo(SubSystemInfo *info, int32 subsys);

	static int32 getNumVideoModes(void);
	static int32 getCurrentVideoMode(void);
	static bool32 setVideoMode(int32 mode);
	static VideoMode *getVideoModeInfo(VideoMode *info, int32 mode);

	static uint32 getMaxMultiSamplingLevels(void);
	static uint32 getMultiSamplingLevels(void);
	static bool32 setMultiSamplingLevels(uint32 levels);

	static PluginList s_plglist;
	static int32 registerPlugin(int32 size, uint32 id,
	                            Constructor ctor, Destructor dtor){
		return s_plglist.registerPlugin(size, id, ctor, dtor, nil);
	}
};

extern Engine *engine;

#define RWTOSTR_(X) #X
#define RWTOSTR(X) RWTOSTR_(X)
#define RWHERE "file: " __FILE__ " line: " RWTOSTR(__LINE__)

extern const char *allocLocation;

inline void *malloc_LOC(size_t sz, uint32 hint, const char *here) { allocLocation = here; return rw::Engine::memfuncs.rwmalloc(sz,hint); }
inline void *realloc_LOC(void *p, size_t sz, uint32 hint, const char *here) { allocLocation = here; return rw::Engine::memfuncs.rwrealloc(p,sz,hint); }
inline void *mustmalloc_LOC(size_t sz, uint32 hint, const char *here) { allocLocation = here; return rw::Engine::memfuncs.rwmustmalloc(sz,hint); }
inline void *mustrealloc_LOC(void *p, size_t sz, uint32 hint, const char *here) { allocLocation = here; return rw::Engine::memfuncs.rwmustrealloc(p,sz,hint); }

char *strdup_LOC(const char *s, uint32 hint, const char *here);

#define rwMalloc(s, h) rw::malloc_LOC(s,h,RWHERE)
#define rwMallocT(t, s, h) (t*)rw::malloc_LOC((s)*sizeof(t),h,RWHERE)
#define rwRealloc(p, s, h) rw::realloc_LOC(p,s,h,RWHERE)
#define rwReallocT(t, p, s, h) (t*)rw::realloc_LOC(p,(s)*sizeof(t),h,RWHERE)
#define rwFree(p) rw::Engine::memfuncs.rwfree(p)
#define rwNew(s, h) rw::mustmalloc_LOC(s,h,RWHERE)
#define rwNewT(t, s, h) (t*)rw::mustmalloc_LOC((s)*sizeof(t),h,RWHERE)
#define rwResize(p, s, h) rw::mustrealloc_LOC(p,s,h,RWHERE)
#define rwResizeT(t, p, s, h) (t*)rw::mustrealloc_LOC(p,(s)*sizeof(t),h,RWHERE)
#define rwStrdup(s, h) rw::strdup_LOC(s,h,RWHERE)

extern MemoryFunctions defaultMemfuncs;
extern MemoryFunctions managedMemfuncs;
void printleaks(void);	// when using managed mem funcs

namespace null {
	void beginUpdate(Camera*);
	void endUpdate(Camera*);
	void clearCamera(Camera*, RGBA *col, uint32 mode);
	void showRaster(Raster*, uint32 flags);

	void   setRenderState(int32 state, void *value);
	void  *getRenderState(int32 state);

	bool32 rasterRenderFast(Raster *raster, int32 x, int32 y);

	Raster *rasterCreate(Raster*);
	uint8 *rasterLock(Raster*, int32 level, int32 lockMode);
	void   rasterUnlock(Raster*, int32 level);
	uint8 *rasterLockPalette(Raster*, int32 lockMode);
	void   rasterUnlockPalette(Raster*);
	int32  rasterNumLevels(Raster*);
	bool32 imageFindRasterFormat(Image *img, int32 type,
		int32 *width, int32 *height, int32 *depth, int32 *format);
	bool32 rasterFromImage(Raster*, Image*);
	Image *rasterToImage(Raster*);

	void im2DRenderLine(void*, int32, int32, int32);
	void im2DRenderTriangle(void*, int32, int32, int32, int32);
	void im2DRenderPrimitive(PrimitiveType, void*, int32);
	void im2DRenderIndexedPrimitive(PrimitiveType, void*, int32, void*, int32);

	void im3DTransform(void *vertices, int32 numVertices, Matrix *world, uint32 flags);
	void im3DRenderPrimitive(PrimitiveType primType);
	void im3DRenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices);
	void im3DEnd(void);

	int deviceSystem(DeviceReq req, void *arg0, int32 n);

	extern Device renderdevice;
}

}

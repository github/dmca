#include <stddef.h>

namespace rw {

struct Object
{
	uint8 type;
	uint8 subType;
	uint8 flags;
	uint8 privateFlags;
	void *parent;

	void init(uint8 type, uint8 subType){
		this->type = type;
		this->subType = subType;
		this->flags = 0;
		this->privateFlags = 0;
		this->parent = nil;
	}
	void copy(Object *o){
		this->type = o->type;
		this->subType = o->subType;
		this->flags = o->flags;
		this->privateFlags = o->privateFlags;
		this->parent = nil;
	}
};

struct Frame
{
	PLUGINBASE
	typedef Frame *(*Callback)(Frame *f, void *data);
	enum { ID = 0 };
	enum {		// private flags
		// The hierarchy has unsynched frames
		HIERARCHYSYNCLTM = 0x01,	// LTM not synched
		HIERARCHYSYNCOBJ = 0x02,	// attached objects not synched
		HIERARCHYSYNC    = HIERARCHYSYNCLTM  | HIERARCHYSYNCOBJ,
		// This frame is not synched
		SUBTREESYNCLTM   = 0x04,
		SUBTREESYNCOBJ   = 0x08,
		SUBTREESYNC      = SUBTREESYNCLTM | SUBTREESYNCOBJ,
		SYNCLTM          = HIERARCHYSYNCLTM | SUBTREESYNCLTM,
		SYNCOBJ          = HIERARCHYSYNCOBJ | SUBTREESYNCOBJ
		// STATIC = 0x10
	};

	Object object;
	LLLink inDirtyList;
	LinkList objectList;
	Matrix matrix;
	Matrix ltm;

	Frame *child;
	Frame *next;
	Frame *root;

	static int32 numAllocated;

	static Frame *create(void);
	Frame *cloneHierarchy(void);
	void destroy(void);
	void destroyHierarchy(void);
	Frame *addChild(Frame *f, bool32 append = 0);
	Frame *removeChild(void);
	Frame *forAllChildren(Callback cb, void *data);
	Frame *getParent(void) const {
		return (Frame*)this->object.parent; }
	int32 count(void);
	bool32 dirty(void) const {
		return !!(this->root->object.privateFlags & HIERARCHYSYNC); }
	Matrix *getLTM(void);
	void rotate(const V3d *axis, float32 angle, CombineOp op);
	void translate(const V3d *trans, CombineOp op);
	void scale(const V3d *scale, CombineOp op);
	void transform(const Matrix *mat, CombineOp op);
	void updateObjects(void);


	void syncHierarchyLTM(void);
	void setHierarchyRoot(Frame *root);
	Frame *cloneAndLink(void);
	void purgeClone(void);

#ifndef RWPUBLIC
	static void registerModule(void);
#endif
	static void syncDirty(void);
};

struct FrameList_
{
	int32 numFrames;
	Frame **frames;

	FrameList_ *streamRead(Stream *stream);
	void streamWrite(Stream *stream);
	static uint32 streamGetSize(Frame *f);
};
Frame **makeFrameList(Frame *frame, Frame **flist);

struct ObjectWithFrame
{
	typedef void (*Sync)(ObjectWithFrame*);

	Object object;
	LLLink inFrame;
	Sync syncCB;

	void setFrame(Frame *f){
		if(this->object.parent)
			this->inFrame.remove();
		this->object.parent = f;
		if(f){
			f->objectList.add(&this->inFrame);
			f->updateObjects();
		}
	}
	void sync(void){ this->syncCB(this); }
	static ObjectWithFrame *fromFrame(LLLink *lnk){
		return LLLinkGetData(lnk, ObjectWithFrame, inFrame);
	}
};

struct Image
{
	int32 flags;
	int32 width, height;
	int32 depth;
	int32 bpp;	// bytes per pixel
	int32 stride;
	uint8 *pixels;
	uint8 *palette;

	static int32 numAllocated;

	static Image *create(int32 width, int32 height, int32 depth);
	void destroy(void);
	void allocate(void);
	void free(void);
	void setPixels(uint8 *pixels);
	void setPixelsDXT(int32 type, uint8 *pixels);
	void setPalette(uint8 *palette);
	void compressPalette(void);	// turn 8 bit into 4 bit if possible
	bool32 hasAlpha(void);
	void convertTo32(void);
	void palettize(int32 depth);
	void unpalettize(bool forceAlpha = false);
	void makeMask(void);
	void applyMask(Image *mask);
	void removeMask(void);
	Image *extractMask(void);

	static void setSearchPath(const char*);
	static void printSearchPath(void);
	static char *getFilename(const char*);
	static Image *read(const char *imageName);
	static Image *readMasked(const char *imageName, const char *maskName);


	typedef Image *(*fileRead)(const char *afilename);
	typedef void (*fileWrite)(Image *image, const char *filename);
	static bool32 registerFileFormat(const char *ext, fileRead read, fileWrite write);

#ifndef RWPUBLIC
	static void registerModule(void);
#endif
};

Image *readTGA(const char *filename);
void writeTGA(Image *image, const char *filename);
Image *readBMP(const char *filename);
void writeBMP(Image *image, const char *filename);
Image *readPNG(const char *filename);
void writePNG(Image *image, const char *filename);

enum { QUANTDEPTH = 8 };

struct ColorQuant
{
	struct Node {
		uint32 r, g, b, a;
		int32 numPixels;
		Node *parent;
		Node *children[16];
		LLLink link;

		void destroy(void);
		void addColor(RGBA color);
		bool isLeaf(void) { for(int32 i = 0; i < 16; i++) if(this->children[i]) return false; return true; }
	};

	Node *root;
	LinkList leaves;

	void init(void);
	void destroy(void);
	Node *createNode(int32 level);
	Node *getNode(Node *root, uint32 addr, int32 level);
	Node *findNode(Node *root, uint32 addr, int32 level);
	void reduceNode(Node *node);
	void addColor(RGBA color);
	uint8 findColor(RGBA color);
	void addImage(Image *img);
	void makePalette(int32 numColors, RGBA *colors);
	void matchImage(uint8 *dstPixels, uint32 dstStride, Image *src);
};

// used to emulate d3d and xbox textures
struct RasterLevels
{
	int32 numlevels;
	uint32 format;
	struct Level {
		int32 width, height, size;
		uint8 *data;
	} levels[1];	// 0 is illegal :/
};

struct Raster
{
	enum { FLIPWAITVSYNCH = 1 };

	PLUGINBASE
	int32 platform;

	// TODO: use bytes
	int32 type;
	int32 flags;
	int32 privateFlags;
	int32 format;
	int32 width, height, depth;
	int32 stride;
	uint8 *pixels;
	uint8 *palette;
	// remember for locked rasters
	uint8 *originalPixels;
	int32 originalWidth;
	int32 originalHeight;
	int32 originalStride;
	// subraster
	Raster *parent;
	int32 offsetX, offsetY;

	static int32 numAllocated;

	static Raster *create(int32 width, int32 height, int32 depth,
	                      int32 format, int32 platform = 0);
	void subRaster(Raster *parent, Rect *r);
	void destroy(void);
	static bool32 imageFindRasterFormat(Image *image, int32 type,
		int32 *pWidth, int32 *pHeight, int32 *pDepth, int32 *pFormat, int32 platform = 0);
	Raster *setFromImage(Image *image, int32 platform = 0);
	static Raster *createFromImage(Image *image, int32 platform = 0);
	Image *toImage(void);
	uint8 *lock(int32 level, int32 lockMode);
	void unlock(int32 level);
	uint8 *lockPalette(int32 lockMode);
	void unlockPalette(void);
	int32 getNumLevels(void);
	static int32 calculateNumLevels(int32 width, int32 height);
	static bool formatHasAlpha(int32 format);

	void show(uint32 flags);

	static Raster *pushContext(Raster *raster);
	static Raster *popContext(void);
	static Raster *getCurrentContext(void);
	bool32 renderFast(int32 x, int32 y);

	static Raster *convertTexToCurrentPlatform(Raster *ras);
#ifndef RWPUBLIC
	static void registerModule(void);
#endif

	enum Format {
		DEFAULT    = 0,
		C1555      = 0x0100,
		C565       = 0x0200,
		C4444      = 0x0300,
		LUM8       = 0x0400,
		C8888      = 0x0500,
		C888       = 0x0600,
		D16        = 0x0700,
		D24        = 0x0800,
		D32        = 0x0900,
		C555       = 0x0A00,
		AUTOMIPMAP = 0x1000,
		PAL8       = 0x2000,
		PAL4       = 0x4000,
		MIPMAP     = 0x8000
	};
	enum Type {
		NORMAL        = 0x00,
		ZBUFFER       = 0x01,
		CAMERA        = 0x02,
		TEXTURE       = 0x04,
		CAMERATEXTURE = 0x05,
		DONTALLOCATE  = 0x80
	};
	enum LockMode {
		LOCKWRITE	= 1,
		LOCKREAD	= 2,
		LOCKNOFETCH	= 4,	// don't fetch pixel data
		LOCKRAW		= 8,
	};

	enum
	{
		// from RW
		PRIVATELOCK_READ		= 0x02,
		PRIVATELOCK_WRITE		= 0x04,
		PRIVATELOCK_READ_PALETTE	= 0x08,
		PRIVATELOCK_WRITE_PALETTE	= 0x10,
	};
};

void conv_RGBA8888_from_RGBA8888(uint8 *out, uint8 *in);
void conv_BGRA8888_from_RGBA8888(uint8 *out, uint8 *in);
void conv_RGBA8888_from_RGB888(uint8 *out, uint8 *in);
void conv_BGRA8888_from_RGB888(uint8 *out, uint8 *in);
void conv_RGB888_from_RGB888(uint8 *out, uint8 *in);
void conv_BGR888_from_RGB888(uint8 *out, uint8 *in);
void conv_ARGB1555_from_ARGB1555(uint8 *out, uint8 *in);
void conv_ARGB1555_from_RGB555(uint8 *out, uint8 *in);
void conv_RGBA5551_from_ARGB1555(uint8 *out, uint8 *in);
void conv_ARGB1555_from_RGBA5551(uint8 *out, uint8 *in);
void conv_RGBA8888_from_ARGB1555(uint8 *out, uint8 *in);
void conv_ABGR1555_from_ARGB1555(uint8 *out, uint8 *in);
inline void conv_8_from_8(uint8 *out, uint8 *in) { *out = *in; }
// some swaps are the same, so these are just more descriptive names
inline void conv_RGBA8888_from_BGRA8888(uint8 *out, uint8 *in) { conv_BGRA8888_from_RGBA8888(out, in); }
inline void conv_RGB888_from_BGR888(uint8 *out, uint8 *in) { conv_BGR888_from_RGB888(out, in); }
inline void conv_ARGB1555_from_ABGR1555(uint8 *out, uint8 *in) { conv_ABGR1555_from_ARGB1555(out, in); }

void expandPal4(uint8 *dst, uint32 dststride, uint8 *src, uint32 srcstride, int32 w, int32 h);
void compressPal4(uint8 *dst, uint32 dststride, uint8 *src, uint32 srcstride, int32 w, int32 h);
void expandPal4_BE(uint8 *dst, uint32 dststride, uint8 *src, uint32 srcstride, int32 w, int32 h);
void compressPal4_BE(uint8 *dst, uint32 dststride, uint8 *src, uint32 srcstride, int32 w, int32 h);
void copyPal8(uint8 *dst, uint32 dststride, uint8 *src, uint32 srcstride, int32 w, int32 h);

void flipDXT(int32 type, uint8 *dst, uint8 *src, uint32 width, uint32 height);


#define IGNORERASTERIMP 0

struct TexDictionary;

struct Texture
{
	enum FilterMode {
		NEAREST = 1,
		LINEAR,
		MIPNEAREST,	// one mipmap
		MIPLINEAR,
		LINEARMIPNEAREST,	// mipmap interpolated
		LINEARMIPLINEAR
	};
	enum Addressing {
		WRAP = 1,
		MIRROR,
		CLAMP,
		BORDER
	};

	PLUGINBASE
	Raster *raster;
	TexDictionary *dict;
	LLLink inDict;
	char name[32];
	char mask[32];
	uint32 filterAddressing; // VVVVUUUU FFFFFFFF
	int32 refCount;

	LLLink inGlobalList;	// actually not in RW

	static int32 numAllocated;

	static Texture *create(Raster *raster);
	void addRef(void) { this->refCount++; }
	void destroy(void);
	static Texture *fromDict(LLLink *lnk){
		return LLLinkGetData(lnk, Texture, inDict); }
	FilterMode getFilter(void) { return (FilterMode)(filterAddressing & 0xFF); }
	void setFilter(FilterMode f) { filterAddressing = (filterAddressing & ~0xFF) | f; }
	Addressing getAddressU(void) { return (Addressing)((filterAddressing >> 8) & 0xF); }
	Addressing getAddressV(void) { return (Addressing)((filterAddressing >> 12) & 0xF); }
	void setAddressU(Addressing u) { filterAddressing = (filterAddressing & ~0xF00) | u<<8; }
	void setAddressV(Addressing v) { filterAddressing = (filterAddressing & ~0xF000) | v<<12; }
	static Texture *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
	static Texture *read(const char *name, const char *mask);
	static Texture *streamReadNative(Stream *stream);
	void streamWriteNative(Stream *stream);
	uint32 streamGetSizeNative(void);

	static Texture *(*findCB)(const char *name);
	static Texture *(*readCB)(const char *name, const char *mask);
	static void setLoadTextures(bool32);	// default: true
	static void setCreateDummies(bool32);	// default: false
	static void setMipmapping(bool32);	// default: false
	static void setAutoMipmapping(bool32);	// default: false
	static bool32 getMipmapping(void);
	static bool32 getAutoMipmapping(void);

	void setMaxAnisotropy(int32 maxaniso);	// only if plugin is attached
	int32 getMaxAnisotropy(void);

#ifndef RWPUBLIC
	static void registerModule(void);
#endif
};

extern int32 anisotOffset;
#define GETANISOTROPYEXT(texture) PLUGINOFFSET(int32, texture, rw::anisotOffset)
void registerAnisotropyPlugin(void);
int32 getMaxSupportedMaxAnisotropy(void);

struct SurfaceProperties
{
	float32 ambient;
	float32 specular;
	float32 diffuse;
};

struct Material
{
	PLUGINBASE
	Texture *texture;
	RGBA color;
	SurfaceProperties surfaceProps;
	Pipeline *pipeline;
	int32 refCount;

	static int32 numAllocated;

	static Material *create(void);
	void addRef(void) { this->refCount++; }
	Material *clone(void);
	void destroy(void);
	void setTexture(Texture *tex);
	static Material *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
};

void registerMaterialRightsPlugin(void);

struct Mesh
{
	uint16 *indices;
	uint32 numIndices;
	Material *material;
};

struct MeshHeader
{
	enum {
		TRISTRIP = 1
	};
	uint32 flags;
	uint16 numMeshes;
	uint16 serialNum;
	uint32 totalIndices;
	uint32 pad;	// needed for alignment of Meshes
	// after this the meshes

	Mesh *getMeshes(void) { return (Mesh*)(this+1); }
	void setupIndices(void);
	uint32 guessNumTriangles(void);
};

struct Geometry;

struct MorphTarget
{
	Geometry *parent;
	Sphere boundingSphere;
	V3d *vertices;
	V3d *normals;

	Sphere calculateBoundingSphere(void) const;
};

struct InstanceDataHeader
{
	uint32 platform;
};

struct Triangle
{
	uint16 v[3];
	uint16 matId;
};

struct MaterialList
{
	Material **materials;
	int32 numMaterials;
	int32 space;

	void init(void);
	void deinit(void);
	int32 appendMaterial(Material *mat);
	int32 findIndex(Material *mat);
	static MaterialList *streamRead(Stream *stream, MaterialList *matlist);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
};

struct Geometry
{
	PLUGINBASE
	enum { ID = 8 };
	Object object;
	uint32 flags;
	uint16 lockedSinceInst;
	int32 numTriangles;
	int32 numVertices;
	int32 numMorphTargets;
	int32 numTexCoordSets;

	Triangle *triangles;
	RGBA *colors;
	TexCoords *texCoords[8];

	MorphTarget *morphTargets;
	MaterialList matList;

	MeshHeader *meshHeader;
	InstanceDataHeader *instData;

	int32 refCount;

	static int32 numAllocated;

	static Geometry *create(int32 numVerts, int32 numTris, uint32 flags);
	void addRef(void) { this->refCount++; }
	void destroy(void);
	void lock(int32 lockFlags);
	void unlock(void);
	void addMorphTargets(int32 n);
	void calculateBoundingSphere(void);
	bool32 hasColoredMaterial(void);
	void allocateData(void);
	MeshHeader *allocateMeshes(int32 numMeshes, uint32 numIndices, bool32 noIndices);
	void generateTriangles(int8 *adc = nil);
	void buildMeshes(void);
	void buildTristrips(void);	// private, used by buildMeshes
	void correctTristripWinding(void);
	void removeUnusedMaterials(void);
	static Geometry *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);

	enum Flags
	{
		TRISTRIP  = 0x01,
		POSITIONS = 0x02,
		TEXTURED  = 0x04,
		PRELIT    = 0x08,
		NORMALS   = 0x10,
		LIGHT     = 0x20,
		MODULATE  = 0x40,
		TEXTURED2 = 0x80,
		// When this flag is set the geometry has
		// native geometry. When streamed out this geometry
		// is written out instead of the platform independent data.
		// When streamed in with this flag, the geometry is mostly empty.
		NATIVE         = 0x01000000,
		// Just for documentation: RW sets this flag
		// to prevent rendering when executing a pipeline,
		// so only instancing will occur.
		// librw's pipelines are different so it's unused here.
		NATIVEINSTANCE = 0x02000000
	};

	enum LockFlags
	{
		LOCKPOLYGONS     = 0x0001,
		LOCKVERTICES     = 0x0002,
		LOCKNORMALS      = 0x0004,
		LOCKPRELIGHT     = 0x0008,

		LOCKTEXCOORDS    = 0x0010,
		LOCKTEXCOORDS1   = 0x0010,
		LOCKTEXCOORDS2   = 0x0020,
		LOCKTEXCOORDS3   = 0x0040,
		LOCKTEXCOORDS4   = 0x0080,
		LOCKTEXCOORDS5   = 0x0100,
		LOCKTEXCOORDS6   = 0x0200,
		LOCKTEXCOORDS7   = 0x0400,
		LOCKTEXCOORDS8   = 0x0800,
		LOCKTEXCOORDSALL = 0x0ff0,

		LOCKALL          = 0x0fff
	};
};

void registerMeshPlugin(void);
void registerNativeDataPlugin(void);

struct Clump;
struct World;

struct Atomic
{
	PLUGINBASE
	typedef void (*RenderCB)(Atomic *atomic);
	enum { ID = 1 };
	enum {
	// flags
		COLLISIONTEST = 0x01,	// unused here
		RENDER = 0x04,
	// private flags
		WORLDBOUNDDIRTY = 0x01,
	// for setGeometry
		SAMEBOUNDINGSPHERE = 0x01
	};

	ObjectWithFrame object;
	Geometry *geometry;
	Sphere boundingSphere;
	Sphere worldBoundingSphere;
	Clump *clump;
	LLLink inClump;
	ObjPipeline *pipeline;
	RenderCB renderCB;

	World *world;
	ObjectWithFrame::Sync originalSync;

	static int32 numAllocated;

	static Atomic *create(void);
	Atomic *clone(void);
	void destroy(void);
	void setFrame(Frame *f) {
		this->object.setFrame(f);
		this->object.object.privateFlags |= WORLDBOUNDDIRTY;
	}
	Frame *getFrame(void) const { return (Frame*)this->object.object.parent; }
	static Atomic *fromClump(LLLink *lnk){
		return LLLinkGetData(lnk, Atomic, inClump); }
	void setGeometry(Geometry *geo, uint32 flags);
	Sphere *getWorldBoundingSphere(void);
	ObjPipeline *getPipeline(void);
	void instance(void);
	void uninstance(void);
	void render(void) { this->renderCB(this); }
	void setRenderCB(RenderCB renderCB){
		this->renderCB = renderCB;
		if(this->renderCB == nil)
			this->renderCB = defaultRenderCB;
	};
	void setFlags(uint32 flags) { this->object.object.flags = flags; }
	uint32 getFlags(void) const { return this->object.object.flags; }
	static Atomic *streamReadClump(Stream *stream,
		FrameList_ *frameList, Geometry **geometryList);
	bool streamWriteClump(Stream *stream, FrameList_ *frmlst);
	uint32 streamGetSize(void);

	static void defaultRenderCB(Atomic *atomic);
};

void registerAtomicRightsPlugin(void);

struct Light
{
	PLUGINBASE
	enum { ID = 3 };
	ObjectWithFrame object;
	float32 radius;
	RGBAf color;
	float32 minusCosAngle;
	LLLink inWorld;

	// clump extension
	Clump *clump;
	LLLink inClump;

	// world extension
	World *world;
	ObjectWithFrame::Sync originalSync;

	static int32 numAllocated;

	static Light *create(int32 type);
	void destroy(void);
	void setFrame(Frame *f) { this->object.setFrame(f); }
	Frame *getFrame(void) const { return (Frame*)this->object.object.parent; }
	static Light *fromClump(LLLink *lnk){
		return LLLinkGetData(lnk, Light, inClump); }
	static Light *fromWorld(LLLink *lnk){
		return LLLinkGetData(lnk, Light, inWorld); }
	void setAngle(float32 angle);
	float32 getAngle(void);
	void setColor(float32 r, float32 g, float32 b);
	int32 getType(void){ return this->object.object.subType; }
	void setFlags(uint32 flags) { this->object.object.flags = flags; }
	uint32 getFlags(void) { return this->object.object.flags; }
	static Light *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);

	enum Type {
		DIRECTIONAL = 1,
		AMBIENT,
		POINT = 0x80,	// positioned
		SPOT,
		SOFTSPOT
	};
	enum Flags {
		LIGHTATOMICS = 1,
		LIGHTWORLD = 2
	};
};

struct FrustumPlane
{
	Plane plane;
	/* Used for BBox tests:
	 * 0 = inf is closer to normal direction
	 * 1 = sup is closer to normal direction */
	uint8 closestX;
	uint8 closestY;
	uint8 closestZ;
};

struct Camera
{
	PLUGINBASE
	enum { ID = 4 };
	enum { PERSPECTIVE = 1, PARALLEL };
	enum { CLEARIMAGE = 0x1, CLEARZ = 0x2, CLEARSTENCIL = 0x4 };
	// return value of frustumTestSphere
	enum { SPHEREOUTSIDE, SPHEREBOUNDARY, SPHEREINSIDE };

	ObjectWithFrame object;
	void (*beginUpdateCB)(Camera*);
	void (*endUpdateCB)(Camera*);
	V2d viewWindow;
	V2d viewOffset;
	float32 nearPlane, farPlane;
	float32 fogPlane;
	int32 projection;

	Matrix viewMatrix;
	float32 zScale, zShift;

	FrustumPlane frustumPlanes[6];
	V3d frustumCorners[8];
	BBox frustumBoundBox;

	Raster *frameBuffer;
	Raster *zBuffer;

	// Device dependent view and projection matrices
	// optional
	RawMatrix devView;
	RawMatrix devProj;

	// clump link handled by plugin in RW
	Clump *clump;
	LLLink inClump;

	// world extension
	/* RW: frustum sectors, space, position */
	World *world;
	ObjectWithFrame::Sync originalSync;
	void (*originalBeginUpdate)(Camera*);
	void (*originalEndUpdate)(Camera*);

	static int32 numAllocated;

	static Camera *create(void);
	Camera *clone(void);
	void destroy(void);
	void setFrame(Frame *f) { this->object.setFrame(f); }
	Frame *getFrame(void)const { return (Frame*)this->object.object.parent; }
	static Camera *fromClump(LLLink *lnk){
		return LLLinkGetData(lnk, Camera, inClump); }
	void beginUpdate(void) { this->beginUpdateCB(this); }
	void endUpdate(void) { this->endUpdateCB(this); }
	void clear(RGBA *col, uint32 mode);
	void showRaster(uint32 flags);
	void setNearPlane(float32);
	void setFarPlane(float32);
	void setViewWindow(const V2d *window);
	void setViewOffset(const V2d *offset);
	void setProjection(int32 proj);
	int32 frustumTestSphere(const Sphere *s) const;
	static Camera *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);

	// fov in degrees
	void setFOV(float32 fov, float32 ratio);
};

struct Clump
{
	PLUGINBASE
	enum { ID = 2 };
	Object object;
	LinkList atomics;
	LinkList lights;
	LinkList cameras;

	World *world;
	LLLink inWorld;

	static int32 numAllocated;

	static Clump *create(void);
	Clump *clone(void);
	void destroy(void);
	static Clump *fromWorld(LLLink *lnk){
		return LLLinkGetData(lnk, Clump, inWorld); }
	int32 countAtomics(void) { return this->atomics.count(); }
	void addAtomic(Atomic *a);
	void removeAtomic(Atomic *a);
	int32 countLights(void) { return this->lights.count(); }
	void addLight(Light *l);
	void removeLight(Light *l);
	int32 countCameras(void) { return this->cameras.count(); }
	void addCamera(Camera *c);
	void removeCamera(Camera *c);
	void setFrame(Frame *f){
		this->object.parent = f; }
	Frame *getFrame(void) const {
		return (Frame*)this->object.parent; }
	static Clump *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
	void render(void);
};

// used by enumerateLights for lighting callback
struct WorldLights
{
	int32 numAmbients;
	RGBAf ambient;	// all ambients added
	int32 numDirectionals;
	Light **directionals;	// only directionals
	int32 numLocals;
	Light **locals;	// points, (soft)spots
};

// A bit of a stub right now
struct World
{
	PLUGINBASE
	enum { ID = 7 };
	Object object;
	LinkList localLights;	// these have positions (type >= 0x80)
	LinkList globalLights;	// these do not (type < 0x80)
	LinkList clumps;

	static int32 numAllocated;

	static World *create(void);
	void destroy(void);
	void addLight(Light *light);
	void removeLight(Light *light);
	void addCamera(Camera *cam);
	void removeCamera(Camera *cam);
	void addAtomic(Atomic *atomic);
	void removeAtomic(Atomic *atomic);
	void addClump(Clump *clump);
	void removeClump(Clump *clump);
	void render(void);
	void enumerateLights(Atomic *atomic, WorldLights *lightData);
};

struct TexDictionary
{
	PLUGINBASE
	enum { ID = 6 };
	Object object;
	LinkList textures;
	LLLink inGlobalList;

	static int32 numAllocated;

	static TexDictionary *create(void);
	static TexDictionary *fromLink(LLLink *lnk){
		return LLLinkGetData(lnk, TexDictionary, inGlobalList); }
	void destroy(void);
	int32 count(void) { return this->textures.count(); }
	void add(Texture *t);
	void addFront(Texture *t);
	void remove(Texture *t);
	Texture *find(const char *name);
	static TexDictionary *streamRead(Stream *stream);
	void streamWrite(Stream *stream);
	uint32 streamGetSize(void);

	static void setCurrent(TexDictionary *txd);
	static TexDictionary *getCurrent(void);
};

}

#ifdef RW_GL3
#include "glad/glad.h"
#ifdef LIBRW_SDL2
#include <SDL.h>
#else
#include <GLFW/glfw3.h>
#endif
#endif

namespace rw {

#ifdef RW_GL3
struct EngineOpenParams
{
#ifdef LIBRW_SDL2
	SDL_Window **window;
	bool32 fullscreen;
#else
	GLFWwindow **window;
#endif
	int width, height;
	const char *windowtitle;
};
#endif

namespace gl3 {

void registerPlatformPlugins(void);

extern Device renderdevice;

// arguments to glVertexAttribPointer basically
struct AttribDesc
{
	uint32 index;
	int32  type;
	bool32 normalized;
	int32  size;
	uint32 stride;
	uint32 offset;
};

enum AttribIndices
{
	ATTRIB_POS = 0,
	ATTRIB_NORMAL,
	ATTRIB_COLOR,
	ATTRIB_WEIGHTS,
	ATTRIB_INDICES,
	ATTRIB_TEXCOORDS0,
	ATTRIB_TEXCOORDS1,
	ATTRIB_TEXCOORDS2,
	ATTRIB_TEXCOORDS3,
	ATTRIB_TEXCOORDS4,
	ATTRIB_TEXCOORDS5,
	ATTRIB_TEXCOORDS6,
	ATTRIB_TEXCOORDS7,
};

// default uniform indices
extern int32 u_matColor;
extern int32 u_surfProps;

struct InstanceData
{
	uint32    numIndex;
	uint32    minVert;	// not used for rendering
	int32     numVertices;	//
	Material *material;
	bool32    vertexAlpha;
	uint32    program;
	uint32    offset;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	uint32      serialNumber;
	uint32      numMeshes;
	uint16     *indexBuffer;
	uint32      primType;
	uint8      *vertexBuffer;
	int32       numAttribs;
	AttribDesc *attribDesc;
	uint32      totalNumIndex;
	uint32      totalNumVertex;

	uint32      ibo;
	uint32      vbo;		// or 2?
#ifdef RW_GL_USE_VAOS
	uint32      vao;
#endif

	InstanceData *inst;
};

#ifdef RW_GL3

struct Shader;

extern Shader *defaultShader;

struct Im3DVertex
{
	V3d     position;
	uint8   r, g, b, a;
	float32 u, v;

	void setX(float32 x) { this->position.x = x; }
	void setY(float32 y) { this->position.y = y; }
	void setZ(float32 z) { this->position.z = z; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) {
		this->r = r; this->g = g; this->b = b; this->a = a; }
	void setU(float32 u) { this->u = u; }
	void setV(float32 v) { this->v = v; }

	float getX(void) { return this->position.x; }
	float getY(void) { return this->position.y; }
	float getZ(void) { return this->position.z; }
	RGBA getColor(void) { return makeRGBA(this->r, this->g, this->b, this->a); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};

struct Im2DVertex
{
	float32 x, y, z, w;
	uint8   r, g, b, a;
	float32 u, v;

	void setScreenX(float32 x) { this->x = x; }
	void setScreenY(float32 y) { this->y = y; }
	void setScreenZ(float32 z) { this->z = z; }
	// This is a bit unefficient but we have to counteract GL's divide, so multiply
	void setCameraZ(float32 z) { this->w = z; }
	void setRecipCameraZ(float32 recipz) { this->w = 1.0f/recipz; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) {
		this->r = r; this->g = g; this->b = b; this->a = a; }
	void setU(float32 u, float recipz) { this->u = u; }
	void setV(float32 v, float recipz) { this->v = v; }

	float getScreenX(void) { return this->x; }
	float getScreenY(void) { return this->y; }
	float getScreenZ(void) { return this->z; }
	float getCameraZ(void) { return this->w; }
	float getRecipCameraZ(void) { return 1.0f/this->w; }
	RGBA getColor(void) { return makeRGBA(this->r, this->g, this->b, this->a); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};

void setAttribPointers(AttribDesc *attribDescs, int32 numAttribs);
void disableAttribPointers(AttribDesc *attribDescs, int32 numAttribs);

// Render state

// Vertex shader bits
enum
{
	// These should be low so they could be used as indices
	VSLIGHT_DIRECT	= 1,
	VSLIGHT_POINT	= 2,
	VSLIGHT_SPOT	= 4,
	VSLIGHT_MASK	= 7,	// all the above
	// less critical
	VSLIGHT_AMBIENT = 8,
};

extern const char *shaderDecl;	// #version stuff
extern const char *header_vert_src;
extern const char *header_frag_src;

extern Shader *im2dOverrideShader;

// per Scene
void setProjectionMatrix(float32*);
void setViewMatrix(float32*);

// per Object
void setWorldMatrix(Matrix*);
int32 setLights(WorldLights *lightData);

// per Mesh
void setTexture(int32 n, Texture *tex);
void setMaterial(const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f);
inline void setMaterial(uint32 flags, const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f)
{
	static RGBA white = { 255, 255, 255, 255 };
	if(flags & Geometry::MODULATE)
		setMaterial(color, surfaceprops, extraSurfProp);
	else
		setMaterial(white, surfaceprops, extraSurfProp);
}

void setAlphaBlend(bool32 enable);
bool32 getAlphaBlend(void);

void bindFramebuffer(uint32 fbo);
uint32 bindTexture(uint32 texid);

void flushCache(void);

#endif

class ObjPipeline : public rw::ObjPipeline
{
public:
	void init(void);
	static ObjPipeline *create(void);

	void (*instanceCB)(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
	void (*uninstanceCB)(Geometry *geo, InstanceDataHeader *header);
	void (*renderCB)(Atomic *atomic, InstanceDataHeader *header);
};

void defaultInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
void defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header);
void defaultRenderCB(Atomic *atomic, InstanceDataHeader *header);
int32 lightingCB(Atomic *atomic);

void drawInst_simple(InstanceDataHeader *header, InstanceData *inst);
// Emulate PS2 GS alpha test FB_ONLY case: failed alpha writes to frame- but not to depth buffer
void drawInst_GSemu(InstanceDataHeader *header, InstanceData *inst);
// This one switches between the above two depending on render state;
void drawInst(InstanceDataHeader *header, InstanceData *inst);


void *destroyNativeData(void *object, int32, int32);

ObjPipeline *makeDefaultPipeline(void);

// Native Texture and Raster

struct Gl3Raster
{
	// arguments to glTexImage2D
	int32 internalFormat;
	int32 type;
	int32 format;
	int32 bpp;	// bytes per pixel
	// texture object
	uint32 texid;

	bool isCompressed;
	bool hasAlpha;
	bool autogenMipmap;
	int8 numLevels;
	// cached filtermode and addressing
	uint8 filterMode;
	uint8 addressU;
	uint8 addressV;
	int32 maxAnisotropy;

	uint32 fbo;		// used for camera texture only!
	Raster *fboMate;	// color or zbuffer raster mate of this one
	RasterLevels *backingStore;	// if we can't read back GPU memory but have to
};

struct Gl3Caps
{
	int gles;
	int glversion;
	bool dxtSupported;
	bool astcSupported;	// not used yet
	float maxAnisotropy;
};
extern Gl3Caps gl3Caps;
// GLES can't read back textures very nicely.
// In most cases that's not an issue, but when it is,
// this has to be set before the texture is filled:
extern bool32 needToReadBackTextures;

void allocateDXT(Raster *raster, int32 dxt, int32 numLevels, bool32 hasAlpha);

Texture *readNativeTexture(Stream *stream);
void writeNativeTexture(Texture *tex, Stream *stream);
uint32 getSizeNativeTexture(Texture *tex);

extern int32 nativeRasterOffset;
void registerNativeRaster(void);
#define GETGL3RASTEREXT(raster) PLUGINOFFSET(Gl3Raster, raster, rw::gl3::nativeRasterOffset)

}
}

#ifdef RW_D3D9
#ifdef WITH_D3D
#include <d3d9.h>
#endif
#endif

namespace rw {

#ifdef RW_D3D9

#ifdef _WINDOWS_
struct EngineOpenParams
{
	HWND window;
};
#else
struct EngineOpenParams
{
	uint32 please_include_windows_h;
};
#endif
#else
#ifdef _D3D9_H_
#error "please don't include d3d9.h for non-d3d9 platforms"
#endif
#endif

namespace d3d {

extern bool32 isP8supported;

extern Device renderdevice;

#ifdef RW_D3D9
#ifdef _D3D9_H_
extern IDirect3DDevice9 *d3ddevice;
void setD3dMaterial(D3DMATERIAL9 *mat9);
#endif

#define COLOR_ARGB(a, r, g, b) ((rw::uint32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

struct Im3DVertex
{
	V3d position;
	uint32 color;
	float32 u, v;

	void setX(float32 x) { this->position.x = x; }
	void setY(float32 y) { this->position.y = y; }
	void setZ(float32 z) { this->position.z = z; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) { this->color = COLOR_ARGB(a, r, g, b); }
	void setU(float32 u) { this->u = u; }
	void setV(float32 v) { this->v = v; }

	float getX(void) { return this->position.x; }
	float getY(void) { return this->position.y; }
	float getZ(void) { return this->position.z; }
	RGBA getColor(void) { return makeRGBA(this->color>>16 & 0xFF, this->color>>8 & 0xFF,
		this->color & 0xFF, this->color>>24 & 0xFF); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};

struct Im2DVertex
{
	float32 x, y, z;
	//float32 q;	// recipz no longer used because we have a vertex stage now
	float32 w;
	uint32 color;
	float32 u, v;

	void setScreenX(float32 x) { this->x = x; }
	void setScreenY(float32 y) { this->y = y; }
	void setScreenZ(float32 z) { this->z = z; }
	void setCameraZ(float32 z) { this->w = z; }
//	void setRecipCameraZ(float32 recipz) { this->q = recipz; }
	void setRecipCameraZ(float32 recipz) { this->w = 1.0f/recipz; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) { this->color = COLOR_ARGB(a, r, g, b); }
	void setU(float32 u, float recipZ) { this->u = u; }
	void setV(float32 v, float recipZ) { this->v = v; }

	float getScreenX(void) { return this->x; }
	float getScreenY(void) { return this->y; }
	float getScreenZ(void) { return this->z; }
//	float getCameraZ(void) { return 1.0f/this->q; }
//	float getRecipCameraZ(void) { return this->q; }
	float getCameraZ(void) { return this->w; }
	float getRecipCameraZ(void) { return 1.0f/this->w; }
	RGBA getColor(void) { return makeRGBA(this->color>>16 & 0xFF, this->color>>8 & 0xFF,
		this->color & 0xFF, this->color>>24 & 0xFF); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};

#else
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
            ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) |       \
            ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))
#endif
enum {
	D3DFMT_UNKNOWN              =  0,

	D3DFMT_R8G8B8               = 20,
	D3DFMT_A8R8G8B8             = 21,
	D3DFMT_X8R8G8B8             = 22,
	D3DFMT_R5G6B5               = 23,
	D3DFMT_X1R5G5B5             = 24,
	D3DFMT_A1R5G5B5             = 25,
	D3DFMT_A4R4G4B4             = 26,
	D3DFMT_R3G3B2               = 27,
	D3DFMT_A8                   = 28,
	D3DFMT_A8R3G3B2             = 29,
	D3DFMT_X4R4G4B4             = 30,
	D3DFMT_A2B10G10R10          = 31,
	D3DFMT_A8B8G8R8             = 32,
	D3DFMT_X8B8G8R8             = 33,
	D3DFMT_G16R16               = 34,
	D3DFMT_A2R10G10B10          = 35,
	D3DFMT_A16B16G16R16         = 36,

	D3DFMT_A8P8                 = 40,
	D3DFMT_P8                   = 41,

	D3DFMT_L8                   = 50,
	D3DFMT_A8L8                 = 51,
	D3DFMT_A4L4                 = 52,

	D3DFMT_V8U8                 = 60,
	D3DFMT_L6V5U5               = 61,
	D3DFMT_X8L8V8U8             = 62,
	D3DFMT_Q8W8V8U8             = 63,
	D3DFMT_V16U16               = 64,
	D3DFMT_A2W10V10U10          = 67,

	D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
	D3DFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
	D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
	D3DFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
	D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
	D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
	D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
	D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
	D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

	D3DFMT_D16_LOCKABLE         = 70,
	D3DFMT_D32                  = 71,
	D3DFMT_D15S1                = 73,
	D3DFMT_D24S8                = 75,
	D3DFMT_D24X8                = 77,
	D3DFMT_D24X4S4              = 79,
	D3DFMT_D16                  = 80,

	D3DFMT_D32F_LOCKABLE        = 82,
	D3DFMT_D24FS8               = 83,

	// d3d9ex only
	/* Z-Stencil formats valid for CPU access */
	D3DFMT_D32_LOCKABLE         = 84,
	D3DFMT_S8_LOCKABLE          = 85,

	D3DFMT_L16                  = 81,

	D3DFMT_VERTEXDATA           =100,
	D3DFMT_INDEX16              =101,
	D3DFMT_INDEX32              =102,

	D3DFMT_Q16W16V16U16         =110,

	D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

	// Floating point surface formats

	// s10e5 formats (16-bits per channel)
	D3DFMT_R16F                 = 111,
	D3DFMT_G16R16F              = 112,
	D3DFMT_A16B16G16R16F        = 113,

	// IEEE s23e8 formats (32-bits per channel)
	D3DFMT_R32F                 = 114,
	D3DFMT_G32R32F              = 115,
	D3DFMT_A32B32G32R32F        = 116,

	D3DFMT_CxV8U8               = 117,

	// d3d9ex only
	// Monochrome 1 bit per pixel format
	D3DFMT_A1                   = 118,
	// 2.8 biased fixed point
	D3DFMT_A2B10G10R10_XR_BIAS  = 119,
	// Binary format indicating that the data has no inherent type
	D3DFMT_BINARYBUFFER         = 199
};

enum {
	D3DLOCK_NOSYSLOCK     =  0,  // ignored
	D3DPOOL_MANAGED       =  0,  // ignored
	D3DPT_TRIANGLELIST    =  4,
	D3DPT_TRIANGLESTRIP   =  5,


	D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
	D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
	D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
	D3DDECLTYPE_FLOAT4    =  3,  // 4D float
	D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
	                             // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
	D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
	D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

	D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
	D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
	D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
	D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
	D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
	D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
	D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
	D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
	D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
	D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.


	D3DDECLMETHOD_DEFAULT =  0,


	D3DDECLUSAGE_POSITION = 0,
	D3DDECLUSAGE_BLENDWEIGHT,   // 1
	D3DDECLUSAGE_BLENDINDICES,  // 2
	D3DDECLUSAGE_NORMAL,        // 3
	D3DDECLUSAGE_PSIZE,         // 4
	D3DDECLUSAGE_TEXCOORD,      // 5
	D3DDECLUSAGE_TANGENT,       // 6
	D3DDECLUSAGE_BINORMAL,      // 7
	D3DDECLUSAGE_TESSFACTOR,    // 8
	D3DDECLUSAGE_POSITIONT,     // 9
	D3DDECLUSAGE_COLOR,         // 10
	D3DDECLUSAGE_FOG,           // 11
	D3DDECLUSAGE_DEPTH,         // 12
	D3DDECLUSAGE_SAMPLE         // 13
	,

	D3DUSAGE_AUTOGENMIPMAP = 0x400
};
#endif

extern int vertFormatMap[];

void *createIndexBuffer(uint32 length, bool dynamic);
void destroyIndexBuffer(void *indexBuffer);
uint16 *lockIndices(void *indexBuffer, uint32 offset, uint32 size, uint32 flags);
void unlockIndices(void *indexBuffer);

void *createVertexBuffer(uint32 length, uint32 fvf, bool dynamic);
void destroyVertexBuffer(void *vertexBuffer);
uint8 *lockVertices(void *vertexBuffer, uint32 offset, uint32 size, uint32 flags);
void unlockVertices(void *vertexBuffer);

void *createTexture(int32 width, int32 height, int32 levels, uint32 usage, uint32 format);
void destroyTexture(void *texture);
uint8 *lockTexture(void *texture, int32 level);
void unlockTexture(void *texture, int32 level);

// Native Texture and Raster

struct D3dRaster
{
	void *texture;
	void *palette;
	void *lockedSurf;
	uint32 format;
	uint32 bpp;	// bytes per pixel
	bool hasAlpha;
	bool customFormat;
	bool autogenMipmap;
};

int32 getLevelSize(Raster *raster, int32 level);
void allocateDXT(Raster *raster, int32 dxt, int32 numLevels, bool32 hasAlpha);
void setPalette(Raster *raster, void *palette, int32 size);
void setTexels(Raster *raster, void *texels, int32 level);

extern int32 nativeRasterOffset;
void registerNativeRaster(void);
#define GETD3DRASTEREXT(raster) PLUGINOFFSET(rw::d3d::D3dRaster, raster, rw::d3d::nativeRasterOffset)

// Rendering

void setRenderState(uint32 state, uint32 value);
void getRenderState(uint32 state, uint32 *value);
void setTextureStageState(uint32 stage, uint32 type, uint32 value);
void getTextureStageState(uint32 stage, uint32 type, uint32 *value);
void setSamplerState(uint32 stage, uint32 type, uint32 value);
void getSamplerState(uint32 stage, uint32 type, uint32 *value);
void flushCache(void);

void setTexture(uint32 stage, Texture *tex);
void setMaterial(const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f);
inline void setMaterial(uint32 flags, const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f)
{
	static RGBA white = { 255, 255, 255, 255 };
	if(flags & Geometry::MODULATE)
		setMaterial(color, surfaceprops, extraSurfProp);
	else
		setMaterial(white, surfaceprops, extraSurfProp);
}

void setVertexShader(void *vs);
void setPixelShader(void *ps);
void setIndices(void *indexBuffer);
void setStreamSource(int n, void *buffer, uint32 offset, uint32 stride);
void setVertexDeclaration(void *declaration);

void *createVertexShader(void *csosrc);
void *createPixelShader(void *csosrc);
void destroyVertexShader(void *shader);
void destroyPixelShader(void *shader);


/*
 * Vertex shaders and common pipeline stuff
 */

// This data will be available in vertex stream 2
struct VertexConstantData
{
	V3d normal;
	RGBA color;
	TexCoords texCoors[8];
};
extern void *constantVertexStream;

// TODO: figure out why this even still exists...
struct D3dShaderState
{
	// for VS
	struct {
		float32 start;
		float32 end;
		float32 range;	// 1/(start-end)
		float32 disable;	// lower clamp
	} fogData, fogDisable;
	RGBA matColor;
	SurfaceProperties surfProps;
	float extraSurfProp;
	float lightOffset[3];
	int32 numDir, numPoint, numSpot;
	RGBAf ambient;
	// for PS
	RGBAf fogColor;

	bool fogDirty;
};
extern D3dShaderState d3dShaderState;

// Standard Vertex shader locations
enum
{
	VSLOC_combined	= 0,
	VSLOC_world	= 4,
	VSLOC_normal	= 8,
	VSLOC_matColor	= 12,
	VSLOC_surfProps	= 13,
	VSLOC_fogData	= 14,
	VSLOC_ambLight	= 15,
	VSLOC_lightOffset	= 16,
	VSLOC_lights	= 17,
	VSLOC_afterLights	= VSLOC_lights + 8*3,

	VSLOC_numLights	= 0,

	PSLOC_fogColor = 0
};

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

void lightingCB_Fix(Atomic *atomic);
int32 lightingCB_Shader(Atomic *atomic);
// for VS
void uploadMatrices(void);	// no world transform
void uploadMatrices(Matrix *worldMat);
void setAmbient(const RGBAf &color);
void setNumLights(int numDir, int numPoint, int numSpot);
int32 uploadLights(WorldLights *lightData);	// called by lightingCB_Shader

extern void *im2dOverridePS;

extern void *default_amb_VS;
extern void *default_amb_dir_VS;
extern void *default_all_VS;
extern void *default_PS;
extern void *default_tex_PS;
extern void *im2d_VS;
extern void *im2d_PS;
extern void *im2d_tex_PS;
void createDefaultShaders(void);
void destroyDefaultShaders(void);


}
}

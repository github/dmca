namespace rw {
namespace xbox {

void registerPlatformPlugins(void);

extern int v3dFormatMap[6];
extern int v2dFormatMap[6];

struct InstanceData
{
	uint32 minVert;
	int32 numVertices;
	int32 numIndices;
	void *indexBuffer;
	Material *material;
	uint32 vertexShader;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	int32 size;
	uint16 serialNumber;
	uint16 numMeshes;
	uint32 primType;
	int32 numVertices;
	int32 stride;
	void *vertexBuffer;
	bool32 vertexAlpha;
	InstanceData *begin;
	InstanceData *end;

	uint8 *data;
};

void *destroyNativeData(void *object, int32, int32);
Stream *readNativeData(Stream *stream, int32 len, void *object, int32, int32);
Stream *writeNativeData(Stream *stream, int32 len, void *object, int32, int32);
int32 getSizeNativeData(void *object, int32, int32);
void registerNativeDataPlugin(void);

class ObjPipeline : public rw::ObjPipeline
{
public:
	void init(void);
	static ObjPipeline *create(void);

	void (*instanceCB)(Geometry *geo, InstanceDataHeader *header);
	void (*uninstanceCB)(Geometry *geo, InstanceDataHeader *header);
};

ObjPipeline *makeDefaultPipeline(void);

void defaultInstanceCB(Geometry *geo, InstanceDataHeader *header);
void defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header);

// Skin plugin

Stream *readNativeSkin(Stream *stream, int32, void *object, int32 offset);
Stream *writeNativeSkin(Stream *stream, int32 len, void *object, int32 offset);
int32 getSizeNativeSkin(void *object, int32 offset);

void initSkin(void);
ObjPipeline *makeSkinPipeline(void);

// MatFX plugin

void initMatFX(void);
ObjPipeline *makeMatFXPipeline(void);

// Vertex Format plugin

extern uint32 vertexFormatSizes[6];

uint32 *getVertexFmt(Geometry *g);
uint32 makeVertexFmt(int32 flags, uint32 numTexSets);
uint32 getVertexFmtStride(uint32 fmt);

void registerVertexFormatPlugin(void);

// Native Texture and Raster

struct XboxRaster
{
	void *texture;
	void *palette;
	uint32 format;
	uint32 bpp;	// bytes per pixel
	bool hasAlpha;
	bool customFormat;
	bool32 unknownFlag;
};

int32 getLevelSize(Raster *raster, int32 level);

extern int32 nativeRasterOffset;
void registerNativeRaster(void);
#define GETXBOXRASTEREXT(raster) PLUGINOFFSET(rw::xbox::XboxRaster, raster, rw::xbox::nativeRasterOffset)

Texture *readNativeTexture(Stream *stream);
void writeNativeTexture(Texture *tex, Stream *stream);
uint32 getSizeNativeTexture(Texture *tex);

enum {
	D3DFMT_UNKNOWN              = 0xFFFFFFFF,

	/* Swizzled formats */

	D3DFMT_A8R8G8B8             = 0x00000006,
	D3DFMT_X8R8G8B8             = 0x00000007,
	D3DFMT_R5G6B5               = 0x00000005,
	D3DFMT_R6G5B5               = 0x00000027,
	D3DFMT_X1R5G5B5             = 0x00000003,
	D3DFMT_A1R5G5B5             = 0x00000002,
	D3DFMT_A4R4G4B4             = 0x00000004,
	D3DFMT_A8                   = 0x00000019,
	D3DFMT_A8B8G8R8             = 0x0000003A,
	D3DFMT_B8G8R8A8             = 0x0000003B,
	D3DFMT_R4G4B4A4             = 0x00000039,
	D3DFMT_R5G5B5A1             = 0x00000038,
	D3DFMT_R8G8B8A8             = 0x0000003C,
	D3DFMT_R8B8                 = 0x00000029,
	D3DFMT_G8B8                 = 0x00000028,

	D3DFMT_P8                   = 0x0000000B,

	D3DFMT_L8                   = 0x00000000,
	D3DFMT_A8L8                 = 0x0000001A,
	D3DFMT_AL8                  = 0x00000001,
	D3DFMT_L16                  = 0x00000032,

	D3DFMT_V8U8                 = 0x00000028,
	D3DFMT_L6V5U5               = 0x00000027,
	D3DFMT_X8L8V8U8             = 0x00000007,
	D3DFMT_Q8W8V8U8             = 0x0000003A,
	D3DFMT_V16U16               = 0x00000033,

	D3DFMT_D16_LOCKABLE         = 0x0000002C,
	D3DFMT_D16                  = 0x0000002C,
	D3DFMT_D24S8                = 0x0000002A,
	D3DFMT_F16                  = 0x0000002D,
	D3DFMT_F24S8                = 0x0000002B,

	/* YUV formats */

	D3DFMT_YUY2                 = 0x00000024,
	D3DFMT_UYVY                 = 0x00000025,

	/* Compressed formats */

	D3DFMT_DXT1                 = 0x0000000C,
	D3DFMT_DXT2                 = 0x0000000E,
	D3DFMT_DXT3                 = 0x0000000E,
	D3DFMT_DXT4                 = 0x0000000F,
	D3DFMT_DXT5                 = 0x0000000F,

	/* Linear formats */

	D3DFMT_LIN_A1R5G5B5         = 0x00000010,
	D3DFMT_LIN_A4R4G4B4         = 0x0000001D,
	D3DFMT_LIN_A8               = 0x0000001F,
	D3DFMT_LIN_A8B8G8R8         = 0x0000003F,
	D3DFMT_LIN_A8R8G8B8         = 0x00000012,
	D3DFMT_LIN_B8G8R8A8         = 0x00000040,
	D3DFMT_LIN_G8B8             = 0x00000017,
	D3DFMT_LIN_R4G4B4A4         = 0x0000003E,
	D3DFMT_LIN_R5G5B5A1         = 0x0000003D,
	D3DFMT_LIN_R5G6B5           = 0x00000011,
	D3DFMT_LIN_R6G5B5           = 0x00000037,
	D3DFMT_LIN_R8B8             = 0x00000016,
	D3DFMT_LIN_R8G8B8A8         = 0x00000041,
	D3DFMT_LIN_X1R5G5B5         = 0x0000001C,
	D3DFMT_LIN_X8R8G8B8         = 0x0000001E,

	D3DFMT_LIN_A8L8             = 0x00000020,
	D3DFMT_LIN_AL8              = 0x0000001B,
	D3DFMT_LIN_L16              = 0x00000035,
	D3DFMT_LIN_L8               = 0x00000013,

	D3DFMT_LIN_V16U16           = 0x00000036,
	D3DFMT_LIN_V8U8             = 0x00000017,
	D3DFMT_LIN_L6V5U5           = 0x00000037,
	D3DFMT_LIN_X8L8V8U8         = 0x0000001E,
	D3DFMT_LIN_Q8W8V8U8         = 0x00000012,

	D3DFMT_LIN_D24S8            = 0x0000002E,
	D3DFMT_LIN_F24S8            = 0x0000002F,
	D3DFMT_LIN_D16              = 0x00000030,
	D3DFMT_LIN_F16              = 0x00000031,

	D3DFMT_VERTEXDATA           = 100,
	D3DFMT_INDEX16              = 101
};

}
}

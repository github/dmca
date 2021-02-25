
namespace rw {
namespace wdgl {

// NOTE: This is not really RW OpenGL! It's specific to WarDrum's GTA ports

void registerPlatformPlugins(void);

struct AttribDesc
{
	// arguments to glVertexAttribPointer (should use OpenGL types here)
	// Vertex = 0, TexCoord, Normal, Color, Weight, Bone Index, Extra Color
	uint32 index;
	// float = 0, byte, ubyte, short, ushort
	int32 type;
	bool32 normalized;
	int32 size;
	uint32 stride;
	uint32 offset;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	int32 numAttribs;
	AttribDesc *attribs;
	uint32 dataSize;
	uint8 *data;

	// needed for rendering
	uint32 vbo;
	uint32 ibo;
};

// only RW_OPENGL
void uploadGeo(Geometry *geo);
void setAttribPointers(InstanceDataHeader *inst);

void packattrib(uint8 *dst, float32 *src, AttribDesc *a, float32 scale);
void unpackattrib(float *dst, uint8 *src, AttribDesc *a, float32 scale);

void *destroyNativeData(void *object, int32, int32);
Stream *readNativeData(Stream *stream, int32 len, void *object, int32, int32);
Stream *writeNativeData(Stream *stream, int32 len, void *object, int32, int32);
int32 getSizeNativeData(void *object, int32, int32);
void registerNativeDataPlugin(void);

void printPipeinfo(Atomic *a);

class ObjPipeline : public rw::ObjPipeline
{
public:
	void init(void);
	static ObjPipeline *create(void);

	uint32 numCustomAttribs;
	uint32 (*instanceCB)(Geometry *g, int32 i, uint32 offset);
	void (*uninstanceCB)(Geometry *g);
};

ObjPipeline *makeDefaultPipeline(void);

// Skin plugin

void initSkin(void);
Stream *readNativeSkin(Stream *stream, int32, void *object, int32 offset);
Stream *writeNativeSkin(Stream *stream, int32 len, void *object, int32 offset);
int32 getSizeNativeSkin(void *object, int32 offset);

ObjPipeline *makeSkinPipeline(void);

// MatFX plugin

void initMatFX(void);
ObjPipeline *makeMatFXPipeline(void);

// Raster

struct Texture : rw::Texture
{
	void upload(void);
	void bind(int n);
};

extern int32 nativeRasterOffset;
void registerNativeRaster(void);

}
}

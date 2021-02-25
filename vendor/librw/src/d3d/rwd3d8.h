namespace rw {
namespace d3d8 {

void registerPlatformPlugins(void);

struct InstanceData
{
	uint32    minVert;
	int32     stride;
	int32     numVertices;
	int32     numIndices;
	Material *material;
	uint32    vertexShader;
	uint32    primType;
	void     *indexBuffer;
	void     *vertexBuffer;
	uint32    baseIndex;
	uint8     vertexAlpha;
	uint8     managed;
	uint8     remapped;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	uint16  serialNumber;
	uint16  numMeshes;

	InstanceData *inst;
};

uint32 makeFVFDeclaration(uint32 flags, int32 numTex);
int32 getStride(uint32 flags, int32 numTex);

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

	void (*instanceCB)(Geometry *geo, InstanceData *header);
	void (*uninstanceCB)(Geometry *geo, InstanceData *header);
	void (*renderCB)(Atomic *atomic, InstanceDataHeader *header);
};

void defaultInstanceCB(Geometry *geo, InstanceData *header);
void defaultUninstanceCB(Geometry *geo, InstanceData *header);
void defaultRenderCB(Atomic *atomic, InstanceDataHeader *header);

ObjPipeline *makeDefaultPipeline(void);

// Skin plugin

void initSkin(void);
ObjPipeline *makeSkinPipeline(void);

// MatFX plugin

void initMatFX(void);
ObjPipeline *makeMatFXPipeline(void);

// Native Texture and Raster

Texture *readNativeTexture(Stream *stream);
void writeNativeTexture(Texture *tex, Stream *stream);
uint32 getSizeNativeTexture(Texture *tex);

}
}

namespace rw {
namespace d3d9 {

void registerPlatformPlugins(void);

struct VertexElement
{
	uint16   stream;
	uint16   offset;
	uint8    type;
	uint8    method;
	uint8    usage;
	uint8    usageIndex;
};

struct VertexStream
{
	void  *vertexBuffer;
	uint32 offset;
	uint32 stride;
	uint16 geometryFlags;
	uint8  managed;
	uint8  dynamicLock;
};

struct InstanceData
{
	uint32    numIndex;
	uint32    minVert;
	Material *material;
	bool32    vertexAlpha;
	void     *vertexShader;
	uint32    baseIndex;
	uint32    numVertices;
	uint32    startIndex;
	uint32    numPrimitives;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	uint32  serialNumber;
	uint32  numMeshes;
	void   *indexBuffer;
	uint32  primType;
	VertexStream vertexStream[2];
	bool32  useOffsets;
	void   *vertexDeclaration;
	uint32  totalNumIndex;
	uint32  totalNumVertex;

	InstanceData *inst;
};

void *createVertexDeclaration(VertexElement *elements);
void destroyVertexDeclaration(void *delaration);
uint32 getDeclaration(void *declaration, VertexElement *elements);

void drawInst_simple(d3d9::InstanceDataHeader *header, d3d9::InstanceData *inst);
// Emulate PS2 GS alpha test FB_ONLY case: failed alpha writes to frame- but not to depth buffer
void drawInst_GSemu(d3d9::InstanceDataHeader *header, InstanceData *inst);
// This one switches between the above two depending on render state;
void drawInst(d3d9::InstanceDataHeader *header, d3d9::InstanceData *inst);




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

	void (*instanceCB)(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
	void (*uninstanceCB)(Geometry *geo, InstanceDataHeader *header);
	void (*renderCB)(Atomic *atomic, InstanceDataHeader *header);
};

void defaultInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
void defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header);
void defaultRenderCB_Fix(Atomic *atomic, InstanceDataHeader *header);
void defaultRenderCB_Shader(Atomic *atomic, InstanceDataHeader *header);

ObjPipeline *makeDefaultPipeline(void);


// Skin plugin

void initSkin(void);
void uploadSkinMatrices(Atomic *atomic);
void skinInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
void skinRenderCB(Atomic *atomic, InstanceDataHeader *header);
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

namespace rw {

#ifdef RW_PS2
struct EngineOpenParams
{
};
#endif

namespace ps2 {

void registerPlatformPlugins(void);

extern Device renderdevice;

struct Im2DVertex
{
	float32 x, y, z, w;
	uint32  r, g, b, a;
	float32 u, v, q, PAD;

	void setScreenX(float32 x) { this->x = x; }
	void setScreenY(float32 y) { this->y = y; }
	void setScreenZ(float32 z) { this->z = z; }
	void setCameraZ(float32 z) { this->w = z; }
	void setRecipCameraZ(float32 recipz) { this->q = recipz; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) {
		this->r = r; this->g = g; this->b = b; this->a = a; }
	void setU(float32 u, float recipz) { this->u = u; }
	void setV(float32 v, float recipz) { this->v = v; }

	float getScreenX(void) { return this->x; }
	float getScreenY(void) { return this->y; }
	float getScreenZ(void) { return this->z; }
	float getCameraZ(void) { return this->w; }
	float getRecipCameraZ(void) { return this->q; }
	RGBA getColor(void) { return makeRGBA(this->r, this->g, this->b, this->a); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};


struct InstanceData
{
	uint32 dataSize;
	uint8 *dataRaw;
	uint8 *data;
	Material *material;
};

struct InstanceDataHeader : rw::InstanceDataHeader
{
	uint32 numMeshes;
	InstanceData *instanceMeshes;
};

enum {
	VU_Lights	= 0x3d0
};

enum PS2Attribs {
	AT_V2_32	= 0x64000000,
	AT_V2_16	= 0x65000000,
	AT_V2_8		= 0x66000000,
	AT_V3_32	= 0x68000000,
	AT_V3_16	= 0x69000000,
	AT_V3_8		= 0x6A000000,
	AT_V4_32	= 0x6C000000,
	AT_V4_16	= 0x6D000000,
	AT_V4_8		= 0x6E000000,
	AT_UNSGN	= 0x00004000,

	AT_RW		= 0x6
};

// Not really types as in RW but offsets
enum PS2AttibTypes {
	AT_XYZ		= 0,
	AT_UV		= 1,
	AT_RGBA		= 2,
	AT_NORMAL	= 3
};

void *destroyNativeData(void *object, int32, int32);
Stream *readNativeData(Stream *stream, int32 len, void *object, int32, int32);
Stream *writeNativeData(Stream *stream, int32 len, void *object, int32, int32);
int32 getSizeNativeData(void *object, int32, int32);
void registerNativeDataPlugin(void);

void printDMA(InstanceData *inst);
void printDMAVIF(InstanceData *inst);
void sizedebug(InstanceData *inst);

void fixDmaOffsets(InstanceData *inst);	// only RW_PS2
int32 unfixDmaOffsets(InstanceData *inst);

struct PipeAttribute
{
	const char *name;
	uint32 attrib;
};

extern PipeAttribute attribXYZ;
extern PipeAttribute attribXYZW;
extern PipeAttribute attribUV;
extern PipeAttribute attribUV2;
extern PipeAttribute attribRGBA;
extern PipeAttribute attribNormal;
extern PipeAttribute attribWeights;

class MatPipeline : public rw::Pipeline
{
public:
	uint32 vifOffset;
	uint32 inputStride;
	// number of vertices for tri strips and lists
	uint32 triStripCount, triListCount;
	PipeAttribute *attribs[10];
	void (*instanceCB)(MatPipeline*, Geometry*, Mesh*, uint8**);
	void (*uninstanceCB)(MatPipeline*, Geometry*, uint32*, Mesh*, uint8**);
	void (*preUninstCB)(MatPipeline*, Geometry*);
	void (*postUninstCB)(MatPipeline*, Geometry*);
	// RW has more:
	//  instanceTestCB()
	//  resEntryAllocCB()
	//  bridgeCB()
	//  postMeshCB()
	//  vu1code
	//  primtype

	static uint32 getVertCount(uint32 top, uint32 inAttribs,
	                           uint32 outAttribs, uint32 outBufs) {
		return (top-outBufs)/(inAttribs*2+outAttribs*outBufs);
	}

	void init(void);
	static MatPipeline *create(void);
	void destroy(void);
	void dump(void);
	void setTriBufferSizes(uint32 inputStride, uint32 bufferSize);
	void instance(Geometry *g, InstanceData *inst, Mesh *m);
	uint8 *collectData(Geometry *g, InstanceData *inst, Mesh *m, uint8 *data[]);
};

class ObjPipeline : public rw::ObjPipeline
{
public:
	void init(void);
	static ObjPipeline *create(void);

	MatPipeline *groupPipeline;
	// RW has more:
	//  setupCB()
	//  finalizeCB()
	//  lightOffset
	//  lightSize
};

struct Vertex {
	V3d       p;
	TexCoords t;
	TexCoords t1;
	RGBA      c;
	V3d       n;
	// skin
	float32 w[4];
	uint8   i[4];
};

void insertVertex(Geometry *geo, int32 i, uint32 mask, Vertex *v);

extern ObjPipeline *defaultObjPipe;
extern MatPipeline *defaultMatPipe;

void genericUninstanceCB(MatPipeline *pipe, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[]);
void genericPreCB(MatPipeline *pipe, Geometry *geo);	// skin and ADC
//void defaultUninstanceCB(MatPipeline *pipe, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[]);
void skinInstanceCB(MatPipeline *, Geometry *g, Mesh *m, uint8 **data);
//void skinUninstanceCB(MatPipeline*, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[]);

ObjPipeline *makeDefaultPipeline(void);
void dumpPipeline(rw::Pipeline *pipe);

// ADC plugin

// Each element in adcBits corresponds to an index in Mesh->indices,
// this assumes the Mesh indices are ADC formatted.
// ADCData->numBits != Mesh->numIndices. ADCData->numBits is probably
// equal to Mesh->numIndices before the Mesh gets ADC formatted.
//
// Can't convert between ADC-formatted and non-ADC-formatted yet :(

struct ADCData
{
	bool32 adcFormatted;
	int8 *adcBits;
	int32 numBits;
};
extern int32 adcOffset;
void registerADCPlugin(void);

int8 *getADCbits(Geometry *geo);
int8 *getADCbitsForMesh(Geometry *geo, Mesh *mesh);
void convertADC(Geometry *g);
void unconvertADC(Geometry *geo);
void allocateADC(Geometry *geo);

// PDS plugin

Pipeline *getPDSPipe(uint32 data);
void registerPDSPipe(Pipeline *pipe);
void registerPDSPlugin(int32 n);
void registerPluginPDSPipes(void);

// Native Texture and Raster

struct Ps2Raster
{
	enum Flags {
		NEWSTYLE  = 0x1,	// has GIF tags and transfer DMA chain
		SWIZZLED8 = 0x2,
		SWIZZLED4 = 0x4
	};
	struct PixelPtr {
		// RW has pixels as second element but we don't want this struct
		// to be longer than 16 bytes
		uint8 *pixels;
		// palette can be allocated in last level, in that case numTransfers is
		// one less than numTotalTransfers.
		int32 numTransfers;
		int32 numTotalTransfers;
	};

	uint64 tex0;
	uint32 paletteBase;   // block address from beginning of GS data (words/64)
	uint16 kl;
	uint8 tex1low;          // MXL and LCM of TEX1
	uint8 unk2;
	uint64 miptbp1;
	uint64 miptbp2;
	uint32 pixelSize;	// in bytes
	uint32 paletteSize;	// in bytes
	uint32 totalSize;	// total size of texture on GS in words
	int8 flags;

	uint8 *data;	//tmp
	uint32 dataSize;
};

extern int32 nativeRasterOffset;
void registerNativeRaster(void);

Texture *readNativeTexture(Stream *stream);
void writeNativeTexture(Texture *tex, Stream *stream);
uint32 getSizeNativeTexture(Texture *tex);

}
}

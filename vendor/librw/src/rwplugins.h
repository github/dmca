namespace rw {

/*
 * HAnim
 */

struct HAnimKeyFrame
{
	HAnimKeyFrame *prev;
	float32        time;
	Quat           q;
	V3d            t;
};

struct HAnimInterpFrame
{
	HAnimKeyFrame *keyFrame1;
	HAnimKeyFrame *keyFrame2;
	Quat           q;
	V3d            t;
};

struct HAnimNodeInfo
{
	int32 id;
	int32 index;
	int32 flags;
	Frame *frame;
};

struct HAnimHierarchy
{
	int32 flags;
	int32 numNodes;
	Matrix *matrices;
	void  *matricesUnaligned;
	HAnimNodeInfo *nodeInfo;
	Frame *parentFrame;
	HAnimHierarchy *parentHierarchy;	// mostly unused
	AnimInterpolator *interpolator;

	static HAnimHierarchy *create(int32 numNodes, int32 *nodeFlags,
			int32 *nodeIDs, int32 flags, int32 maxKeySize);
	void destroy(void);
	void attachByIndex(int32 id);
	void attach(void);
	int32 getIndex(int32 id);
	int32 getIndex(Frame *f);
	void updateMatrices(void);

	static HAnimHierarchy *get(Frame *f);
	static HAnimHierarchy *get(Clump *c){
		return find(c->getFrame()); }
	static HAnimHierarchy *find(Frame *f);

	enum Flags {
		SUBHIERARCHY = 0x1,
		NOMATRICES   = 0x2,

		UPDATEMODELLINGMATRICES = 0x1000,
		UPDATELTMS              = 0x2000,
		LOCALSPACEMATRICES      = 0x4000
	};
	enum NodeFlag {
		POP = 1,
		PUSH
	};
};

struct HAnimData
{
	int32 id;
	HAnimHierarchy *hierarchy;

	static HAnimData *get(Frame *f);
};

extern int32 hAnimOffset;
extern bool32 hAnimDoStream;
void registerHAnimPlugin(void);


/*
 * MatFX
 */

struct MatFX
{
	enum {
		NOTHING = 0,
		BUMPMAP,
		ENVMAP,
		BUMPENVMAP,	// BUMP | ENV
		DUAL,
		UVTRANSFORM,
		DUALUVTRANSFORM
	};
	struct Bump {
		Frame   *frame;
		Texture *bumpedTex;
		Texture *tex;
		float    coefficient;
	};
	struct Env {
		Frame   *frame;
		Texture *tex;
		float    coefficient;
		int32    fbAlpha;
	};
	struct Dual {
		Texture *tex;
		int32    srcBlend;
		int32    dstBlend;
	};
	struct UVtransform {
		Matrix *baseTransform;
		Matrix *dualTransform;
	};
	struct {
		uint32 type;
		union {
			Bump bump;
			Env  env;
			Dual dual;
			UVtransform uvtransform;
		};
	} fx[2];
	uint32 type;

	static void setEffects(Material *m, uint32 flags);
	static uint32 getEffects(const Material *m);
	static MatFX *get(const Material *m);
	int32 getEffectIndex(uint32 type);
	// Bump
	void setBumpTexture(Texture *t);
	void setBumpCoefficient(float32 coef);
	Texture *getBumpTexture(void);
	float32 getBumpCoefficient(void);
	// Env
	void setEnvTexture(Texture *t);
	void setEnvFrame(Frame *f);
	void setEnvCoefficient(float32 coef);
	void setEnvFBAlpha(bool32 useFBAlpha);
	Texture *getEnvTexture(void);
	Frame *getEnvFrame(void);
	float32 getEnvCoefficient(void);
	bool32 getEnvFBAlpha(void);
	// Dual
	void setDualTexture(Texture *t);
	void setDualSrcBlend(int32 blend);
	void setDualDestBlend(int32 blend);
	Texture *getDualTexture(void);
	int32 getDualSrcBlend(void);
	int32 getDualDestBlend(void);
	// UV transform
	void setUVTransformMatrices(Matrix *base, Matrix *dual);
	void getUVTransformMatrices(Matrix **base, Matrix **dual);

	static void enableEffects(Atomic *atomic);
	static void disableEffects(Atomic *atomic);
	static bool32 getEffects(Atomic *atomic);

	static bool32 modulateEnvMap;
};

struct MatFXGlobals
{
	int32 atomicOffset;
	int32 materialOffset;
	ObjPipeline *pipelines[NUM_PLATFORMS];
	ObjPipeline *dummypipe;
};
extern MatFXGlobals matFXGlobals;
void registerMatFXPlugin(void);


/*
 * Skin
 */

struct SkinGlobals
{
	int32 geoOffset;
	int32 atomicOffset;
	ObjPipeline *pipelines[NUM_PLATFORMS];
	ObjPipeline *dummypipe;
};
extern SkinGlobals skinGlobals;

struct Skin
{
	int32 numBones;
	int32 numUsedBones;
	int32 numWeights;
	uint8 *usedBones;
	float *inverseMatrices;
	uint8 *indices;
	float *weights;

	// split skin

	// points into rle for each mesh
	struct RLEcount {
		uint8 start;
		uint8 size;
	};
	// run length encoded used bones
	struct RLE {
		uint8 startbone;  // into remapIndices
		uint8 n;
	};
	int32 boneLimit;
	int32 numMeshes;
	int32 rleSize;
	int8 *remapIndices;
	RLEcount *rleCount;
	RLE *rle;

	uint8 *data;	// only used by delete
	void *platformData; // a place to store platform specific stuff
	bool32 legacyType;	// old skin attached to atomic, needed for always CB

	void init(int32 numBones, int32 numUsedBones, int32 numVertices);
	void findNumWeights(int32 numVertices);
	void findUsedBones(int32 numVertices);

	static void setPipeline(Atomic *a, int32 type);
	static Skin *get(const Geometry *geo){
		return *PLUGINOFFSET(Skin*, geo, skinGlobals.geoOffset);
	}
	static void set(Geometry *geo, Skin *skin){
		*PLUGINOFFSET(Skin*, geo, skinGlobals.geoOffset) = skin;
	}
	static void setHierarchy(Atomic *atomic, HAnimHierarchy *hier){
		*PLUGINOFFSET(HAnimHierarchy*, atomic,
		              skinGlobals.atomicOffset) = hier;
	}
	static HAnimHierarchy *getHierarchy(const Atomic *atomic){
		return *PLUGINOFFSET(HAnimHierarchy*, atomic,
		                     skinGlobals.atomicOffset);
	}
};

Stream *readSkinSplitData(Stream *stream, Skin *skin);
Stream *writeSkinSplitData(Stream *stream, Skin *skin);
int32 skinSplitDataSize(Skin *skin);
void registerSkinPlugin(void);

}

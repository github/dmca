#include <stddef.h>

namespace rw {

struct Animation;

// These sizes of these are sadly not platform independent
// because pointer sizes can vary.

struct KeyFrameHeader
{
	KeyFrameHeader *prev;
	float32 time;

	KeyFrameHeader *next(int32 sz){
		return (KeyFrameHeader*)((uint8*)this + sz); }
};

struct InterpFrameHeader
{
	KeyFrameHeader *keyFrame1;
	KeyFrameHeader *keyFrame2;
};

struct AnimInterpolatorInfo
{
	typedef void (*ApplyCB)(void *result, void *frame);
	typedef void (*BlendCB)(void *out, void *in1, void *in2, float32 a);
	typedef void (*InterpCB)(void *out, void *in1, void *in2, float32 t,
	                         void *custom);
	typedef void (*AddCB)(void *out, void *in1, void *in2);
	typedef void (*MulRecipCB)(void *frame, void *start);

	int32      id;
	int32      interpKeyFrameSize;
	int32      animKeyFrameSize;
	int32      customDataSize;

	ApplyCB    applyCB;
	BlendCB    blendCB;
	InterpCB   interpCB;
	AddCB      addCB;
	MulRecipCB mulRecipCB;
	void (*streamRead)(Stream *stream, Animation *anim);
	void (*streamWrite)(Stream *stream, Animation *anim);
	uint32 (*streamGetSize)(Animation *anim);

	static void registerInterp(AnimInterpolatorInfo *interpInfo);
	static void unregisterInterp(AnimInterpolatorInfo *interpInfo);
	static AnimInterpolatorInfo *find(int32 id);
};

struct Animation
{
	AnimInterpolatorInfo *interpInfo;
	int32    numFrames;
	int32    flags;
	float32  duration;
	void    *keyframes;
	void    *customData;

	static Animation *create(AnimInterpolatorInfo*, int32 numFrames,
	                         int32 flags, float duration);
	void destroy(void);
	int32 getNumNodes(void);
	KeyFrameHeader *getAnimFrame(int32 n){
		return (KeyFrameHeader*)((uint8*)this->keyframes +
		                         n*this->interpInfo->animKeyFrameSize);
	}
	static Animation *streamRead(Stream *stream);
	static Animation *streamReadLegacy(Stream *stream);
	bool streamWrite(Stream *stream);
	bool streamWriteLegacy(Stream *stream);
	uint32 streamGetSize(void);
};

struct AnimInterpolator
{
	Animation *currentAnim;
	float32    currentTime;
	void      *nextFrame;
	int32      maxInterpKeyFrameSize;
	int32      currentInterpKeyFrameSize;
	int32      currentAnimKeyFrameSize;
	int32      numNodes;
	// TODO some callbacks, parent/sub
	// cached from the InterpolatorInfo
	AnimInterpolatorInfo::ApplyCB    applyCB;
	AnimInterpolatorInfo::BlendCB    blendCB;
	AnimInterpolatorInfo::InterpCB   interpCB;
	AnimInterpolatorInfo::AddCB      addCB;
	// after this interpolated frames

	static AnimInterpolator *create(int32 numNodes, int32 maxKeyFrameSize);
	void destroy(void);
	bool32 setCurrentAnim(Animation *anim);
	void addTime(float32 t);
	void *getFrames(void){ return this+1;}
	InterpFrameHeader *getInterpFrame(int32 n){
		return (InterpFrameHeader*)((uint8*)getFrames() +
		                            n*currentInterpKeyFrameSize);
	}
	KeyFrameHeader *getAnimFrame(int32 n){
		return (KeyFrameHeader*)((uint8*)currentAnim->keyframes +
		                         n*currentAnimKeyFrameSize);
	}
};

//
// UV anim
//

struct UVAnimParamData
{
	float32 theta;	// rotation
	float32 s0;	// scale x
	float32 s1;	// scale y
	float32 skew;	// skew
	float32 x;	// x pos
	float32 y;	// y pos
};

struct UVAnimKeyFrame
{
	UVAnimKeyFrame *prev;
	float32 time;
	float32 uv[6];
};

struct UVAnimInterpFrame
{
	UVAnimKeyFrame *keyFrame1;
	UVAnimKeyFrame *keyFrame2;
	float32 uv[6];
};

struct UVAnimDictionary;

// RW does it differently...maybe we should implement RtDict
// and make it more general?

struct UVAnimCustomData
{
	char name[32];
	int32 nodeToUVChannel[8];
	int32 refCount;

	void destroy(Animation *anim);
	static UVAnimCustomData *get(Animation *anim){
		return (UVAnimCustomData*)anim->customData; }
};

// This should be more general probably
struct UVAnimDictEntry
{
	Animation *anim;
	LLLink inDict;
	static UVAnimDictEntry *fromDict(LLLink *lnk){
		return LLLinkGetData(lnk, UVAnimDictEntry, inDict); }
};

// This too
struct UVAnimDictionary
{
	LinkList animations;

	static UVAnimDictionary *create(void);
	void destroy(void);
	int32 count(void) { return this->animations.count(); }
	void add(Animation *anim);
	Animation *find(const char *name);

	static UVAnimDictionary *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
};

extern UVAnimDictionary *currentUVAnimDictionary;

// Material plugin
struct UVAnim
{
	Matrix *uv[2];
	AnimInterpolator *interp[8];

	static bool32 exists(Material *mat);
	static void addTime(Material *mat, float32 t);
	static void applyUpdate(Material *mat);
};

extern int32 uvAnimOffset;

void registerUVAnimPlugin(void);

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"
#include "rwanim.h"
#include "rwplugins.h"
#include "ps2/rwps2.h"
#include "ps2/rwps2plg.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"
#include "gl/rwwdgl.h"
#include "gl/rwgl3.h"

#define PLUGIN_ID ID_HANIM

namespace rw {

int32 hAnimOffset;
bool32 hAnimDoStream = 1;

HAnimHierarchy*
HAnimHierarchy::create(int32 numNodes, int32 *nodeFlags, int32 *nodeIDs,
                       int32 flags, int32 maxKeySize)
{
	HAnimHierarchy *hier = (HAnimHierarchy*)rwMalloc(sizeof(*hier), MEMDUR_EVENT | ID_HANIM);
	if(hier == nil){
		RWERROR((ERR_ALLOC, sizeof(*hier)));
		return nil;
	}
	hier->interpolator = AnimInterpolator::create(numNodes, maxKeySize);

	hier->numNodes = numNodes;
	hier->flags = flags;
	hier->parentFrame = nil;
	hier->parentHierarchy = hier;
	if(hier->flags & NOMATRICES){
		hier->matrices = nil;
		hier->matricesUnaligned = nil;
	}else{
		hier->matricesUnaligned = rwNew(hier->numNodes*64 + 0xF, MEMDUR_EVENT | ID_HANIM);
		hier->matrices =
		  (Matrix*)(((uintptr)hier->matricesUnaligned + 0xF) & ~0xF);
	}
	hier->nodeInfo = rwNewT(HAnimNodeInfo, hier->numNodes, MEMDUR_EVENT | ID_HANIM);
	for(int32 i = 0; i < hier->numNodes; i++){
		if(nodeIDs)
			hier->nodeInfo[i].id = nodeIDs[i];
		else
			hier->nodeInfo[i].id = 0;
		hier->nodeInfo[i].index = i;
		if(nodeFlags)
			hier->nodeInfo[i].flags = nodeFlags[i];
		else
			hier->nodeInfo[i].flags = 0;
		hier->nodeInfo[i].frame = nil;
	}
	return hier;
}

void
HAnimHierarchy::destroy(void)
{
	this->interpolator->destroy();
	rwFree(this->matricesUnaligned);
	rwFree(this->nodeInfo);
	rwFree(this);
}

static Frame*
findById(Frame *f, int32 id)
{
	if(f == nil) return nil;
	HAnimData *hanim = HAnimData::get(f);
	if(hanim->id >= 0 && hanim->id == id) return f;
	Frame *ff = findById(f->next, id);
	if(ff) return ff;
	return findById(f->child, id);
}

static Frame*
findUnattachedById(HAnimHierarchy *hier, Frame *f, int32 id)
{
	if(f == nil) return nil;
	HAnimData *hanim = HAnimData::get(f);
	if(hanim->id >= 0 && hanim->id == id && hier->getIndex(f) == -1) return f;
	Frame *ff = findUnattachedById(hier, f->next, id);
	if(ff) return ff;
	return findUnattachedById(hier, f->child, id);
}

void
HAnimHierarchy::attachByIndex(int32 idx)
{
	int32 id = this->nodeInfo[idx].id;
//	Frame *f = findById(this->parentFrame, id);
	Frame *f = findUnattachedById(this, this->parentFrame, id);
	if(f)
		this->nodeInfo[idx].frame = f;
}

void
HAnimHierarchy::attach(void)
{
	for(int32 i = 0; i < this->numNodes; i++)
		this->attachByIndex(i);
}

int32
HAnimHierarchy::getIndex(int32 id)
{
	for(int32 i = 0; i < this->numNodes; i++)
		if(this->nodeInfo[i].id == id)
			return i;
	return -1;
}

int32
HAnimHierarchy::getIndex(Frame *f)
{
	for(int32 i = 0; i < this->numNodes; i++)
		if(this->nodeInfo[i].frame == f)
			return i;
	return -1;
}

HAnimHierarchy*
HAnimHierarchy::get(Frame *f)
{
	return HAnimData::get(f)->hierarchy;
}

HAnimHierarchy*
HAnimHierarchy::find(Frame *f)
{
	if(f == nil) return nil;
	HAnimHierarchy *hier = HAnimHierarchy::get(f);
	if(hier) return hier;
	hier = HAnimHierarchy::find(f->next);
	if(hier) return hier;
	return HAnimHierarchy::find(f->child);
}

void
HAnimHierarchy::updateMatrices(void)
{
	// TODO: handle more (all!) cases

	Matrix rootMat, animMat;
	Matrix *curMat, *parentMat;
	Matrix **sp, *stack[64];
	Frame *frm, *parfrm;
	int32 i;
	AnimInterpolator *anim = this->interpolator;

	sp = stack;
	curMat = this->matrices;

	frm = this->parentFrame;
	if(frm && (parfrm = frm->getParent()) && !(this->flags&LOCALSPACEMATRICES))
		rootMat = *parfrm->getLTM();
	else
		rootMat.setIdentity();
	parentMat = &rootMat;
	*sp++ = parentMat;
	HAnimNodeInfo *node = this->nodeInfo;
	for(i = 0; i < this->numNodes; i++){
		anim->applyCB(&animMat, anim->getInterpFrame(i));

		// TODO: here we could update local matrices

		Matrix::mult(curMat, &animMat, parentMat);

		// TODO: here we could update LTM

		if(node->flags & PUSH)
			*sp++ = parentMat;
		parentMat = curMat;
		if(node->flags & POP)
			parentMat = *--sp;
		assert(sp >= stack);
		assert(sp <= &stack[64]);

		node++;
		curMat++;
	}
}

HAnimData*
HAnimData::get(Frame *f)
{
	return PLUGINOFFSET(HAnimData, f, hAnimOffset);
}

static void*
createHAnim(void *object, int32 offset, int32)
{
	HAnimData *hanim = PLUGINOFFSET(HAnimData, object, offset);
	hanim->id = -1;
	hanim->hierarchy = nil;
	return object;
}

static void*
destroyHAnim(void *object, int32 offset, int32)
{
	int i;
	HAnimData *hanim = PLUGINOFFSET(HAnimData, object, offset);
	if(hanim->hierarchy){
		for(i = 0; i < hanim->hierarchy->numNodes; i++)
			hanim->hierarchy->nodeInfo[i].frame = nil;
		if(object == hanim->hierarchy->parentFrame)
			hanim->hierarchy->destroy();
	}
	hanim->id = -1;
	hanim->hierarchy = nil;
	return object;
}

static void*
copyHAnim(void *dst, void *src, int32 offset, int32)
{
	int i;
	HAnimData *dsthanim = PLUGINOFFSET(HAnimData, dst, offset);
	HAnimData *srchanim = PLUGINOFFSET(HAnimData, src, offset);
	HAnimHierarchy *srchier, *dsthier;
	dsthanim->id = srchanim->id;
	dsthanim->hierarchy = nil;
	srchier = srchanim->hierarchy;
	if(srchier && !(srchier->flags & HAnimHierarchy::SUBHIERARCHY)){
		dsthier = HAnimHierarchy::create(srchier->numNodes, nil, nil, srchier->flags, srchier->interpolator->maxInterpKeyFrameSize);
		for(i = 0; i < dsthier->numNodes; i++){
			dsthier->nodeInfo[i].frame = nil;
			dsthier->nodeInfo[i].flags = srchier->nodeInfo[i].flags;
			dsthier->nodeInfo[i].index = srchier->nodeInfo[i].index;
			dsthier->nodeInfo[i].id = srchier->nodeInfo[i].id;
		}
		dsthanim->hierarchy = dsthier;
		dsthier->parentFrame = (Frame*)dst;
	}
	return dst;
}

static Stream*
readHAnim(Stream *stream, int32, void *object, int32 offset, int32)
{
	int32 ver, numNodes;
	HAnimData *hanim = PLUGINOFFSET(HAnimData, object, offset);
	ver = stream->readI32();
	assert(ver == 0x100);
	hanim->id = stream->readI32();
	numNodes = stream->readI32();
	if(numNodes != 0){
		int32 flags = stream->readI32();
		int32 maxKeySize = stream->readI32();
		// Sizes are fucked for 64 bit pointers but
		// AnimInterpolator::create() will take care of that
		int32 *nodeFlags = rwNewT(int32, numNodes,
			MEMDUR_FUNCTION | ID_HANIM);
		int32 *nodeIDs = rwNewT(int32, numNodes,
			MEMDUR_FUNCTION | ID_HANIM);
		for(int32 i = 0; i < numNodes; i++){
			nodeIDs[i] = stream->readI32();
			stream->readI32();	// index...unused
			nodeFlags[i] = stream->readI32();
		}
		hanim->hierarchy = HAnimHierarchy::create(numNodes,
			nodeFlags, nodeIDs, flags, maxKeySize);
		hanim->hierarchy->parentFrame = (Frame*)object;
		rwFree(nodeFlags);
		rwFree(nodeIDs);
	}
	return stream;
}

static Stream*
writeHAnim(Stream *stream, int32, void *object, int32 offset, int32)
{
	HAnimData *hanim = PLUGINOFFSET(HAnimData, object, offset);
	stream->writeI32(256);
	stream->writeI32(hanim->id);
	if(hanim->hierarchy == nil){
		stream->writeI32(0);
		return stream;
	}
	HAnimHierarchy *hier = hanim->hierarchy;
	stream->writeI32(hier->numNodes);
	stream->writeI32(hier->flags);
	stream->writeI32(hier->interpolator->maxInterpKeyFrameSize);
	for(int32 i = 0; i < hier->numNodes; i++){
		stream->writeI32(hier->nodeInfo[i].id);
		stream->writeI32(hier->nodeInfo[i].index);
		stream->writeI32(hier->nodeInfo[i].flags);
	}
	return stream;
}

static int32
getSizeHAnim(void *object, int32 offset, int32)
{
	HAnimData *hanim = PLUGINOFFSET(HAnimData, object, offset);
	if(!hAnimDoStream ||
	   (version >= 0x35000 && hanim->id == -1 && hanim->hierarchy == nil))
		return 0;
	if(hanim->hierarchy)
		return 12 + 8 + hanim->hierarchy->numNodes*12;
	return 12;
}

static void
hAnimFrameRead(Stream *stream, Animation *anim)
{
	HAnimKeyFrame *frames = (HAnimKeyFrame*)anim->keyframes;
	for(int32 i = 0; i < anim->numFrames; i++){
		frames[i].time = stream->readF32();
		stream->read32(&frames[i].q, 4*4);
		stream->read32(&frames[i].t, 3*4);
		int32 prev = stream->readI32()/0x24;
		frames[i].prev = &frames[prev];
	}
}

static void
hAnimFrameWrite(Stream *stream, Animation *anim)
{
	HAnimKeyFrame *frames = (HAnimKeyFrame*)anim->keyframes;
	for(int32 i = 0; i < anim->numFrames; i++){
		stream->writeF32(frames[i].time);
		stream->write32(&frames[i].q, 4*4);
		stream->write32(&frames[i].t, 3*4);
		stream->writeI32((frames[i].prev - frames)*0x24);
	}
}

static uint32
hAnimFrameGetSize(Animation *anim)
{
	return anim->numFrames*(4 + 4*4 + 3*4 + 4);
}

//void hanimBlendCB(void *out, void *in1, void *in2, float32 a);
//void hanimAddCB(void *out, void *in1, void *in2);
//void hanimMulRecipCB(void *frame, void *start);

static void
hanimApplyCB(void *result, void *frame)
{
	Matrix *m = (Matrix*)result;
	HAnimInterpFrame *f = (HAnimInterpFrame*)frame;
	m->rotate(f->q, COMBINEREPLACE);
	m->pos = f->t;
}

static void
hanimInterpCB(void *vout, void *vin1, void *vin2, float32 t, void*)
{
	HAnimInterpFrame *out = (HAnimInterpFrame*)vout;
	HAnimKeyFrame *in1 = (HAnimKeyFrame*)vin1;
	HAnimKeyFrame *in2 = (HAnimKeyFrame*)vin2;
assert(t >= in1->time && t <= in2->time);
	float32 a = (t - in1->time)/(in2->time - in1->time);
	out->t =  lerp(in1->t, in2->t, a);
	out->q = slerp(in1->q, in2->q, a);
}

static void*
hanimOpen(void *object, int32 offset, int32 size)
{
	AnimInterpolatorInfo *info = rwNewT(AnimInterpolatorInfo, 1, MEMDUR_GLOBAL | ID_HANIM);
	info->id = 1;
	info->interpKeyFrameSize = sizeof(HAnimInterpFrame);
	info->animKeyFrameSize = sizeof(HAnimKeyFrame);
	info->customDataSize = 0;
	info->applyCB = hanimApplyCB;
	info->blendCB = nil;
	info->interpCB = hanimInterpCB;
	info->addCB = nil;
	info->mulRecipCB = nil;
	info->streamRead = hAnimFrameRead;
	info->streamWrite = hAnimFrameWrite;
	info->streamGetSize = hAnimFrameGetSize;
	AnimInterpolatorInfo::registerInterp(info);
	return object;
}

static void*
hanimClose(void *object, int32 offset, int32 size)
{
	AnimInterpolatorInfo::unregisterInterp(AnimInterpolatorInfo::find(1));
	return object;
}


void
registerHAnimPlugin(void)
{
	Engine::registerPlugin(0, ID_HANIM, hanimOpen, hanimClose);
	hAnimOffset = Frame::registerPlugin(sizeof(HAnimData), ID_HANIM,
	                                    createHAnim,
	                                    destroyHAnim, copyHAnim);
	Frame::registerPluginStream(ID_HANIM,
	                            readHAnim,
	                            writeHAnim,
	                            getSizeHAnim);
}

}

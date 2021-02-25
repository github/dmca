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

#define PLUGIN_ID ID_ANIMANIMATION

namespace rw {

//
// AnimInterpolatorInfo
//

#define MAXINTERPINFO 10

static AnimInterpolatorInfo *interpInfoList[MAXINTERPINFO];

void
AnimInterpolatorInfo::registerInterp(AnimInterpolatorInfo *interpInfo)
{
	for(int32 i = 0; i < MAXINTERPINFO; i++)
		if(interpInfoList[i] == nil){
			interpInfoList[i] = interpInfo;
			return;
		}
	assert(0 && "no room for interpolatorInfo");
}

void
AnimInterpolatorInfo::unregisterInterp(AnimInterpolatorInfo *interpInfo)
{
	for(int32 i = 0; i < MAXINTERPINFO; i++)
		if(interpInfoList[i] == interpInfo){
			rwFree(interpInfoList[i]);
			interpInfoList[i] = nil;
			return;
		}
}

AnimInterpolatorInfo*
AnimInterpolatorInfo::find(int32 id)
{
	for(int32 i = 0; i < MAXINTERPINFO; i++){
		if(interpInfoList[i] && interpInfoList[i]->id == id)
			return interpInfoList[i];
	}
	return nil;
}

//
// Animation
//

Animation*
Animation::create(AnimInterpolatorInfo *interpInfo, int32 numFrames,
                  int32 flags, float duration)
{
	int32 sz = sizeof(Animation) +
	           numFrames*interpInfo->animKeyFrameSize +
	           interpInfo->customDataSize;
	uint8 *data = (uint8*)rwMalloc(sz, MEMDUR_EVENT | ID_ANIMANIMATION);
	if(data == nil){
		RWERROR((ERR_ALLOC, sz));
		return nil;
	}
	Animation *anim = (Animation*)data;
	data += sizeof(Animation);
	anim->interpInfo = interpInfo;
	anim->numFrames = numFrames;
	anim->flags = flags;
	anim->duration = duration;
	anim->keyframes = data;
	data += anim->numFrames*interpInfo->animKeyFrameSize;
	anim->customData = data;
	return anim;
}

void
Animation::destroy(void)
{
	rwFree(this);
}

int32
Animation::getNumNodes(void)
{
	int32 sz = this->interpInfo->animKeyFrameSize;
	KeyFrameHeader *first = (KeyFrameHeader*)this->keyframes;
	int32 n = 0;
	for(KeyFrameHeader *f = first; f->prev != first; f = f->next(sz))
		n++;
	return n;
}

Animation*
Animation::streamRead(Stream *stream)
{
	Animation *anim;
	if(stream->readI32() != 0x100)
		return nil;
	int32 typeID = stream->readI32();
	AnimInterpolatorInfo *interpInfo = AnimInterpolatorInfo::find(typeID);
	int32 numFrames = stream->readI32();
	int32 flags = stream->readI32();
	float duration = stream->readF32();
	anim = Animation::create(interpInfo, numFrames, flags, duration);
	interpInfo->streamRead(stream, anim);
	return anim;
}

Animation*
Animation::streamReadLegacy(Stream *stream)
{
	Animation *anim;
	AnimInterpolatorInfo *interpInfo = AnimInterpolatorInfo::find(1);
	int32 numFrames = stream->readI32();
	int32 flags = stream->readI32();
	float duration = stream->readF32();
	anim = Animation::create(interpInfo, numFrames, flags, duration);
	HAnimKeyFrame *frames = (HAnimKeyFrame*)anim->keyframes;
	for(int32 i = 0; i < anim->numFrames; i++){
		stream->read32(&frames[i].q, 4*4);
		frames[i].q = conj(frames[i].q);
		stream->read32(&frames[i].t, 3*4);
		frames[i].time = stream->readF32();
		int32 prev = stream->readI32()/0x24;
		frames[i].prev = &frames[prev];
	}
	return anim;
}

bool
Animation::streamWrite(Stream *stream)
{
	writeChunkHeader(stream, ID_ANIMANIMATION, this->streamGetSize());
	stream->writeI32(0x100);
	stream->writeI32(this->interpInfo->id);
	stream->writeI32(this->numFrames);
	stream->writeI32(this->flags);
	stream->writeF32(this->duration);
	this->interpInfo->streamWrite(stream, this);
	return true;
}

bool
Animation::streamWriteLegacy(Stream *stream)
{
	stream->writeI32(this->numFrames);
	stream->writeI32(this->flags);
	stream->writeF32(this->duration);
	assert(interpInfo->id == 1);
	HAnimKeyFrame *frames = (HAnimKeyFrame*)this->keyframes;
	for(int32 i = 0; i < this->numFrames; i++){
		frames[i].q = conj(frames[i].q);
		stream->write32(&frames[i].q, 4*4);
		frames[i].q = conj(frames[i].q);
		stream->write32(&frames[i].t, 3*4);
		stream->writeF32(frames[i].time);
		stream->writeI32((frames[i].prev - frames)*0x24);
	}
	return true;
}

uint32
Animation::streamGetSize(void)
{
	uint32 size = 4 + 4 + 4 + 4 + 4;
	size += this->interpInfo->streamGetSize(this);
	return size;
}

//
// AnimInterpolator
//

AnimInterpolator*
AnimInterpolator::create(int32 numNodes, int32 maxFrameSize)
{
	AnimInterpolator *interp;
	int32 sz;
	int32 realsz = maxFrameSize;

	// Add some space for pointers and padding, hopefully this will be
	// enough. Don't change maxFrameSize not to mess up streaming.
	if(sizeof(void*) > 4)
		realsz += 16;
	sz = sizeof(AnimInterpolator) + numNodes*realsz;
	interp = (AnimInterpolator*)rwMalloc(sz, MEMDUR_EVENT | ID_ANIMANIMATION);
	if(interp == nil){
		RWERROR((ERR_ALLOC, sz));
		return nil;
	}
	interp->currentAnim = nil;
	interp->currentTime = 0.0f;
	interp->nextFrame = nil;
	interp->maxInterpKeyFrameSize = maxFrameSize;
	interp->currentInterpKeyFrameSize = maxFrameSize;
	interp->currentAnimKeyFrameSize = -1;
	interp->numNodes = numNodes;;

	return interp;
}

void
AnimInterpolator::destroy(void)
{
	rwFree(this);
}

bool32
AnimInterpolator::setCurrentAnim(Animation *anim)
{
	int32 i;
	AnimInterpolatorInfo *interpInfo = anim->interpInfo;
	this->currentAnim = anim;
	this->currentTime = 0.0f;
	int32 maxkf = this->maxInterpKeyFrameSize;
	if(sizeof(void*) > 4)	// see above in create()
		maxkf += 16;
	if(interpInfo->interpKeyFrameSize > maxkf){
		RWERROR((ERR_GENERAL, "interpolation frame too big"));
		return 0;
	}
	this->currentInterpKeyFrameSize = interpInfo->interpKeyFrameSize;
	this->currentAnimKeyFrameSize = interpInfo->animKeyFrameSize;
	this->applyCB = interpInfo->applyCB;
	this->blendCB = interpInfo->blendCB;
	this->interpCB = interpInfo->interpCB;
	this->addCB = interpInfo->addCB;
	for(i = 0; i < numNodes; i++){
		InterpFrameHeader *intf;
		KeyFrameHeader *kf1, *kf2;
		intf = this->getInterpFrame(i);
		kf1 = this->getAnimFrame(i);
		kf2 = this->getAnimFrame(i+numNodes);
		intf->keyFrame1 = kf1;
		intf->keyFrame2 = kf2;
		// TODO: perhaps just implement all interpolator infos?
		if(this->interpCB)
			this->interpCB(intf, kf1, kf2, 0.0f, anim->customData);
	}
	this->nextFrame = this->getAnimFrame(numNodes*2);
	return 1;
}

void
AnimInterpolator::addTime(float32 t)
{
	int32 i;
	if(t <= 0.0f)
		return;
	this->currentTime += t;
	// reset animation
	if(this->currentTime > this->currentAnim->duration){
		this->setCurrentAnim(this->currentAnim);
		return;
	}
	KeyFrameHeader *last = this->getAnimFrame(this->currentAnim->numFrames);
	KeyFrameHeader *next = (KeyFrameHeader*)this->nextFrame;
	InterpFrameHeader *ifrm = nil;
	while(next < last && next->prev->time <= this->currentTime){
		// find next interpolation frame to expire
		for(i = 0; i < this->numNodes; i++){
			ifrm = this->getInterpFrame(i);
			if(ifrm->keyFrame2 == next->prev)
				break;
		}
		// advance interpolation frame
		ifrm->keyFrame1 = ifrm->keyFrame2;
		ifrm->keyFrame2 = next;
		// ... and next frame
		next = (KeyFrameHeader*)((uint8*)this->nextFrame +
		                         currentAnimKeyFrameSize);
		this->nextFrame = next;
	}
	for(i = 0; i < this->numNodes; i++){
		ifrm = this->getInterpFrame(i);
		this->interpCB(ifrm, ifrm->keyFrame1, ifrm->keyFrame2,
		               this->currentTime,
		               this->currentAnim->customData);
	}
}

}

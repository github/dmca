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

#define PLUGIN_ID ID_UVANIMATION

namespace rw {

//
// UVAnim
//

void
UVAnimCustomData::destroy(Animation *anim)
{
	this->refCount--;
	if(this->refCount <= 0)
		anim->destroy();
}

UVAnimDictionary *currentUVAnimDictionary;

UVAnimDictionary*
UVAnimDictionary::create(void)
{
	UVAnimDictionary *dict = (UVAnimDictionary*)rwMalloc(sizeof(UVAnimDictionary), MEMDUR_EVENT | ID_UVANIMATION);
	if(dict == nil){
		RWERROR((ERR_ALLOC, sizeof(UVAnimDictionary)));
		return nil;
	}
	dict->animations.init();
	return dict;
}

void
UVAnimDictionary::destroy(void)
{
	FORLIST(lnk, this->animations){
		UVAnimDictEntry *de = UVAnimDictEntry::fromDict(lnk);
		UVAnimCustomData *cust = UVAnimCustomData::get(de->anim);
		cust->destroy(de->anim);
		rwFree(de);
	}
	rwFree(this);
}

void
UVAnimDictionary::add(Animation *anim)
{
	UVAnimDictEntry *de = rwNewT(UVAnimDictEntry, 1, MEMDUR_EVENT | ID_UVANIMDICT);
	de->anim = anim;
	this->animations.append(&de->inDict);
}

UVAnimDictionary*
UVAnimDictionary::streamRead(Stream *stream)
{
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	UVAnimDictionary *dict = UVAnimDictionary::create();
	if(dict == nil)
		return nil;
	int32 numAnims = stream->readI32();
	Animation *anim;
	for(int32 i = 0; i < numAnims; i++){
		if(!findChunk(stream, ID_ANIMANIMATION, nil, nil)){
			RWERROR((ERR_CHUNK, "ANIMANIMATION"));
			goto fail;
		}
		anim = Animation::streamRead(stream);
		if(anim == nil)
			goto fail;
		dict->add(anim);
	}
	return dict;
fail:
	dict->destroy();
	return nil;
}

bool
UVAnimDictionary::streamWrite(Stream *stream)
{
	uint32 size = this->streamGetSize();
	writeChunkHeader(stream, ID_UVANIMDICT, size);
	writeChunkHeader(stream, ID_STRUCT, 4);
	int32 numAnims = this->count();
	stream->writeI32(numAnims);
	FORLIST(lnk, this->animations){
		UVAnimDictEntry *de = UVAnimDictEntry::fromDict(lnk);
		de->anim->streamWrite(stream);
	}
	return true;
}

uint32
UVAnimDictionary::streamGetSize(void)
{
	uint32 size = 12 + 4;
	FORLIST(lnk, this->animations){
		UVAnimDictEntry *de = UVAnimDictEntry::fromDict(lnk);
		size += 12 + de->anim->streamGetSize();
	}
	return size;
}

Animation*
UVAnimDictionary::find(const char *name)
{
	FORLIST(lnk, this->animations){
		Animation *anim = UVAnimDictEntry::fromDict(lnk)->anim;
		UVAnimCustomData *custom = UVAnimCustomData::get(anim);
		if(strncmp_ci(custom->name, name, 32) == 0)
			return anim;
	}
	return nil;
}

static void
uvAnimStreamRead(Stream *stream, Animation *anim)
{
	UVAnimCustomData *custom = UVAnimCustomData::get(anim);
	UVAnimKeyFrame *frames = (UVAnimKeyFrame*)anim->keyframes;
	stream->readI32();
	stream->read8(custom->name, 32);
	stream->read32(custom->nodeToUVChannel, 8*4);
	custom->refCount = 1;

	for(int32 i = 0; i < anim->numFrames; i++){
		frames[i].time = stream->readF32();
		stream->read32(frames[i].uv, 6*4);
		int32 prev = stream->readI32();
		frames[i].prev = &frames[prev];
	}
}

static void
uvAnimStreamWrite(Stream *stream, Animation *anim)
{
	UVAnimCustomData *custom = UVAnimCustomData::get(anim);
	UVAnimKeyFrame *frames = (UVAnimKeyFrame*)anim->keyframes;
	stream->writeI32(0);
	stream->write8(custom->name, 32);
	stream->write32(custom->nodeToUVChannel, 8*4);

	for(int32 i = 0; i < anim->numFrames; i++){
		stream->writeF32(frames[i].time);
		stream->write32(frames[i].uv, 6*4);
		stream->writeI32(frames[i].prev - frames);
	}
}

static uint32
uvAnimStreamGetSize(Animation *anim)
{
	return 4 + 32 + 8*4 + anim->numFrames*(4 + 6*4 + 4);
}

static void
uvAnimLinearApplyCB(void *result, void *frame)
{
	Matrix *m = (Matrix*)result;
	UVAnimInterpFrame *f = (UVAnimInterpFrame*)frame;
	m->right.x = f->uv[0];
	m->right.y = f->uv[1];
	m->right.z = 0.0f;
	m->up.x = f->uv[2];
	m->up.y = f->uv[3];
	m->up.z = 0.0f;
	m->at.x = 0.0f;
	m->at.y = 0.0f;
	m->at.z = 0.0f;
	m->pos.x = f->uv[4];
	m->pos.y = f->uv[5];
	m->pos.z = 0.0f;
	m->update();
}

static void
uvAnimLinearInterpCB(void *out, void *in1, void *in2, float32 t, void *custom)
{
	UVAnimInterpFrame *intf = (UVAnimInterpFrame*)out;
	UVAnimKeyFrame *kf1 = (UVAnimKeyFrame*)in1;
	UVAnimKeyFrame *kf2 = (UVAnimKeyFrame*)in2;
	float32 f = (t - kf1->time) / (kf2->time - kf1->time);
	intf->uv[0] = (kf2->uv[0] - kf1->uv[0])*f + kf1->uv[0];
	intf->uv[1] = (kf2->uv[1] - kf1->uv[1])*f + kf1->uv[1];
	intf->uv[2] = (kf2->uv[2] - kf1->uv[2])*f + kf1->uv[2];
	intf->uv[3] = (kf2->uv[3] - kf1->uv[3])*f + kf1->uv[3];
	intf->uv[4] = (kf2->uv[4] - kf1->uv[4])*f + kf1->uv[4];
	intf->uv[5] = (kf2->uv[5] - kf1->uv[5])*f + kf1->uv[5];
}

static void
uvAnimParamApplyCB(void *result, void *frame)
{
	Matrix *m = (Matrix*)result;
	UVAnimInterpFrame *f = (UVAnimInterpFrame*)frame;
	UVAnimParamData *p = (UVAnimParamData*)f->uv;

	m->right.x = p->s0;
	m->right.y = p->skew;
	m->right.z = 0.0f;
	m->up.x = 0.0f;
	m->up.y = p->s1;
	m->up.z = 0.0f;
	m->at.x = 0.0f;
	m->at.y = 0.0f;
	m->at.z = 0.0f;
	m->pos.x = p->x;
	m->pos.y = p->y;
	m->pos.z = 0.0f;
	m->update();
	static V3d xlat1 = { -0.5f, -0.5f, 0.0f };
	static V3d xlat2 = {  0.5f,  0.5f, 0.0f };
	static V3d axis = { 0.0f, 0.0f, 1.0f };
	m->translate(&xlat1, COMBINEPOSTCONCAT);
	m->rotate(&axis, p->theta*180.0f/(float)M_PI, COMBINEPOSTCONCAT);
	m->translate(&xlat2, COMBINEPOSTCONCAT);
}

static void
uvAnimParamInterpCB(void *out, void *in1, void *in2, float32 t, void *custom)
{
	UVAnimInterpFrame *intf = (UVAnimInterpFrame*)out;
	UVAnimKeyFrame *kf1 = (UVAnimKeyFrame*)in1;
	UVAnimKeyFrame *kf2 = (UVAnimKeyFrame*)in2;
	float32 f = (t - kf1->time) / (kf2->time - kf1->time);

	float32 a = kf2->uv[0] - kf1->uv[0];
	while(a < (float)M_PI) a += 2 * (float)M_PI;
	while(a > (float)M_PI) a -= 2 * (float)M_PI;
	intf->uv[0] = a*f + kf1->uv[0];
	intf->uv[1] = (kf2->uv[1] - kf1->uv[1])*f + kf1->uv[1];
	intf->uv[2] = (kf2->uv[2] - kf1->uv[2])*f + kf1->uv[2];
	intf->uv[3] = (kf2->uv[3] - kf1->uv[3])*f + kf1->uv[3];
	intf->uv[4] = (kf2->uv[4] - kf1->uv[4])*f + kf1->uv[4];
	intf->uv[5] = (kf2->uv[5] - kf1->uv[5])*f + kf1->uv[5];
}


static void
registerUVAnimInterpolator(void)
{
	// TODO: implement this fully

	// Linear
	AnimInterpolatorInfo *info = rwNewT(AnimInterpolatorInfo, 1, MEMDUR_GLOBAL | ID_UVANIMATION);
	info->id = 0x1C0;
	info->interpKeyFrameSize = sizeof(UVAnimInterpFrame);
	info->animKeyFrameSize = sizeof(UVAnimKeyFrame);
	info->customDataSize = sizeof(UVAnimCustomData);
	info->applyCB = uvAnimLinearApplyCB;
	info->blendCB = nil;
	info->interpCB = uvAnimLinearInterpCB;
	info->addCB = nil;
	info->mulRecipCB = nil;
	info->streamRead = uvAnimStreamRead;
	info->streamWrite = uvAnimStreamWrite;
	info->streamGetSize = uvAnimStreamGetSize;
	AnimInterpolatorInfo::registerInterp(info);

	// Param
	info = rwNewT(AnimInterpolatorInfo, 1, MEMDUR_GLOBAL | ID_UVANIMATION);
	info->id = 0x1C1;
	info->interpKeyFrameSize = sizeof(UVAnimInterpFrame);
	info->animKeyFrameSize = sizeof(UVAnimKeyFrame);
	info->customDataSize = sizeof(UVAnimCustomData);
	info->applyCB = uvAnimParamApplyCB;
	info->blendCB = nil;
	info->interpCB = uvAnimParamInterpCB;
	info->addCB = nil;
	info->mulRecipCB = nil;
	info->streamRead = uvAnimStreamRead;
	info->streamWrite = uvAnimStreamWrite;
	info->streamGetSize = uvAnimStreamGetSize;
	AnimInterpolatorInfo::registerInterp(info);
}

int32 uvAnimOffset;

static void*
createUVAnim(void *object, int32 offset, int32)
{
	UVAnim *uvanim;
	uvanim = PLUGINOFFSET(UVAnim, object, offset);
	memset(uvanim, 0, sizeof(*uvanim));
	return object;
}

static void*
destroyUVAnim(void *object, int32 offset, int32)
{
	UVAnim *uvanim;
	uvanim = PLUGINOFFSET(UVAnim, object, offset);
	for(int32 i = 0; i < 8; i++){
		AnimInterpolator *ip = uvanim->interp[i];
		if(ip){
			UVAnimCustomData *custom =
				UVAnimCustomData::get(ip->currentAnim);
			custom->destroy(ip->currentAnim);
			rwFree(ip);
		}
	}
	return object;
}

static void*
copyUVAnim(void *dst, void *src, int32 offset, int32)
{
	UVAnim *srcuvanim, *dstuvanim;
	dstuvanim = PLUGINOFFSET(UVAnim, dst, offset);
	srcuvanim = PLUGINOFFSET(UVAnim, src, offset);
	for(int32 i = 0; i < 8; i++){
		AnimInterpolator *srcip = srcuvanim->interp[i];
		AnimInterpolator *dstip;
		if(srcip){
			Animation *anim = srcip->currentAnim;
			UVAnimCustomData *custom = UVAnimCustomData::get(anim);
			dstip = AnimInterpolator::create(anim->getNumNodes(),
				anim->interpInfo->interpKeyFrameSize);
			dstip->setCurrentAnim(anim);
			custom->refCount++;
			dstuvanim->interp[i] = dstip;
		}
	}
	return dst;
}

Animation*
makeDummyAnimation(const char *name)
{
	AnimInterpolatorInfo *interpInfo = AnimInterpolatorInfo::find(0x1C0);
	Animation *anim = Animation::create(interpInfo, 2, 0, 1.0f);
	UVAnimCustomData *custom = UVAnimCustomData::get(anim);
	strncpy(custom->name, name, 32);
	memset(custom->nodeToUVChannel, 0, sizeof(custom->nodeToUVChannel));
	custom->refCount = 1;
	UVAnimKeyFrame *frames = (UVAnimKeyFrame*)anim->keyframes;
	frames[0].time = 0.0;
	frames[0].prev = nil;
	frames[1].time = 1.0;
	frames[1].prev = &frames[0];
	return anim;
}

static Stream*
readUVAnim(Stream *stream, int32, void *object, int32 offset, int32)
{
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, object, offset);
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	char name[32];
	uint32 mask = stream->readI32();
	uint32 bit = 1;
	for(int32 i = 0; i < 8; i++){
		if(mask & bit){
			stream->read8(name, 32);
			Animation *anim = nil;
			if(currentUVAnimDictionary)
				anim = currentUVAnimDictionary->find(name);
			if(anim == nil){
				anim = makeDummyAnimation(name);
				if(currentUVAnimDictionary)
					currentUVAnimDictionary->add(anim);
			}
			UVAnimCustomData *custom = UVAnimCustomData::get(anim);
			AnimInterpolator *interp;
			interp = AnimInterpolator::create(anim->getNumNodes(),
				anim->interpInfo->interpKeyFrameSize);
			interp->setCurrentAnim(anim);
			custom->refCount++;
			uvanim->interp[i] = interp;
		}
		bit <<= 1;
	}
	// TEMP
	if(uvanim->uv[0] == nil)
		uvanim->uv[0] = Matrix::create();
	if(uvanim->uv[1] == nil)
		uvanim->uv[1] = Matrix::create();
	return stream;
}

static Stream*
writeUVAnim(Stream *stream, int32 size, void *object, int32 offset, int32)
{
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, object, offset);
	writeChunkHeader(stream, ID_STRUCT, size-12);
	uint32 mask = 0;
	uint32 bit = 1;
	for(int32 i = 0; i < 8; i++){
		if(uvanim->interp[i])
			mask |= bit;
		bit <<= 1;
	}
	stream->writeI32(mask);
	for(int32 i = 0; i < 8; i++){
		if(uvanim->interp[i]){
			UVAnimCustomData *custom =
			  UVAnimCustomData::get(uvanim->interp[i]->currentAnim);
			stream->write8(custom->name, 32);
		}
	}
	return stream;
}

static int32
getSizeUVAnim(void *object, int32 offset, int32)
{
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, object, offset);
	int32 size = 0;
	for(int32 i = 0; i < 8; i++)
		if(uvanim->interp[i])
			size += 32;
	return size ? size + 12 + 4 : 0;
}

static void*
uvanimOpen(void *object, int32 offset, int32 size)
{
	registerUVAnimInterpolator();
	return object;
}
static void *uvanimClose(void *object, int32 offset, int32 size) { return object; }

void
registerUVAnimPlugin(void)
{
	Engine::registerPlugin(0, ID_UVANIMATION, uvanimOpen, uvanimClose);
	uvAnimOffset = Material::registerPlugin(sizeof(UVAnim), ID_UVANIMATION,
		createUVAnim, destroyUVAnim, copyUVAnim);
	Material::registerPluginStream(ID_UVANIMATION,
		readUVAnim, writeUVAnim, getSizeUVAnim);
}

bool32 
UVAnim::exists(Material *mat)
{
	int32 i;
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, mat, uvAnimOffset);
	for(i = 0; i < 8; i++)
		if(uvanim->interp[i])
			return 1;
	return 0;
}

void
UVAnim::addTime(Material *mat, float32 t)
{
	int32 i;
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, mat, uvAnimOffset);
	for(i = 0; i < 8; i++)
		if(uvanim->interp[i])
			uvanim->interp[i]->addTime(t);
}

void
UVAnim::applyUpdate(Material *mat)
{
	int32 i, j;
	int32 uv;
	Matrix m;
	UVAnim *uvanim = PLUGINOFFSET(UVAnim, mat, uvAnimOffset);
	for(i = 0; i < 2; i++)
		if(uvanim->uv[i])
			uvanim->uv[i]->setIdentity();
	m.setIdentity();

	for(i = 0; i < 8; i++){
		AnimInterpolator *ip = uvanim->interp[i];
		if(ip == nil)
			continue;
		UVAnimCustomData *custom = UVAnimCustomData::get(ip->currentAnim);
		for(j = 0; j < ip->numNodes; j++){
			InterpFrameHeader *f = ip->getInterpFrame(j);
			uv = custom->nodeToUVChannel[j];
			if(uv < 2 && uvanim->uv[uv]){
				ip->applyCB(&m, f);
				uvanim->uv[uv]->transform(&m, COMBINEPRECONCAT);
			}
		}
	}
	MatFX *mfx = MatFX::get(mat);
	if(mfx) mfx->setUVTransformMatrices(uvanim->uv[0], uvanim->uv[1]);
}

}

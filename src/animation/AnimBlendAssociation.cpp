#include "common.h"

#include "AnimBlendHierarchy.h"
#include "AnimBlendClumpData.h"
#include "RpAnimBlend.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "MemoryMgr.h"

CAnimBlendAssociation::CAnimBlendAssociation(void)
{
	nodes = nil;
	blendAmount = 1.0f;
	blendDelta = 0.0f;
	currentTime = 0.0f;
	speed = 1.0f;
	timeStep = 0.0f;
	animId = -1;
	flags = 0;
	callbackType = CB_NONE;
	link.Init();
}

CAnimBlendAssociation::CAnimBlendAssociation(CAnimBlendAssociation &other)
{
	nodes = nil;
	blendAmount = 1.0f;
	blendDelta = 0.0f;
	currentTime = 0.0f;
	speed = 1.0f;
	timeStep = 0.0f;
	callbackType = CB_NONE;
	link.Init();
	Init(other);
}

CAnimBlendAssociation::~CAnimBlendAssociation(void)
{
	FreeAnimBlendNodeArray();
	link.Remove();
}


void
CAnimBlendAssociation::AllocateAnimBlendNodeArray(int n)
{
	int i;

	nodes = (CAnimBlendNode*)RwMallocAlign(n*sizeof(CAnimBlendNode), 64);
	for(i = 0; i < n; i++)
		nodes[i].Init();
}

void
CAnimBlendAssociation::FreeAnimBlendNodeArray(void)
{
	assert(nodes != nil);
	RwFreeAlign(nodes);
}

void
CAnimBlendAssociation::Init(RpClump *clump, CAnimBlendHierarchy *hier)
{
	int i;
	AnimBlendFrameData *frame;

	CAnimBlendClumpData *clumpData = *RPANIMBLENDCLUMPDATA(clump);
	numNodes = clumpData->numFrames;
	AllocateAnimBlendNodeArray(numNodes);
	for(i = 0; i < numNodes; i++)
		nodes[i].association = this;
	hierarchy = hier;

	// Init every node from a sequence and a Clump frame
	// NB: This is where the order of nodes is defined
	for(i = 0; i < hier->numSequences; i++){
		CAnimBlendSequence *seq = &hier->sequences[i];
		frame = RpAnimBlendClumpFindFrame(clump, seq->name);
		if(frame && seq->numFrames > 0)
			nodes[frame - clumpData->frames].sequence = seq;
	}
}

void
CAnimBlendAssociation::Init(CAnimBlendAssociation &assoc)
{
	int i;

	hierarchy = assoc.hierarchy;
	numNodes = assoc.numNodes;
	flags = assoc.flags;
	animId = assoc.animId;
	AllocateAnimBlendNodeArray(numNodes);
	for(i = 0; i < numNodes; i++){
		nodes[i] = assoc.nodes[i];
		nodes[i].association = this;
	}
}

void
CAnimBlendAssociation::SetBlend(float amount, float delta)
{
	blendAmount = amount;
	blendDelta = delta;
}

void
CAnimBlendAssociation::SetFinishCallback(void (*cb)(CAnimBlendAssociation*, void*), void *arg)
{
	callbackType = CB_FINISH;
	callback = cb;
	callbackArg = arg;
}

void
CAnimBlendAssociation::SetDeleteCallback(void (*cb)(CAnimBlendAssociation*, void*), void *arg)
{
	callbackType = CB_DELETE;
	callback = cb;
	callbackArg = arg;
}

void
CAnimBlendAssociation::SetCurrentTime(float time)
{
	int i;

	for(currentTime = time; currentTime >= hierarchy->totalLength; currentTime -= hierarchy->totalLength)
		if(!IsRepeating())
			return;
	CAnimManager::UncompressAnimation(hierarchy);
	for(i = 0; i < numNodes; i++)
		if(nodes[i].sequence)
			nodes[i].FindKeyFrame(currentTime);
}

void
CAnimBlendAssociation::SyncAnimation(CAnimBlendAssociation *other)
{
	SetCurrentTime(other->currentTime/other->hierarchy->totalLength * hierarchy->totalLength);
}

void
CAnimBlendAssociation::Start(float time)
{
	flags |= ASSOC_RUNNING;
	SetCurrentTime(time);
}

bool
CAnimBlendAssociation::UpdateTime(float timeDelta, float relSpeed)
{
	if(!IsRunning())
		return true;

	timeStep = (flags & ASSOC_MOVEMENT ? relSpeed*hierarchy->totalLength : speed) * timeDelta;
	currentTime += timeStep;

	if(currentTime >= hierarchy->totalLength){
		// Ran past end

		if(IsRepeating())
			currentTime -= hierarchy->totalLength;
		else{
			currentTime = hierarchy->totalLength;
			flags &= ~ASSOC_RUNNING;
			if(flags & ASSOC_FADEOUTWHENDONE){
				flags |= ASSOC_DELETEFADEDOUT;
				blendDelta = -4.0f;
			}
			if(callbackType == CB_FINISH){
				callbackType = CB_NONE;
				callback(this, callbackArg);
			}
		}
	}
	return true;
}

// return whether we still exist after this function
bool
CAnimBlendAssociation::UpdateBlend(float timeDelta)
{
	blendAmount += blendDelta * timeDelta;

	if(blendAmount <= 0.0f && blendDelta < 0.0f){
		// We're faded out and are not fading in
		blendAmount = 0.0f;
		blendDelta = Max(0.0f, blendDelta);
		if(flags & ASSOC_DELETEFADEDOUT){
			if(callbackType == CB_FINISH || callbackType == CB_DELETE)
				callback(this, callbackArg);
			delete this;
			return false;
		}
	}

	if(blendAmount > 1.0f){
		// Maximally faded in, clamp values
		blendAmount = 1.0f;
		blendDelta = Min(0.0f, blendDelta);
	}

	return true;
}

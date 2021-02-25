#pragma once

#include "rtquat.h"

//struct RpHAnimHierarchy;
typedef rw::HAnimHierarchy RpHAnimHierarchy;
//struct RpHAnimAnimation;
typedef rw::Animation RpHAnimAnimation;

#define rpHANIMSTDKEYFRAMETYPEID 0x1

// same as rw::HAnimKeyFrame, but we need RtQuat in this one
struct RpHAnimStdKeyFrame
{
	RpHAnimStdKeyFrame *prevFrame;
	RwReal        time;
	RtQuat           q;
	RwV3d            t;
};
// same story, this one only exists in later RW versions
// but we need it for 64 bit builds because offset and size differs!
struct RpHAnimStdInterpFrame
{
	RpHAnimStdKeyFrame *keyFrame1;
	RpHAnimStdKeyFrame *keyFrame2;
	RtQuat           q;
	RwV3d            t;
};

enum RpHAnimHierarchyFlag
{
	rpHANIMHIERARCHYSUBHIERARCHY =              rw::HAnimHierarchy::SUBHIERARCHY,
	rpHANIMHIERARCHYNOMATRICES =                rw::HAnimHierarchy::NOMATRICES,

	rpHANIMHIERARCHYUPDATEMODELLINGMATRICES = rw::HAnimHierarchy::UPDATEMODELLINGMATRICES,
	rpHANIMHIERARCHYUPDATELTMS =              rw::HAnimHierarchy::UPDATELTMS,
	rpHANIMHIERARCHYLOCALSPACEMATRICES =      rw::HAnimHierarchy::LOCALSPACEMATRICES
};

#define rpHANIMPOPPARENTMATRIX      rw::HAnimHierarchy::POP
#define rpHANIMPUSHPARENTMATRIX     rw::HAnimHierarchy::PUSH

RwBool RpHAnimPluginAttach(void);

RwBool RpHAnimFrameSetID(RwFrame *frame, RwInt32 id);
RwInt32 RpHAnimFrameGetID(RwFrame *frame);

RwInt32 RpHAnimIDGetIndex(RpHAnimHierarchy *hierarchy, RwInt32 ID);

RwBool RpHAnimFrameSetHierarchy(RwFrame *frame, RpHAnimHierarchy *hierarchy);
RpHAnimHierarchy *RpHAnimFrameGetHierarchy(RwFrame *frame);

RpHAnimHierarchy *RpHAnimHierarchySetFlags(RpHAnimHierarchy *hierarchy, RpHAnimHierarchyFlag flags);
RpHAnimHierarchyFlag RpHAnimHierarchyGetFlags(RpHAnimHierarchy *hierarchy);

RwBool RpHAnimHierarchySetCurrentAnim(RpHAnimHierarchy *hierarchy, RpHAnimAnimation *anim);
RwBool RpHAnimHierarchySetCurrentAnimTime(RpHAnimHierarchy *hierarchy, RwReal time);
RwBool RpHAnimHierarchySubAnimTime(RpHAnimHierarchy *hierarchy, RwReal time);
RwBool RpHAnimHierarchyAddAnimTime(RpHAnimHierarchy *hierarchy, RwReal time);

RwMatrix *RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy *hierarchy);
RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy *hierarchy);

#define rpHANIMHIERARCHYGETINTERPFRAME( hierarchy, nodeIndex )    \
        ( (void *)( ( (RwUInt8 *)&(hierarchy->interpolator[1]) +                \
                      ((nodeIndex) *                               \
                       hierarchy->interpolator->currentInterpKeyFrameSize) ) ) )


RpHAnimAnimation *RpHAnimAnimationCreate(RwInt32 typeID, RwInt32 numFrames, RwInt32 flags, RwReal duration);
RpHAnimAnimation  *RpHAnimAnimationDestroy(RpHAnimAnimation *animation);
RpHAnimAnimation  *RpHAnimAnimationStreamRead(RwStream *stream);

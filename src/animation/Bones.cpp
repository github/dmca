#include "common.h"
#include "PedModelInfo.h"
#include "Bones.h"

#ifdef PED_SKIN

int
ConvertPedNode2BoneTag(int node)
{
	switch(node){
	case PED_TORSO:	return BONE_waist;
	case PED_MID:	return BONE_torso;	// this is what Xbox/Mobile use
	//		return BONE_mid;	// this is what PS2/PC use
	case PED_HEAD:		return BONE_head;
	case PED_UPPERARML:	return BONE_upperarml;
	case PED_UPPERARMR:	return BONE_upperarmr;
	case PED_HANDL:		return BONE_Lhand;
	case PED_HANDR:		return BONE_Rhand;
	case PED_UPPERLEGL:	return BONE_upperlegl;
	case PED_UPPERLEGR:	return BONE_upperlegr;
	case PED_FOOTL:		return BONE_footl;
	case PED_FOOTR:		return BONE_footr;
	case PED_LOWERLEGR:	return BONE_lowerlegl;
	}
	return -1;
}

const char*
ConvertBoneTag2BoneName(int tag)
{
	switch(tag){
	case BONE_waist:	return "Swaist";
	case BONE_upperlegr:	return "Supperlegr";
	case BONE_lowerlegr:	return "Slowerlegr";
	case BONE_footr:	return "Sfootr";
	case BONE_upperlegl:	return "Supperlegl";
	case BONE_lowerlegl:	return "Slowerlegl";
	case BONE_footl:	return "Sfootl";
	case BONE_mid:	return "Smid";
	case BONE_torso:	return "Storso";
	case BONE_head:	return "Shead";
	case BONE_upperarmr:	return "Supperarmr";
	case BONE_lowerarmr:	return "Slowerarmr";
	case BONE_Rhand:	return "SRhand";
	case BONE_upperarml:	return "Supperarml";
	case BONE_lowerarml:	return "Slowerarml";
	case BONE_Lhand:	return "SLhand";
	}
	return nil;
}

#endif

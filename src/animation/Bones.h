#pragma once

enum BoneTag
{
	BONE_waist,
	BONE_upperlegr,
	BONE_lowerlegr,
	BONE_footr,
	BONE_upperlegl,
	BONE_lowerlegl,
	BONE_footl,
	BONE_mid,
	BONE_torso,
	BONE_head,
	BONE_upperarmr,
	BONE_lowerarmr,
	BONE_Rhand,
	BONE_upperarml,
	BONE_lowerarml,
	BONE_Lhand,
};

int ConvertPedNode2BoneTag(int node);
const char *ConvertBoneTag2BoneName(int tag);

#pragma once

#include "ClumpModelInfo.h"

class CMloModelInfo : public CClumpModelInfo
{
public:
	float field_34; // draw distance?
	int firstInstance;
	int lastInstance;
public:
	CMloModelInfo(void) : CClumpModelInfo(MITYPE_MLO) {}
	void ConstructClump();
};
#pragma once

#include "ClumpModelInfo.h"

class CXtraCompsModelInfo : public CClumpModelInfo
{
	int field_34;
public:
	CXtraCompsModelInfo(void) : CClumpModelInfo(MITYPE_XTRACOMPS) { field_34 = 0; }
	void SetClump(RpClump*) {};
	void Shutdown(void) {};
};
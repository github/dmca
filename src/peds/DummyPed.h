#pragma once

#include "Dummy.h"

// actually unused
class CDummyPed : CDummy
{
	int32 pedType;
	int32 unknown;
};

VALIDATE_SIZE(CDummyPed, 0x70);

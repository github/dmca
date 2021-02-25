#pragma once

#include "Dummy.h"

class CObject;

class CDummyObject : public CDummy
{
public:
	CDummyObject(void) {}
	CDummyObject(CObject *obj);
};

VALIDATE_SIZE(CDummyObject, 0x68);

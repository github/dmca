#include "common.h"

CRect::CRect(void)
{
	left = 1000000.0f;
	top = 1000000.0f;
	right = -1000000.0f;
	bottom = -1000000.0f;
}

CRect::CRect(float l, float t, float r, float b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
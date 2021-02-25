#pragma once
class CVector;

class CCurves
{
public:
	static float CalcSpeedScaleFactor(CVector*, CVector*, float, float, float, float);
	static void CalcCurvePoint(CVector*, CVector*, CVector*, CVector*, float, int32, CVector*, CVector*);
};

#pragma once

class CPedPlacement {
public:
	static void FindZCoorForPed(CVector* pos);
	static CEntity* IsPositionClearOfCars(Const CVector*);
	static bool IsPositionClearForPed(CVector*);
};
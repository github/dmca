#pragma once

#include "WeaponType.h"

class CEntity;

class CShotInfo
{
public:
	eWeaponType m_weapon;
	CVector m_startPos;
	CVector m_areaAffected;
	float m_radius;
	CEntity *m_sourceEntity;
	float m_timeout;
	bool m_inUse;

	static float ms_afRandTable[20];

	static void Initialise(void);
	static bool AddShot(CEntity*, eWeaponType, CVector, CVector);
	static void Shutdown(void);
	static void Update(void);
};

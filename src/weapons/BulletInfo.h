#pragma once

#include "WeaponType.h"

class CEntity;

class CBulletInfo
{
	eWeaponType m_eWeaponType;
	CEntity* m_pSource;
	float m_fTimer; // big mistake
	bool m_bInUse;
	CVector m_vecPosition;
	CVector m_vecSpeed;
	int16 m_nDamage;
public:
	enum {
		NUM_BULLETS = 100
	};
	static void Initialise(void);
	static void Shutdown(void);
	static bool AddBullet(CEntity* pSource, eWeaponType type, CVector vecPosition, CVector vecSpeed);
	static void Update(void);
	static bool TestForSniperBullet(float x1, float x2, float y1, float y2, float z1, float z2);
};
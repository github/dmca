#pragma once

#include "WeaponType.h"

class CEntity;
class CObject;
class CProjectile;

class CProjectileInfo
{
public:
	eWeaponType m_eWeaponType;
	CEntity *m_pSource;
	uint32 m_nExplosionTime;
	bool m_bInUse;
	CVector m_vecPos;

public:
	static CProjectileInfo *GetProjectileInfo(int32 id);
	static CProjectile *ms_apProjectile[NUM_PROJECTILES];

	static void Initialise();
	static void Shutdown();
	static bool AddProjectile(CEntity *ped, eWeaponType weapon, CVector pos, float speed);
	static void RemoveProjectile(CProjectileInfo *info, CProjectile *projectile);
	static void RemoveNotAdd(CEntity *entity, eWeaponType weaponType, CVector pos);
	static bool RemoveIfThisIsAProjectile(CObject *pObject);
	static void RemoveAllProjectiles();
	static void Update();
	static bool IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool remove);
};

extern CProjectileInfo gaProjectileInfo[NUM_PROJECTILES];
#include "common.h"

#include "Projectile.h"

CProjectile::CProjectile(int32 model) : CObject()
{
	m_fMass = 1.0f;
	m_fTurnMass = 1.0f;
	m_fAirResistance = 0.99999f;
	m_fElasticity = 0.75f;
	m_fBuoyancy = GRAVITY * m_fMass * 0.1f;
	bExplosionProof = true;
	SetModelIndex(model);
	ObjectCreatedBy = MISSION_OBJECT;
}

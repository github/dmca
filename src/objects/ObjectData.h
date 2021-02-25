#pragma once

class CObject;

class CObjectInfo
{
public:
	float m_fMass;
	float m_fTurnMass;
	float m_fAirResistance;
	float m_fElasticity;
	float m_fBuoyancy;
	float m_fUprootLimit;
	float m_fCollisionDamageMultiplier;
	uint8 m_nCollisionDamageEffect;
	uint8 m_nSpecialCollisionResponseCases;
	bool m_bCameraToAvoidThisObject;
};
VALIDATE_SIZE(CObjectInfo, 0x20);

class CObjectData
{
	static CObjectInfo ms_aObjectInfo[NUMOBJECTINFO];
public:
	static void Initialise(const char *filename);
	static void SetObjectData(int32 modelId, CObject &object);
};

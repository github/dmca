#pragma once

enum eCrimeType {
	CRIME_NONE,
	CRIME_POSSESSION_GUN,
	CRIME_HIT_PED,
	CRIME_HIT_COP,
	CRIME_SHOOT_PED,
	CRIME_SHOOT_COP,
	CRIME_STEAL_CAR,
	CRIME_RUN_REDLIGHT,
	CRIME_RECKLESS_DRIVING,
	CRIME_SPEEDING,
	CRIME_RUNOVER_PED,
	CRIME_RUNOVER_COP,
	CRIME_SHOOT_HELI,
	CRIME_PED_BURNED,
	CRIME_COP_BURNED,
	CRIME_VEHICLE_BURNED,
	CRIME_DESTROYED_CESSNA,
	NUM_CRIME_TYPES
};

class CCrimeBeingQd
{
public:
	eCrimeType m_nType;
	uint32 m_nId;
	uint32 m_nTime;
	CVector m_vecPosn;
	bool m_bReported;
	bool m_bPoliceDoesntCare;

	CCrimeBeingQd() { };
	~CCrimeBeingQd() { };
};

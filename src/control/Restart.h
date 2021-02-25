#pragma once

#define NUM_RESTART_POINTS 8

class CRestart
{
public:
	static void AddPoliceRestartPoint(const CVector&, float);
	static void AddHospitalRestartPoint(const CVector&, float);
	static void OverrideNextRestart(const CVector&, float);

	static void FindClosestHospitalRestartPoint(const CVector &, CVector *, float *);
	static void FindClosestPoliceRestartPoint(const CVector &, CVector *, float *);
	static void Initialise();
	static void CancelOverrideRestart();

	static void LoadAllRestartPoints(uint8 *buf, uint32 size);
	static void SaveAllRestartPoints(uint8 *buf, uint32 *size);

	static uint8 OverrideHospitalLevel;
	static uint8 OverridePoliceStationLevel;
	static bool bFadeInAfterNextArrest;
	static bool bFadeInAfterNextDeath;

	static bool bOverrideRestart;
	static CVector OverridePosition;
	static float OverrideHeading;

	static CVector HospitalRestartPoints[NUM_RESTART_POINTS];
	static float HospitalRestartHeadings[NUM_RESTART_POINTS];
	static uint16 NumberOfHospitalRestarts;

	static CVector PoliceRestartPoints[NUM_RESTART_POINTS];
	static float PoliceRestartHeadings[NUM_RESTART_POINTS];
	static uint16 NumberOfPoliceRestarts;
};

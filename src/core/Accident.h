#pragma once
#include "config.h"

class CPed;

class CAccident
{
public:
	CPed *m_pVictim;
	uint32 m_nMedicsAttending;
	uint32 m_nMedicsPerformingCPR;
	CAccident() : m_pVictim(nil), m_nMedicsAttending(0), m_nMedicsPerformingCPR(0) {}
};

class CAccidentManager
{
	CAccident m_aAccidents[NUM_ACCIDENTS];
	enum {
		MAX_MEDICS_TO_ATTEND_ACCIDENT = 2
	};
public:
	CAccident *GetNextFreeAccident();
	void ReportAccident(CPed *ped);
	void Update();
	CAccident *FindNearestAccident(CVector vecPos, float *pDistance);
	uint16 CountActiveAccidents();
	bool UnattendedAccidents();
	bool WorkToDoForMedics();
};

extern CAccidentManager gAccidentManager;
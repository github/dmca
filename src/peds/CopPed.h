#pragma once
#include "Ped.h"

enum eCopType
{
	COP_STREET = 0,
	COP_FBI = 1,
	COP_SWAT = 2,
	COP_ARMY = 3,
};

class CCopPed : public CPed
{
public:
	int16 m_nRoadblockNode;
	float m_fDistanceToTarget;
	bool m_bIsInPursuit;
	bool m_bIsDisabledCop;
	int8 field_1350;
	bool m_bBeatingSuspect;
	bool m_bStopAndShootDisabledZone;
	bool m_bZoneDisabled;
	float m_fAbseilPos; // VC leftover, unused
	eCopType m_nCopType;
	int8 field_1364;

	CCopPed(eCopType);
	~CCopPed();

	void ClearPursuit(void);
	void ProcessControl(void);
	void SetArrestPlayer(CPed*);
	void SetPursuit(bool);
	void ArrestPlayer(void);
	void ScanForCrimes(void);
	void CopAI(void);
};

#ifndef PED_SKIN
VALIDATE_SIZE(CCopPed, 0x558);
#endif

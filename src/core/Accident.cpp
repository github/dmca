#include "common.h"

#include "Accident.h"

#include "Ped.h"
#include "Pools.h"
#include "World.h"

CAccidentManager gAccidentManager;

CAccident*
CAccidentManager::GetNextFreeAccident()
{
	for (int i = 0; i < NUM_ACCIDENTS; i++) {
		if (m_aAccidents[i].m_pVictim == nil)
			return &m_aAccidents[i];
	}

	return nil;
}

void
CAccidentManager::ReportAccident(CPed *ped)
{
	if (!ped->IsPlayer() && ped->CharCreatedBy != MISSION_CHAR && !ped->bRenderScorched && !ped->bBodyPartJustCameOff && ped->bAllowMedicsToReviveMe && !ped->bIsInWater) {
		for (int i = 0; i < NUM_ACCIDENTS; i++) {
			if (m_aAccidents[i].m_pVictim != nil && m_aAccidents[i].m_pVictim == ped)
				return;
		}

		if (ped->m_pCurrentPhysSurface == nil) {
			CVector point = ped->GetPosition();
			point.z -= 2.0f;

			CColPoint colPoint;
			CEntity *pEntity; 

			if (!CWorld::ProcessVerticalLine(point, -100.0f, colPoint, pEntity, true, false, false, false, false, false, nil)) {
				CAccident *accident = GetNextFreeAccident();
				if (accident != nil) {
					accident->m_pVictim = ped;
					ped->RegisterReference((CEntity**)&accident->m_pVictim);
					accident->m_nMedicsPerformingCPR = 0;
					accident->m_nMedicsAttending = 0;
					ped->m_lastAccident = accident;
					WorkToDoForMedics();
				}
			}
		}
	}
}

void
CAccidentManager::Update()
{
#ifdef SQUEEZE_PERFORMANCE
	// Handled after injury registered.
	return;
#endif
	int32 e;
	if (CEventList::GetEvent(EVENT_INJURED_PED, &e)) {
		CPed *ped = CPools::GetPed(gaEvent[e].entityRef);
		if (ped) {
			ReportAccident(ped);
			CEventList::ClearEvent(e);
		}
	}
}

CAccident*
CAccidentManager::FindNearestAccident(CVector vecPos, float *pDistance)
{
	for (int i = 0; i < MAX_MEDICS_TO_ATTEND_ACCIDENT; i++){
		int accidentId = -1;
		float minDistance = 999999;
		for (int j = 0; j < NUM_ACCIDENTS; j++){
			CPed* pVictim = m_aAccidents[j].m_pVictim;
			if (!pVictim)
				continue;
			if (pVictim->CharCreatedBy == MISSION_CHAR)
				continue;
			if (pVictim->m_fHealth != 0.0f)
				continue;
			if (m_aAccidents[j].m_nMedicsPerformingCPR != i)
				continue;
			float distance = (pVictim->GetPosition() - vecPos).Magnitude2D();
			if (distance / 2 > pVictim->GetPosition().z - vecPos.z && distance < minDistance){
				minDistance = distance;
				accidentId = j;
			}
		}
		*pDistance = minDistance;
		if (accidentId != -1)
			return &m_aAccidents[accidentId];
	}
	return nil;
}

uint16
CAccidentManager::CountActiveAccidents()
{
	uint16 accidents = 0;
	for (int i = 0; i < NUM_ACCIDENTS; i++) {
		if (m_aAccidents[i].m_pVictim)
			accidents++;
	}
	return accidents;
}

bool
CAccidentManager::WorkToDoForMedics()
{
	for (int i = 0; i < NUM_ACCIDENTS; i++) {
		if (m_aAccidents[i].m_pVictim != nil && m_aAccidents[i].m_nMedicsAttending < MAX_MEDICS_TO_ATTEND_ACCIDENT)
			return true;
	}
	return false;
}

bool
CAccidentManager::UnattendedAccidents()
{
	for (int i = 0; i < NUM_ACCIDENTS; i++) {
		if (m_aAccidents[i].m_pVictim != nil && m_aAccidents[i].m_nMedicsAttending == 0)
			return true;
	}
	return false;
}

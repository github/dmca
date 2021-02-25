#include "common.h"
#include "Profile.h"

#ifndef MASTER
float CProfile::ms_afStartTime[NUM_PROFILES];
float CProfile::ms_afCumulativeTime[NUM_PROFILES];
float CProfile::ms_afEndTime[NUM_PROFILES];
float CProfile::ms_afMaxEndTime[NUM_PROFILES];
float CProfile::ms_afMaxCumulativeTime[NUM_PROFILES];
Const char *CProfile::ms_pProfileString[NUM_PROFILES];
RwRGBA CProfile::ms_aBarColours[NUM_PROFILES];

void CProfile::Initialise()
{
	ms_afMaxEndTime[PROFILE_FRAME_RATE] = 0.0f;
	ms_afMaxEndTime[PROFILE_PHYSICS] = 0.0f;
	ms_afMaxEndTime[PROFILE_COLLISION] = 0.0f;
	ms_afMaxEndTime[PROFILE_PED_AI] = 0.0f;
	ms_afMaxEndTime[PROFILE_PROCESSING_TIME] = 0.0f;
	ms_afMaxEndTime[PROFILE_RENDERING_TIME] = 0.0f;
	ms_afMaxEndTime[PROFILE_TOTAL] = 0.0f;

	ms_pProfileString[PROFILE_FRAME_RATE] = "Frame rate";
	ms_pProfileString[PROFILE_PHYSICS] = "Physics";
	ms_pProfileString[PROFILE_COLLISION] = "Collision";
	ms_pProfileString[PROFILE_PED_AI] = "Ped AI";
	ms_pProfileString[PROFILE_PROCESSING_TIME] = "Processing time";
	ms_pProfileString[PROFILE_RENDERING_TIME] = "Rendering time";
	ms_pProfileString[PROFILE_TOTAL] = "Total";

	ms_afMaxCumulativeTime[PROFILE_FRAME_RATE] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_PHYSICS] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_COLLISION] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_PED_AI] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_PROCESSING_TIME] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_RENDERING_TIME] = 0.0f;
	ms_afMaxCumulativeTime[PROFILE_TOTAL] = 0.0f;

	ms_aBarColours[PROFILE_PHYSICS] = { 0, 127, 255, 255 };
	ms_aBarColours[PROFILE_COLLISION] = { 0, 255, 255, 255 };
	ms_aBarColours[PROFILE_PED_AI] = { 255, 0, 0, 255 };
	ms_aBarColours[PROFILE_PROCESSING_TIME] = { 0, 255, 0, 255 };
	ms_aBarColours[PROFILE_RENDERING_TIME] = { 0, 0, 255, 255 };
	ms_aBarColours[PROFILE_TOTAL] = { 255, 255, 255, 255 };
}

void CProfile::SuspendProfile(eProfile profile)
{
	ms_afEndTime[profile] = -ms_afStartTime[profile];
	ms_afCumulativeTime[profile] -= ms_afStartTime[profile];
}

void CProfile::ShowResults()
{
	ms_afMaxEndTime[PROFILE_FRAME_RATE] = Max(ms_afMaxEndTime[PROFILE_FRAME_RATE], ms_afEndTime[PROFILE_FRAME_RATE]);
	ms_afMaxEndTime[PROFILE_PHYSICS] = Max(ms_afMaxEndTime[PROFILE_PHYSICS], ms_afEndTime[PROFILE_PHYSICS]);
	ms_afMaxEndTime[PROFILE_COLLISION] = Max(ms_afMaxEndTime[PROFILE_COLLISION], ms_afEndTime[PROFILE_COLLISION]);
	ms_afMaxEndTime[PROFILE_PED_AI] = Max(ms_afMaxEndTime[PROFILE_PED_AI], ms_afEndTime[PROFILE_PED_AI]);
	ms_afMaxEndTime[PROFILE_PROCESSING_TIME] = Max(ms_afMaxEndTime[PROFILE_PROCESSING_TIME], ms_afEndTime[PROFILE_PROCESSING_TIME]);
	ms_afMaxEndTime[PROFILE_RENDERING_TIME] = Max(ms_afMaxEndTime[PROFILE_RENDERING_TIME], ms_afEndTime[PROFILE_RENDERING_TIME]);
	ms_afMaxEndTime[PROFILE_TOTAL] = Max(ms_afMaxEndTime[PROFILE_TOTAL], ms_afEndTime[PROFILE_TOTAL]);

	ms_afMaxCumulativeTime[PROFILE_FRAME_RATE] = Max(ms_afMaxCumulativeTime[PROFILE_FRAME_RATE], ms_afCumulativeTime[PROFILE_FRAME_RATE]);
	ms_afMaxCumulativeTime[PROFILE_PHYSICS] = Max(ms_afMaxCumulativeTime[PROFILE_PHYSICS], ms_afCumulativeTime[PROFILE_PHYSICS]);
	ms_afMaxCumulativeTime[PROFILE_COLLISION] = Max(ms_afMaxCumulativeTime[PROFILE_COLLISION], ms_afCumulativeTime[PROFILE_COLLISION]);
	ms_afMaxCumulativeTime[PROFILE_PED_AI] = Max(ms_afMaxCumulativeTime[PROFILE_PED_AI], ms_afCumulativeTime[PROFILE_PED_AI]);
	ms_afMaxCumulativeTime[PROFILE_PROCESSING_TIME] = Max(ms_afMaxCumulativeTime[PROFILE_PROCESSING_TIME], ms_afCumulativeTime[PROFILE_PROCESSING_TIME]);
	ms_afMaxCumulativeTime[PROFILE_RENDERING_TIME] = Max(ms_afMaxCumulativeTime[PROFILE_RENDERING_TIME], ms_afCumulativeTime[PROFILE_RENDERING_TIME]);
	ms_afMaxCumulativeTime[PROFILE_TOTAL] = Max(ms_afMaxCumulativeTime[PROFILE_TOTAL], ms_afCumulativeTime[PROFILE_TOTAL]);
}
#endif

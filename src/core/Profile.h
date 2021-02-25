#pragma once

enum eProfile
{
	PROFILE_FRAME_RATE,
	PROFILE_PHYSICS,
	PROFILE_COLLISION,
	PROFILE_PED_AI,
	PROFILE_PROCESSING_TIME,
	PROFILE_RENDERING_TIME,
	PROFILE_TOTAL,
	NUM_PROFILES,
};

class CProfile
{
	static float ms_afStartTime[NUM_PROFILES];
	static float ms_afCumulativeTime[NUM_PROFILES];
	static float ms_afEndTime[NUM_PROFILES];
	static float ms_afMaxEndTime[NUM_PROFILES];
	static float ms_afMaxCumulativeTime[NUM_PROFILES];
	static Const char *ms_pProfileString[NUM_PROFILES];
	static RwRGBA ms_aBarColours[NUM_PROFILES];
public:
	static void Initialise();
	static void SuspendProfile(eProfile profile);
	static void ShowResults();
};

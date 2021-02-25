/*******************************************************************\
*																	*
*  EAX-UTIL.H - utilities for Environmental Audio Extensions v. 3.0	*
*				Definitions of the Original 26 EAX Presets			*
*				Definitions for some new EAX Presets				*
*				Definitions of some Material Presets				*
*				Function declaration for EAX Morphing				*
*																	*
\*******************************************************************/

#ifndef EAXUTIL_INCLUDED
#define EAXUTIL_INCLUDED

#include <eax.h>

/***********************************************************************************************
* Function	:	EAX3ListenerInterpolate
* Params	:	lpStart			- Initial EAX 3 Listener parameters
*			:	lpFinish		- Final EAX 3 Listener parameters
*			:	flRatio			- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
*			:	lpResult		- Interpolated EAX 3 Listener parameters
*			:	bCheckValues	- Check EAX 3.0 parameters are in range,
								- default == false (no checking)
************************************************************************************************/
bool EAX3ListenerInterpolate(EAXLISTENERPROPERTIES *lpStartEAX3LP, EAXLISTENERPROPERTIES *lpFinishEAX3LP,
			float flRatio, EAXLISTENERPROPERTIES *lpResultEAX3LP, bool bCheckValues = false);


/***********************************************************************************************\
*
* Legacy environment presets for use with DSPROPERTY_EAXLISTENER_ALLPARAMETERS.
* Each array conforms to the DSPROPSETID_EAX30_ListenerProperties structure defined in EAX.H.
*
************************************************************************************************/

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_GENERIC \
	{0,		7.5f,	1.000f,	-1000,	-100,	0,		1.49f,	0.83f,	1.00f,	-2602,	0.007f,	0.00f,0.00f,0.00f,	200,	0.011f,		0.00f,0.00f,0.00f,	0.250f,	0.000f,	0.250f,	0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_PADDEDCELL \
	{1,		1.4f,	1.000f,	-1000,	-6000,	0,		0.17f,	0.10f,	1.00f,	-1204,	0.001f, 0.00f,0.00f,0.00f,  207,	0.002f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_ROOM \
	{2,		1.9f,	1.000f,	-1000,	-454,	0,		0.40f,	0.83f,	1.00f,  -1646,	0.002f, 0.00f,0.00f,0.00f,	53,		0.003f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_BATHROOM \
	{3,		1.4f,	1.000f,	-1000,  -1200,	0,		1.49f,	0.54f,	1.00f,  -370,	0.007f, 0.00f,0.00f,0.00f,	1030,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_LIVINGROOM \
	{4,		2.5f,	1.000f,	-1000,  -6000,	0,		0.50f,	0.10f,	1.00f,  -1376,	0.003f, 0.00f,0.00f,0.00f,	-1104,	0.004f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_STONEROOM \
	{5,		11.6f,	1.000f,  -1000, -300,	0,		2.31f,	0.64f,	1.00f,	-711,	0.012f, 0.00f,0.00f,0.00f,	83,		0.017f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_AUDITORIUM \
	{6,		21.6f,	1.000f,  -1000,	-476,	0,		4.32f,	0.59f,	1.00f,	-789,	0.020f, 0.00f,0.00f,0.00f,	-289,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_CONCERTHALL \
	{7,		19.6f,	1.000f,  -1000,	-500,	0,		3.92f,	0.70f,	1.00f,  -1230,	0.020f, 0.00f,0.00f,0.00f,  -02,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CAVE \
	{8,		14.6f,	1.000f,  -1000,	0,		0,		2.91f,	1.30f,	1.00f,  -602,	0.015f, 0.00f,0.00f,0.00f,	-302,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x1f }
#define EAX30_PRESET_ARENA \
	{9,		36.2f,	1.000f,  -1000,	-698,	0,		7.24f,	0.33f,	1.00f,  -1166,	0.020f, 0.00f,0.00f,0.00f,  16,		0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_HANGAR \
	{10,	50.3f,	1.000f,  -1000,	-1000,	0,		10.05f, 0.23f,	1.00f,  -602,	0.020f, 0.00f,0.00f,0.00f,  198,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CARPETTEDHALLWAY \
	{11,	1.9f,	1.000f,	-1000,	-4000,	0,		0.30f,	0.10f,	1.00f,  -1831,	0.002f, 0.00f,0.00f,0.00f,	-1630,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_HALLWAY \
	{12,	1.8f,	1.000f,	-1000,	-300,	0,		1.49f,	0.59f,	1.00f,  -1219,	0.007f, 0.00f,0.00f,0.00f,  441,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_STONECORRIDOR \
	{13,	13.5f,	1.000f,	-1000,	-237,	0,		2.70f,	0.79f,	1.00f,  -1214,	0.013f, 0.00f,0.00f,0.00f,  395,	0.020f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_ALLEY \
	{14,	7.5f,	0.300f,	-1000,	-270,	0,		1.49f,	0.86f,	1.00f,  -1204,	0.007f, 0.00f,0.00f,0.00f,  -4,		0.011f,		0.00f,0.00f,0.00f,	0.125f, 0.950f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_FOREST \
	{15,	38.0f,	0.300f,	-1000,	-3300,	0,		1.49f,	0.54f,	1.00f,  -2560,	0.162f, 0.00f,0.00f,0.00f,	-229,	0.088f,		0.00f,0.00f,0.00f,	0.125f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CITY \
	{16,	7.5f,	0.500f,	-1000,	-800,	0,		1.49f,	0.67f,	1.00f,  -2273,	0.007f, 0.00f,0.00f,0.00f,	-1691,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_MOUNTAINS \
	{17,	100.0f, 0.270f,	-1000,	-2500,	0,		1.49f,	0.21f,	1.00f,  -2780,	0.300f, 0.00f,0.00f,0.00f,	-1434,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_QUARRY \
	{18,	17.5f,	1.000f,	-1000,	-1000,	0,		1.49f,	0.83f,	1.00f,	-10000, 0.061f, 0.00f,0.00f,0.00f,  500,	0.025f,		0.00f,0.00f,0.00f,	0.125f, 0.700f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_PLAIN \
	{19,	42.5f,	0.210f,	-1000,	-2000,	0,		1.49f,	0.50f,	1.00f,  -2466,	0.179f, 0.00f,0.00f,0.00f,	-1926,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_PARKINGLOT \
	{20,	8.3f,	1.000f,	-1000,	0,		0,		1.65f,	1.50f,	1.00f,  -1363,	0.008f, 0.00f,0.00f,0.00f,	-1153,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_SEWERPIPE \
	{21,	1.7f,	0.800f,	-1000,	-1000,	0,		2.81f,	0.14f,	1.00f,	429,	0.014f, 0.00f,0.00f,0.00f,	1023,	0.021f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_UNDERWATER \
	{22,	1.8f,	1.000f,	-1000,  -4000,	0,		1.49f,	0.10f,	1.00f,  -449,	0.007f, 0.00f,0.00f,0.00f,	1700,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 1.180f, 0.348f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_DRUGGED \
	{23,	1.9f,	0.500f,	-1000,	0,		0,		8.39f,	1.39f,	1.00f,  -115,	0.002f, 0.00f,0.00f,0.00f,  985,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_DIZZY \
	{24,	1.8f,	0.600f,	-1000,	-400,	0,		17.23f, 0.56f,	1.00f,  -1713,	0.020f, 0.00f,0.00f,0.00f,	-613,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.810f, 0.310f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_PSYCHOTIC \
	{25,	1.0f,	0.500f,	-1000,	-151,	0,		7.56f,	0.91f,	1.00f,  -626,	0.020f, 0.00f,0.00f,0.00f,  774,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 4.000f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }


/***********************************************************************************************\
*
* New environment presets for use with DSPROPERTY_EAXLISTENER_ALLPARAMETERS.
* Each array conforms to the DSPROPSETID_EAX30_ListenerProperties structure defined in EAX.H.
*
************************************************************************************************/

// STANDARDISED-LOCATION SCENARIOS

// CASTLE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_CASTLE_SMALLROOM \
	{ 26,   8.3f,	0.890f,	-1100,	-800,	-2000,	1.22f,	0.83f,	0.31f,	-100,	0.022f,	0.00f,0.00f,0.00f,	0,		0.011f,		0.00f,0.00f,0.00f,	0.138f,	0.080f,	0.250f,	0.000f,	-5.0f,	5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_SHORTPASSAGE \
	{ 26,   8.3f,	0.890f, -1000,  -1000,  -2000,  2.32f,	0.83f,	0.31f,	-100,	0.007f, 0.00f,0.00f,0.00f,  -500,	0.023f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_MEDIUMROOM \
	{ 26,   8.3f,	0.930f, -1000,  -1100,  -2000,  2.04f,	0.83f,	0.46f,  -300,	0.022f, 0.00f,0.00f,0.00f,	-200,	0.011f,		0.00f,0.00f,0.00f,	0.155f, 0.030f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_LONGPASSAGE \
	{ 26,   8.3f,	0.890f, -1000,  -800,	-2000,  3.42f,	0.83f,	0.31f,  -200,	0.007f, 0.00f,0.00f,0.00f,	-600,	0.023f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_LARGEROOM \
	{ 26,   8.3f,	0.820f, -1000,  -1100,  -1800,  2.53f,	0.83f,	0.50f,  -900,	0.034f, 0.00f,0.00f,0.00f,	-400,	0.016f,		0.00f,0.00f,0.00f,	0.185f, 0.070f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_HALL \
	{ 26,   8.3f,	0.810f, -1000,  -1100,  -1500,  3.14f,	0.79f,	0.62f,  -1300,	0.056f, 0.00f,0.00f,0.00f,	-500,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_CUPBOARD \
	{ 26,   8.3f,	0.890f, -1000,  -1100,  -2000,  0.67f,	0.87f,	0.31f,  300,	0.010f,	0.00f,0.00f,0.00f,	300,	0.007f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAX30_PRESET_CASTLE_COURTYARD \
	{ 26,   8.3f,	0.420f, -1100,  -700,   -900,	2.13f,	0.61f,	0.23f,  -2300,	0.112f, 0.00f,0.00f,0.00f,	-1500,	0.036f,		0.00f,0.00f,0.00f,	0.250f, 0.370f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x1f }
#define EAX30_PRESET_CASTLE_ALCOVE \
	{ 26,   8.3f,	0.890f,	-1000,  -600,	-2000,  1.64f,	0.87f,	0.31f,  -100,	0.007f, 0.00f,0.00f,0.00f,	-500,	0.034f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,	5168.6f,	139.5f,  0.00f, 0x20 }


// FACTORY PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_FACTORY_ALCOVE \
	{ 26,   1.8f,	0.590f,  -1200, -200,   -600,	3.14f,	0.65f,	1.31f,  300,	0.010f, 0.00f,0.00f,0.00f,	-1200,	0.038f,		0.00f,0.00f,0.00f,	0.114f, 0.100f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_SHORTPASSAGE \
	{ 26,   1.8f,	0.640f,  -1200, -200,   -600,	2.53f,	0.65f,	1.31f,  0,		0.010f, 0.00f,0.00f,0.00f,	-600,	0.038f,		0.00f,0.00f,0.00f,	0.135f, 0.230f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_MEDIUMROOM \
	{ 26,   1.9f,	0.820f,  -1200, -200,   -600,	2.76f,	0.65f,	1.31f,  -1100,	0.022f, 0.00f,0.00f,0.00f,	-400,	0.023f,		0.00f,0.00f,0.00f,	0.174f, 0.070f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_LONGPASSAGE \
	{ 26,   1.8f,	0.640f,  -1200, -200,   -600,	4.06f,	0.65f,	1.31f,  0,		0.020f, 0.00f,0.00f,0.00f,	-900,	0.037f,		0.00f,0.00f,0.00f,	0.135f, 0.230f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_LARGEROOM \
	{ 26,   1.9f,	0.750f,  -1200, -300,   -400,	4.24f,	0.51f,	1.31f,  -1500,	0.039f, 0.00f,0.00f,0.00f,	-600,	0.023f,		0.00f,0.00f,0.00f,	0.231f, 0.070f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_HALL \
	{ 26,   1.9f,	0.750f,  -1000, -300,   -400,	7.43f,	0.51f,	1.31f,  -2400,	0.073f, 0.00f,0.00f,0.00f,	-500,	0.027f,		0.00f,0.00f,0.00f,	0.250f, 0.070f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_CUPBOARD \
	{ 26,   1.7f,	0.630f,  -1200, -200,   -600,	0.49f,	0.65f,	1.31f,  200,	0.010f, 0.00f,0.00f,0.00f,	200,	0.032f,		0.00f,0.00f,0.00f,	0.107f, 0.070f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_COURTYARD \
	{ 26,   1.7f,	0.570f,  -1000, -1000,  -400,	2.32f,	0.29f,	0.56f,  -2400,	0.090f, 0.00f,0.00f,0.00f,	-2000,	0.039f,		0.00f,0.00f,0.00f,	0.250f, 0.290f, 0.250f, 0.000f, -0.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAX30_PRESET_FACTORY_SMALLROOM \
	{ 26,   1.8f,	0.820f,  -1200,	-200,   -600,	1.72f,	0.65f,	1.31f,  -300,	0.010f, 0.00f,0.00f,0.00f,	-200,	0.024f,		0.00f,0.00f,0.00f,	0.119f, 0.070f, 0.250f, 0.000f, -0.0f,	3762.6f,	362.5f,  0.00f, 0x20 }

// ICE PALACE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_ICEPALACE_ALCOVE \
	{ 26,   2.7f,	0.840f, -1000,  -500,	-1100,  2.76f,	1.46f,	0.28f,  100,	0.010f, 0.00f,0.00f,0.00f,	-1200,	0.030f,		0.00f,0.00f,0.00f,	0.161f, 0.090f, 0.250f, 0.000f,	-0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_SHORTPASSAGE \
	{ 26,   2.7f,	0.750f, -1000,  -500,	-1100,  1.79f,	1.46f,	0.28f,  -600,	0.010f, 0.00f,0.00f,0.00f,	-700,	0.019f,		0.00f,0.00f,0.00f,	0.177f, 0.090f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_MEDIUMROOM \
	{ 26,   2.7f,	0.870f, -1000,  -500,   -700,	2.22f,	1.53f,	0.32f,  -800,	0.039f, 0.00f,0.00f,0.00f,	-1200,	0.027f,		0.00f,0.00f,0.00f,	0.186f, 0.120f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_LONGPASSAGE \
	{ 26,   2.7f,	0.770f, -1000,  -500,   -800,	3.01f,	1.46f,	0.28f,  -200,	0.012f, 0.00f,0.00f,0.00f,	-800,	0.025f,		0.00f,0.00f,0.00f,	0.186f, 0.040f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_LARGEROOM \
	{ 26,   2.9f,	0.810f, -1000,  -500,   -700,	3.14f,	1.53f,	0.32f,  -1200,	0.039f, 0.00f,0.00f,0.00f,	-1300,	0.027f,		0.00f,0.00f,0.00f,	0.214f, 0.110f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_HALL \
	{ 26,   2.9f,	0.760f, -1000,  -700,   -500,	5.49f,	1.53f,	0.38f,  -1900,	0.054f, 0.00f,0.00f,0.00f,	-1400,	0.052f,		0.00f,0.00f,0.00f,	0.226f, 0.110f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_CUPBOARD \
	{ 26,   2.7f,	0.830f, -1000,  -600,	-1300,  0.76f,	1.53f,	0.26f,  100,	0.012f, 0.00f,0.00f,0.00f,	100,	0.016f,		0.00f,0.00f,0.00f,	0.143f, 0.080f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_COURTYARD \
	{ 26,   2.9f,	0.590f, -1000,  -1100,  -1000,  2.04f,	1.20f,	0.38f,  -2000,	0.073f, 0.00f,0.00f,0.00f,	-2200,	0.043f,		0.00f,0.00f,0.00f,	0.235f, 0.480f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAX30_PRESET_ICEPALACE_SMALLROOM \
	{ 26,   2.7f,	0.840f, -1000,  -500,	-1100,  1.51f,	1.53f,	0.27f,	-100,	0.010f, 0.00f,0.00f,0.00f,	-900,	0.011f,		0.00f,0.00f,0.00f,	0.164f, 0.140f, 0.250f, 0.000f, -0.0f,	12428.5f,	99.6f,  0.00f,	0x20 }

// SPACE STATION PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_SPACESTATION_ALCOVE \
	{ 26,   1.5f,	0.780f, -1100,  -300,   -100,	1.16f,	0.81f,	0.55f,  300,	0.007f, 0.00f,0.00f,0.00f,	-500,	0.018f,		0.00f,0.00f,0.00f,	0.192f, 0.210f, 0.250f, 0.000f,	-5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_MEDIUMROOM \
	{ 26,   1.5f,	0.750f, -1000,  -400,   -100,	3.01f,	0.50f,	0.55f,  -1000,	0.034f, 0.00f,0.00f,0.00f,	-700,	0.035f,		0.00f,0.00f,0.00f,	0.209f, 0.310f, 0.250f, 0.000f,	-5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_SHORTPASSAGE \
	{ 26,   1.5f,	0.870f, -1000,  -400,   -100,	3.57f,	0.50f,	0.55f,  0,		0.012f, 0.00f,0.00f,0.00f,	-600,	0.016f,		0.00f,0.00f,0.00f,	0.172f, 0.200f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_LONGPASSAGE \
	{ 26,   1.9f,	0.820f, -1000,  -400,   -100,	4.62f,	0.62f,	0.55f,  0,		0.012f, 0.00f,0.00f,0.00f,	-800,	0.031f,		0.00f,0.00f,0.00f,	0.250f, 0.230f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_LARGEROOM \
	{ 26,   1.8f,	0.810f, -1000,  -400,   -100,	3.89f,	0.38f,	0.61f,  -1200,	0.056f, 0.00f,0.00f,0.00f,	-800,	0.035f,		0.00f,0.00f,0.00f,	0.233f, 0.280f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_HALL \
	{ 26,   1.9f,	0.870f, -1000,  -400,   -100,	7.11f,	0.38f,	0.61f,  -1500,	0.100f, 0.00f,0.00f,0.00f,	-1000,	0.047f,		0.00f,0.00f,0.00f,	0.250f, 0.250f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_CUPBOARD \
	{ 26,   1.4f,	0.560f, -1000,  -300,   -100,	0.79f,	0.81f,	0.55f,  200,	0.007f, 0.00f,0.00f,0.00f,	400,	0.018f,		0.00f,0.00f,0.00f,	0.181f, 0.310f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPACESTATION_SMALLROOM \
	{ 26,   1.5f,	0.700f, -1000,  -300,   -100,	1.72f,	0.82f,	0.55f,	-400,	0.007f, 0.00f,0.00f,0.00f,	-500,	0.013f,		0.00f,0.00f,0.00f,	0.188f, 0.260f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }

// WOODEN GALLEON PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_WOODEN_ALCOVE \
	{ 26,   7.5f,	1.000f, -1100,  -1800,  -1000,  1.22f,	0.62f,	0.91f,	-100,	0.012f, 0.00f,0.00f,0.00f,	-600,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_SHORTPASSAGE \
	{ 26,   7.5f,	1.000f, -1100,  -1800,  -1000,  1.45f,	0.50f,	0.87f,	-300,	0.012f, 0.00f,0.00f,0.00f,	-700,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_MEDIUMROOM \
	{ 26,   7.5f,	1.000f, -1200,  -2000,  -1100,  1.07f,	0.42f,	0.82f,	-300,	0.039f, 0.00f,0.00f,0.00f,	-400,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_LONGPASSAGE \
	{ 26,   7.5f,	1.000f, -1100,  -2000,  -1000,  1.79f,	0.40f,	0.79f,	-200,	0.020f, 0.00f,0.00f,0.00f,	-1000,	0.036f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_LARGEROOM \
	{ 26,   7.5f,	1.000f, -1200,  -2100,  -1100,  1.45f,	0.33f,	0.82f,	-300,	0.056f, 0.00f,0.00f,0.00f,	-500,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_HALL \
	{ 26,   7.5f,	1.000f, -1200,  -2200,  -1100,  1.95f,	0.30f,	0.82f,	-300,	0.068f, 0.00f,0.00f,0.00f,	-500,	0.063f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_CUPBOARD \
	{ 26,   7.5f,	1.000f, -1000,  -1700,  -1000,  0.56f,	0.46f,	0.91f,	-100,	0.012f, 0.00f,0.00f,0.00f,	-100,	0.028f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_SMALLROOM \
	{ 26,   7.5f,	1.000f, -1200,  -1900,  -1000,  0.79f,	0.32f,	0.87f,	-200,	0.032f, 0.00f,0.00f,0.00f,	-300,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAX30_PRESET_WOODEN_COURTYARD \
	{ 26,   7.5f,	0.650f, -1700,  -2200,  -1000,  1.79f,	0.35f,	0.79f,	-700,	0.063f, 0.00f,0.00f,0.00f,	-2300,	0.032f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }


// OTHER SCENARIOS

// SPORTS PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_SPORT_EMPTYSTADIUM \
	{ 26,   7.2f,	1.000f, -1300,  -700,   -200,	6.26f,	0.51f,	1.10f,  -2400,	0.183f, 0.00f,0.00f,0.00f,	-1100,	0.038f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_SPORT_SQUASHCOURT \
	{ 26,   7.5f,	0.750f, -1100,  -1000,  -200,	2.22f,	0.91f,	1.16f,  -700,	0.007f, 0.00f,0.00f,0.00f,	-300,	0.011f,		0.00f,0.00f,0.00f,	0.126f, 0.190f, 0.250f, 0.000f, -0.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPORT_SMALLSWIMMINGPOOL \
	{ 26,  36.2f,	0.700f, -1400,  -200,   -100,	2.76f,	1.25f,	1.14f,  -400,	0.020f, 0.00f,0.00f,0.00f,	-300,	0.030f,		0.00f,0.00f,0.00f,	0.179f, 0.150f, 0.895f, 0.190f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAX30_PRESET_SPORT_LARGESWIMMINGPOOL\
	{ 26,  36.2f,	0.820f, -1200,  -200,   0,		5.49f,	1.31f,	1.14f,  -700,	0.039f, 0.00f,0.00f,0.00f,	-800,	0.049f,		0.00f,0.00f,0.00f,	0.222f, 0.550f, 1.159f, 0.210f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAX30_PRESET_SPORT_GYMNASIUM \
	{ 26,   7.5f,	0.810f, -1200,  -700,   -100,	3.14f,	1.06f,	1.35f,  -800,	0.029f, 0.00f,0.00f,0.00f,	-700,	0.045f,		0.00f,0.00f,0.00f,	0.146f, 0.140f, 0.250f, 0.000f, -0.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAX30_PRESET_SPORT_FULLSTADIUM \
	{ 26,   7.2f,	1.000f, -1300,  -2300,  -200,	5.25f,	0.17f,	0.80f,  -2000,	0.188f, 0.00f,0.00f,0.00f,	-1300,	0.038f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_SPORT_STADIUMTANNOY \
	{ 26,   3.0f,	0.780f, -900,   -500,   -600,	2.53f,	0.88f,	0.68f,  -1100,	0.230f, 0.00f,0.00f,0.00f,	-600,	0.063f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }

// PREFAB PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_PREFAB_WORKSHOP \
	{ 26,   1.9f,	1.000f, -1000,  -1700,  -800,	0.76f,	1.00f,	1.00f,	0,		0.012f, 0.00f,0.00f,0.00f,	-200,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAX30_PRESET_PREFAB_SCHOOLROOM \
	{ 26,   1.86f,	0.690f, -1100,  -400,   -600,	0.98f,	0.45f,	0.18f,  300,	0.017f, 0.00f,0.00f,0.00f,  0,		0.015f,		0.00f,0.00f,0.00f,	0.095f, 0.140f, 0.250f, 0.000f, -0.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAX30_PRESET_PREFAB_PRACTISEROOM \
	{ 26,   1.86f,	0.870f, -1000,  -800,   -600,	1.12f,	0.56f,	0.18f,  200,	0.010f, 0.00f,0.00f,0.00f,	-200,	0.011f,		0.00f,0.00f,0.00f,	0.095f, 0.140f, 0.250f, 0.000f, -0.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAX30_PRESET_PREFAB_OUTHOUSE \
	{ 26,  80.3f,	0.820f, -1100,  -1900,  -1600,  1.38f,	0.38f,	0.35f,	-100,	0.024f, 0.00f,0.00f,-0.00f,	-800,	0.044f,		0.00f,0.00f,0.00f,	0.121f, 0.170f, 0.250f, 0.000f, -0.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAX30_PRESET_PREFAB_CARAVAN \
	{ 26,   8.3f,	1.000f, -1000,  -2100,  -1800,  0.43f,	1.50f,	1.00f,  0,		0.012f, 0.00f,0.00f,0.00f,	400,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x1f }
			// for US developers, a caravan is the same as a trailer =o)


// DOME AND PIPE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_DOME_TOMB \
	{ 26,  51.8f,	0.790f, -1000,  -900,	-1300,  4.18f,	0.21f,	0.10f,  -825,	0.030f, 0.00f,0.00f,0.00f,	-125,	0.022f,		0.00f,0.00f,0.00f,	0.177f, 0.190f, 0.250f, 0.000f,	-5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }
#define EAX30_PRESET_PIPE_SMALL \
	{ 26,  50.3f,	1.000f, -1000,  -900,	-1300,  5.04f,	0.10f,	0.10f,  -600,	0.032f, 0.00f,0.00f,0.00f,	400,	0.015f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAX30_PRESET_DOME_SAINTPAULS \
	{ 26,  50.3f,	0.870f, -1000,  -900,	-1300,  10.48f,	0.19f,	0.10f,  -1500,	0.090f, 0.00f,0.00f,0.00f,	-500,	0.042f,		0.00f,0.00f,0.00f,	0.250f, 0.120f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAX30_PRESET_PIPE_LONGTHIN \
	{ 26,   1.6f,	0.910f, -1200,  -700,	-1100,  9.21f,	0.18f,	0.10f,  -300,	0.010f, 0.00f,0.00f,0.00f,	-1000,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }
#define EAX30_PRESET_PIPE_LARGE \
	{ 26,  50.3f,	1.000f, -1000,  -900,	-1300,  8.45f,	0.10f,	0.10f,  -800,	0.046f, 0.00f,0.00f,0.00f,  0,		0.032f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAX30_PRESET_PIPE_RESONANT \
	{ 26,   1.3f,	0.910f, -1200,  -700,	-1100,  6.81f,	0.18f,	0.10f,  -300,	0.010f, 0.00f,0.00f,0.00f,	-700,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }

// OUTDOORS PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_OUTDOORS_BACKYARD \
	{ 26,  80.3f,	0.450f,	-1100,  -1200,  -600,	1.12f,	0.34f,	0.46f,  -1100,	0.049f, 0.00f,0.00f,-0.00f,	-1300,	0.023f,		0.00f,0.00f,0.00f,	0.218f, 0.340f, 0.250f, 0.000f,	-5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAX30_PRESET_OUTDOORS_ROLLINGPLAINS \
	{ 26,  80.3f,	0.000f,	-1100,  -3900,  -400,	2.13f,	0.21f,	0.46f,  -2000,	0.300f, 0.00f,0.00f,-0.00f,	-1500,	0.019f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAX30_PRESET_OUTDOORS_DEEPCANYON \
	{ 26,  80.3f,	0.740f,	-1100,  -1500,  -400,	3.89f,	0.21f,	0.46f,  -2000,	0.193f, 0.00f,0.00f,-0.00f,	-1100,	0.019f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAX30_PRESET_OUTDOORS_CREEK \
	{ 26,  80.3f,	0.350f,	-1100,  -1500,  -600,	2.13f,	0.21f,	0.46f,  -1700,	0.115f, 0.00f,0.00f,-0.00f,	-1100,	0.031f,		0.00f,0.00f,0.00f,	0.218f, 0.340f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAX30_PRESET_OUTDOORS_VALLEY \
	{ 26,  80.3f,	0.280f,	-1100,  -3100,	-1600,  2.88f,	0.26f,	0.35f,  -3200,	0.163f, 0.00f,0.00f,-0.00f,	-1000,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 0.340f, 0.250f, 0.000f, -0.0f,  2854.4f,	107.5f,  0.00f, 0x0 }


// MOOD PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_MOOD_HEAVEN \
	{ 26,  19.6f,	0.940f,  -1000, -200,   -700,	5.04f,	1.12f,	0.56f,  -1230,	0.020f, 0.00f,0.00f,0.00f,	-200,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.080f, 2.742f, 0.050f, -2.0f,  5000.0f,	250.0f,  0.00f, 0x3f }
#define EAX30_PRESET_MOOD_HELL \
	{ 26, 100.0f,	0.570f,  -1000, -900,   -700,	3.57f,	0.49f,	2.00f,	-10000, 0.020f, 0.00f,0.00f,0.00f,	100,	0.030f,		0.00f,0.00f,0.00f,	0.110f, 0.040f, 2.109f, 0.520f, -5.0f,  5000.0f,	139.5f,  0.00f, 0x40 }
#define EAX30_PRESET_MOOD_MEMORY \
	{ 26,   8.0f,	0.850f,  -1000, -400,   -900,	4.06f,	0.82f,	0.56f,  -2800,	0.000f, 0.00f,0.00f,0.00f,	-500,	0.000f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.474f, 0.450f, -2.0f,  5000.0f,	250.0f,  0.00f, 0x0 }

// DRIVING SIMULATION PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_DRIVING_COMMENTATOR \
	{ 26,   3.0f,	0.000f, -900,   -500,   -600,	2.42f,	0.88f,	0.68f,  -1400,	0.093f, 0.00f,0.00f,0.00f,	-1200,	0.017f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_DRIVING_PITGARAGE \
	{ 26,   1.9f,	0.590f, -1400,  -300,   -500,	1.72f,	0.93f,	0.87f,  -500,	0.000f, 0.00f,0.00f,0.00f,	0,		0.016f,		0.00f,0.00f,0.00f,	0.250f, 0.110f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAX30_PRESET_DRIVING_INCAR_RACER \
	{ 26,   1.1f,	0.800f, -700,   0,		-200,	0.17f,	2.00f,	0.41f,  500,	0.007f, 0.00f,0.00f,0.00f,	-500,	0.015f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -0.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAX30_PRESET_DRIVING_INCAR_SPORTS \
	{ 26,   1.1f,	0.800f, -900,	-400,   0,		0.17f,	0.75f,	0.41f,  0,		0.010f, 0.00f,0.00f,0.00f,	-600,	0.000f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -0.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAX30_PRESET_DRIVING_INCAR_LUXURY \
	{ 26,   1.6f,	1.000f, -800,	-2000,  -600,	0.13f,	0.41f,	0.46f,  -200,	0.010f, 0.00f,0.00f,0.00f,	300,	0.010f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -0.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAX30_PRESET_DRIVING_FULLGRANDSTAND \
	{ 26,   8.3f,	1.000f, -1100,  -1100,  -400,	3.01f,	1.37f,	1.28f,  -900,	0.090f, 0.00f,0.00f,0.00f,	-1700,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10420.2f,	250.0f,  0.00f, 0x1f }
#define EAX30_PRESET_DRIVING_EMPTYGRANDSTAND \
	{ 26,   8.3f,	1.000f, -700,   0,		-200,	4.62f,	1.75f,	1.40f,  -1363,	0.090f, 0.00f,0.00f,0.00f,	-1900,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10420.2f,	250.0f,  0.00f, 0x1f }
#define EAX30_PRESET_DRIVING_TUNNEL \
	{ 26,   3.1f,	0.810f, -900,   -800,	-100,	3.42f,	0.94f,	1.31f,  -300,	0.051f, 0.00f,0.00f,0.00f,  -500,	0.047f,		0.00f,0.00f,0.00f,	0.214f, 0.050f, 0.250f, 0.000f, -0.0f,  5000.0f,	155.3f,  0.00f, 0x20 }

// CITY PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_CITY_STREETS \
	{ 26,   3.0f,	0.780f, -1100,  -300,   -100,	1.79f,	1.12f,	0.91f,  -1700,	0.046f, 0.00f,0.00f,0.00f,	-2800,	0.028f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_CITY_SUBWAY \
	{ 26,   3.0f,	0.740f, -1100,  -300,   -100,	3.01f,	1.23f,	0.91f,   -700,	0.046f, 0.00f,0.00f,0.00f,	-1000,	0.028f,		0.00f,0.00f,0.00f,	0.125f, 0.210f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_CITY_MUSEUM \
	{ 26,  80.3f,	0.820f, -1100,  -1500,  -1500,  3.28f,	1.40f,	0.57f,  -1600,	0.039f, 0.00f,0.00f,-0.00f, -600,	0.034f,		0.00f,0.00f,0.00f,	0.130f, 0.170f, 0.250f, 0.000f, -0.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAX30_PRESET_CITY_LIBRARY \
	{ 26,  80.3f,	0.820f, -1100,  -1100,  -2100,  2.76f,	0.89f,	0.41f,  -1100,	0.029f, 0.00f,0.00f,-0.00f, -500,	0.020f,		0.00f,0.00f,0.00f,	0.130f, 0.170f, 0.250f, 0.000f, -0.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAX30_PRESET_CITY_UNDERPASS \
	{ 26,   3.0f,	0.820f, -1500,  -700,   -100,	3.57f,	1.12f,	0.91f,  -1500,	0.059f, 0.00f,0.00f,0.00f,	-1100,	0.037f,		0.00f,0.00f,0.00f,	0.250f, 0.140f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAX30_PRESET_CITY_ABANDONED \
	{ 26,   3.0f,	0.690f, -1100,  -200,   -100,	3.28f,	1.17f,	0.91f,  -1400,	0.044f, 0.00f,0.00f,0.00f,	-2400,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -0.0f,  5000.0f,	250.0f,  0.00f, 0x20 }

// MISC ROOMS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAX30_PRESET_DUSTYROOM  \
	{ 26,   1.8f,	0.560f,	-1100,	-200,	-300,	1.79f,	0.38f,	0.21f,	-600,	0.002f,	0.00f,0.00f,0.00f,	200,	0.006f,		0.00f,0.00f,0.00f,	0.202f, 0.050f, 0.250f, 0.000f, -3.0f,  13046.0f,	163.3f,	0.00f,	0x20 }
#define EAX30_PRESET_CHAPEL \
	{ 26,  19.6f,	0.840f,	-1000,  -500,	0,		4.62f,	0.64f,	1.23f,  -700,	0.032f,	0.00f,0.00f,0.00f,	-800,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.110f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_SMALLWATERROOM \
	{ 26,  36.2f,	0.700f, -1200,  -698,   0,		1.51f,	1.25f,	1.14f,  -100,	0.020f, 0.00f,0.00f,0.00f,	200,	0.030f,		0.00f,0.00f,0.00f,	0.179f, 0.150f, 0.895f, 0.190f, -5.0f,  5000.0f,	250.0f, 0.00f, 0x0 }

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Effect Scenarios enumerated						//
//////////////////////////////////////////////////////

typedef enum
{
	EAX30_SCENARIO_CASTLE = 0,
	EAX30_SCENARIO_FACTORY,
	EAX30_SCENARIO_ICEPALACE,
	EAX30_SCENARIO_SPACESTATION,
	EAX30_SCENARIO_WOODGALLEON,
	EAX30_SCENARIO_SPORTS,
	EAX30_SCENARIO_PREFAB,
	EAX30_SCENARIO_DOMESNPIPES,
	EAX30_SCENARIO_OUTDOORS,
	EAX30_SCENARIO_MOOD,
	EAX30_SCENARIO_DRIVING,
	EAX30_SCENARIO_CITY,
	EAX30_SCENARIO_MISC,
	EAX30_SCENARIO_ORIGINAL
}
EAX30_SCENARIO;

//////////////////////////////////////////////////////
// Number of Effect Scenarios						//
//////////////////////////////////////////////////////

#define EAX30_NUM_SCENARIOS					14

//////////////////////////////////////////////////////
// Number of Effect Scenarios with standardised		//
// locations										//
//////////////////////////////////////////////////////

#define EAX30_NUM_STANDARD_SCENARIOS		5

//////////////////////////////////////////////////////
// Array of scenario names							//
//////////////////////////////////////////////////////

extern const char* EAX30_SCENARIO_NAMES[];

//////////////////////////////////////////////////////
// Standardised Locations enumerated				//
//////////////////////////////////////////////////////

typedef enum 
{
	EAX30_LOCATION_HALL = 0,
	EAX30_LOCATION_LARGEROOM,
	EAX30_LOCATION_MEDIUMROOM,
	EAX30_LOCATION_SMALLROOM,
	EAX30_LOCATION_CUPBOARD,
	EAX30_LOCATION_ALCOVE,
	EAX30_LOCATION_LONGPASSAGE,
	EAX30_LOCATION_SHORTPASSAGE,
	EAX30_LOCATION_COURTYARD
}
EAX30_LOCATION;

//////////////////////////////////////////////////////
// Number of Standardised Locations					//
//////////////////////////////////////////////////////

#define EAX30_NUM_LOCATIONS						9

//////////////////////////////////////////////////////
// Array of standardised location names				//
//////////////////////////////////////////////////////

extern const char* EAX30_LOCATION_NAMES[];

//////////////////////////////////////////////////////
// Number of effects in each scenario				//
//////////////////////////////////////////////////////

#define EAX30_NUM_ORIGINAL_PRESETS		26
#define EAX30_NUM_CASTLE_PRESETS		EAX30_NUM_LOCATIONS
#define EAX30_NUM_FACTORY_PRESETS		EAX30_NUM_LOCATIONS
#define EAX30_NUM_ICEPALACE_PRESETS		EAX30_NUM_LOCATIONS
#define EAX30_NUM_SPACESTATION_PRESETS	EAX30_NUM_LOCATIONS
#define EAX30_NUM_WOODGALLEON_PRESETS	EAX30_NUM_LOCATIONS
#define EAX30_NUM_SPORTS_PRESETS		7
#define EAX30_NUM_PREFAB_PRESETS		5
#define EAX30_NUM_DOMESNPIPES_PRESETS	6
#define EAX30_NUM_OUTDOORS_PRESETS		5
#define EAX30_NUM_MOOD_PRESETS			3
#define EAX30_NUM_DRIVING_PRESETS		8
#define EAX30_NUM_CITY_PRESETS			6
#define EAX30_NUM_MISC_PRESETS			3

//////////////////////////////////////////////////////
// Standardised Location effects can be accessed	//
// from a matrix									//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_STANDARD_PRESETS[EAX30_NUM_STANDARD_SCENARIOS][EAX30_NUM_LOCATIONS];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Original Preset effects enumerated				//
//////////////////////////////////////////////////////

typedef enum
{
	ORIGINAL_GENERIC = 0,
	ORIGINAL_PADDEDCELL,
	ORIGINAL_ROOM,
	ORIGINAL_BATHROOM,
	ORIGINAL_LIVINGROOM,
	ORIGINAL_STONEROOM,
	ORIGINAL_AUDITORIUM,
	ORIGINAL_CONCERTHALL,
	ORIGINAL_CAVE,
	ORIGINAL_ARENA,
	ORIGINAL_HANGAR,
	ORIGINAL_CARPETTEDHALLWAY,
	ORIGINAL_HALLWAY,
	ORIGINAL_STONECORRIDOR,
	ORIGINAL_ALLEY,
	ORIGINAL_FOREST,
	ORIGINAL_CITY,
	ORIGINAL_MOUNTAINS,
	ORIGINAL_QUARRY,
	ORIGINAL_PLAIN,
	ORIGINAL_PARKINGLOT,
	ORIGINAL_SEWERPIPE,
	ORIGINAL_UNDERWATER,
	ORIGINAL_DRUGGED,
	ORIGINAL_DIZZY,
	ORIGINAL_PSYCHOTIC
}
EAX30_ORIGINAL_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of original environment names				//
//////////////////////////////////////////////////////

extern const char* EAX30_ORIGINAL_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Original effects matrix							//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_ORIGINAL_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Sports scenario effects enumerated				//
//////////////////////////////////////////////////////

typedef enum
{
	SPORT_EMPTYSTADIUM=0,
	SPORT_FULLSTADIUM,
	SPORT_STADIUMTANNOY,
	SPORT_SQUASHCOURT,
	SPORT_SMALLSWIMMINGPOOL,
	SPORT_LARGESWIMMINGPOOL,
	SPORT_GYMNASIUM
}
EAX30_SPORTS_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of sport environment names					//
//////////////////////////////////////////////////////

extern const char* EAX30_SPORTS_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Sports effects matrix							//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_SPORTS_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Prefab scenario effects enumerated				//
//////////////////////////////////////////////////////

typedef enum
{
	PREFAB_WORKSHOP,
	PREFAB_SCHOOLROOM,
	PREFAB_PRACTISEROOM,
	PREFAB_OUTHOUSE,
	PREFAB_CARAVAN
}
EAX30_PREFAB_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of prefab environment names				//
//////////////////////////////////////////////////////

extern const char* EAX30_PREFAB_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Prefab effects matrix							//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_PREFAB_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Domes & Pipes effects enumerated					//
//////////////////////////////////////////////////////

typedef enum
{
	DOME_TOMB,
	DOME_SAINTPAULS,
	PIPE_SMALL,
	PIPE_LONGTHIN,
	PIPE_LARGE,
	PIPE_RESONANT
}
EAX30_DOMESNPIPES_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of Domes & Pipes environment names			//
//////////////////////////////////////////////////////

extern const char* EAX30_DOMESNPIPES_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Domes & Pipes effects matrix						//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_DOMESNPIPES_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Outdoors scenario effects enumerated				//
//////////////////////////////////////////////////////

typedef enum
{
	OUTDOORS_BACKYARD,
	OUTDOORS_ROLLINGPLAINS,
	OUTDOORS_DEEPCANYON,
	OUTDOORS_CREEK,
	OUTDOORS_VALLEY
}
EAX30_OUTDOORS_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of Outdoors environment names				//
//////////////////////////////////////////////////////

extern const char* EAX30_OUTDOORS_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Outdoors effects matrix							//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_OUTDOORS_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Mood scenario effects enumerated					//
//////////////////////////////////////////////////////

typedef enum
{
	MOOD_HEAVEN,
	MOOD_HELL,
	MOOD_MEMORY
}
EAX30_MOOD_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of Mood environment names					//
//////////////////////////////////////////////////////

extern const char* EAX30_MOOD_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Mood effects matrix								//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_MOOD_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Driving scenario effects enumerated				//
//////////////////////////////////////////////////////

typedef enum
{
	DRIVING_COMMENTATOR,
	DRIVING_PITGARAGE,
	DRIVING_INCAR_RACER,
	DRIVING_INCAR_SPORTS,
	DRIVING_INCAR_LUXURY,
	DRIVING_FULLGRANDSTAND,
	DRIVING_EMPTYGRANDSTAND,
	DRIVING_TUNNEL
}
EAX30_DRIVING_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of driving environment names				//
//////////////////////////////////////////////////////

extern const char* EAX30_DRIVING_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Driving effects matrix							//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_DRIVING_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// City scenario effects enumerated					//
//////////////////////////////////////////////////////

typedef enum
{
	CITY_STREETS,
	CITY_SUBWAY,
	CITY_MUSEUM,
	CITY_LIBRARY,
	CITY_UNDERPASS,
	CITY_ABANDONED
}
EAX30_CITY_PRESET_ENUMS;

//////////////////////////////////////////////////////
// Array of City environment names					//
//////////////////////////////////////////////////////

extern const char* EAX30_CITY_PRESET_NAMES[];

//////////////////////////////////////////////////////
// City effects matrix								//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_CITY_PRESETS[];

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Misc scenario effects enumerated					//
//////////////////////////////////////////////////////

typedef enum
{
	DUSTYROOM,
	CHAPEL,
	SMALLWATERROOM
}
EAX30_MISC_PRESET_ENUMS;


//////////////////////////////////////////////////////
// Array of Misc environment names					//
//////////////////////////////////////////////////////

extern const char* EAX30_MISC_PRESET_NAMES[];

//////////////////////////////////////////////////////
// Misc effects matrix								//
//////////////////////////////////////////////////////

extern EAXLISTENERPROPERTIES EAX30_MISC_PRESETS[];


/***********************************************************************************************\
*
* Material transmission presets
*
* Three values in this order :-
*
*	1. Occlusion (or Obstruction)
*	2. Occlusion LF Ratio (or Obstruction LF Ratio)
*	3. Occlusion Room Ratio
*
************************************************************************************************/


// Single window material preset
#define EAX_MATERIAL_SINGLEWINDOW          (-2800)
#define EAX_MATERIAL_SINGLEWINDOWLF        0.71f
#define EAX_MATERIAL_SINGLEWINDOWROOMRATIO 0.43f

// Double window material preset
#define EAX_MATERIAL_DOUBLEWINDOW          (-5000)
#define EAX_MATERIAL_DOUBLEWINDOWLF        0.40f
#define EAX_MATERIAL_DOUBLEWINDOWROOMRATIO 0.24f

// Thin door material preset
#define EAX_MATERIAL_THINDOOR              (-1800)
#define EAX_MATERIAL_THINDOORLF            0.66f
#define EAX_MATERIAL_THINDOORROOMRATIO     0.66f

// Thick door material preset
#define EAX_MATERIAL_THICKDOOR             (-4400)
#define EAX_MATERIAL_THICKDOORLF           0.64f
#define EAX_MATERIAL_THICKDOORROOMRATIO	   0.27f

// Wood wall material preset
#define EAX_MATERIAL_WOODWALL              (-4000)
#define EAX_MATERIAL_WOODWALLLF            0.50f
#define EAX_MATERIAL_WOODWALLROOMRATIO     0.30f

// Brick wall material preset
#define EAX_MATERIAL_BRICKWALL             (-5000)
#define EAX_MATERIAL_BRICKWALLLF           0.60f
#define EAX_MATERIAL_BRICKWALLROOMRATIO    0.24f

// Stone wall material preset
#define EAX_MATERIAL_STONEWALL             (-6000)
#define EAX_MATERIAL_STONEWALLLF           0.68f
#define EAX_MATERIAL_STONEWALLROOMRATIO    0.20f

// Curtain material preset
#define EAX_MATERIAL_CURTAIN               (-1200)
#define EAX_MATERIAL_CURTAINLF             0.15f
#define EAX_MATERIAL_CURTAINROOMRATIO      1.00f


#endif // EAXUTIL_INCLUDED

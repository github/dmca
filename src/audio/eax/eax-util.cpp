/***********************************************************************************************\
*																								*
*  EAX-UTIL.CPP -	utilities for EAX 3.0														*
*					Function declaration for EAX Morphing										*
*					String names of the all the presets defined in eax-util.h					*
*					Arrays grouping together all the EAX presets in a scenario					*
*																								*
************************************************************************************************/

#include "eax-util.h"
#include <math.h>

// Function prototypes used by EAX3ListenerInterpolate
void Clamp(EAXVECTOR *eaxVector);
bool CheckEAX3LP(LPEAXLISTENERPROPERTIES lpEAX3LP);


/***********************************************************************************************\
*
* Definition of the EAXMorph function - EAX3ListenerInterpolate
*
\***********************************************************************************************/

/*
	EAX3ListenerInterpolate
	lpStart			- Initial EAX 3 Listener parameters
	lpFinish		- Final EAX 3 Listener parameters
	flRatio			- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
	lpResult		- Interpolated EAX 3 Listener parameters
	bCheckValues	- Check EAX 3.0 parameters are in range, default = false (no checking)
*/
bool EAX3ListenerInterpolate(LPEAXLISTENERPROPERTIES lpStart, LPEAXLISTENERPROPERTIES lpFinish,
						float flRatio, LPEAXLISTENERPROPERTIES lpResult, bool bCheckValues)
{
	EAXVECTOR StartVector, FinalVector;

	float flInvRatio;

	if (bCheckValues)
	{
		if (!CheckEAX3LP(lpStart))
			return false;

		if (!CheckEAX3LP(lpFinish))
			return false;
	}

	if (flRatio >= 1.0f)
	{
		memcpy(lpResult, lpFinish, sizeof(EAXLISTENERPROPERTIES));
		return true;
	}
	else if (flRatio <= 0.0f)
	{
		memcpy(lpResult, lpStart, sizeof(EAXLISTENERPROPERTIES));
		return true;
	}

	flInvRatio = (1.0f - flRatio);

	// Environment
	lpResult->ulEnvironment = 26;	// (UNDEFINED environment)

	// Environment Size
	if (lpStart->flEnvironmentSize == lpFinish->flEnvironmentSize)
		lpResult->flEnvironmentSize = lpStart->flEnvironmentSize;
	else
		lpResult->flEnvironmentSize = (float)exp( (log(lpStart->flEnvironmentSize) * flInvRatio) + (log(lpFinish->flEnvironmentSize) * flRatio) );
	
	// Environment Diffusion
	if (lpStart->flEnvironmentDiffusion == lpFinish->flEnvironmentDiffusion)
		lpResult->flEnvironmentDiffusion = lpStart->flEnvironmentDiffusion;
	else
		lpResult->flEnvironmentDiffusion = (lpStart->flEnvironmentDiffusion * flInvRatio) + (lpFinish->flEnvironmentDiffusion * flRatio);
	
	// Room
	if (lpStart->lRoom == lpFinish->lRoom)
		lpResult->lRoom = lpStart->lRoom;
	else
		lpResult->lRoom = (int)( ((float)lpStart->lRoom * flInvRatio) + ((float)lpFinish->lRoom * flRatio) );
	
	// Room HF
	if (lpStart->lRoomHF == lpFinish->lRoomHF)
		lpResult->lRoomHF = lpStart->lRoomHF;
	else
		lpResult->lRoomHF = (int)( ((float)lpStart->lRoomHF * flInvRatio) + ((float)lpFinish->lRoomHF * flRatio) );
	
	// Room LF
	if (lpStart->lRoomLF == lpFinish->lRoomLF)
		lpResult->lRoomLF = lpStart->lRoomLF;
	else
		lpResult->lRoomLF = (int)( ((float)lpStart->lRoomLF * flInvRatio) + ((float)lpFinish->lRoomLF * flRatio) );
	
	// Decay Time
	if (lpStart->flDecayTime == lpFinish->flDecayTime)
		lpResult->flDecayTime = lpStart->flDecayTime;
	else
		lpResult->flDecayTime = (float)exp( (log(lpStart->flDecayTime) * flInvRatio) + (log(lpFinish->flDecayTime) * flRatio) );
	
	// Decay HF Ratio
	if (lpStart->flDecayHFRatio == lpFinish->flDecayHFRatio)
		lpResult->flDecayHFRatio = lpStart->flDecayHFRatio;
	else
		lpResult->flDecayHFRatio = (float)exp( (log(lpStart->flDecayHFRatio) * flInvRatio) + (log(lpFinish->flDecayHFRatio) * flRatio) );
	
	// Decay LF Ratio
	if (lpStart->flDecayLFRatio == lpFinish->flDecayLFRatio)
		lpResult->flDecayLFRatio = lpStart->flDecayLFRatio;
	else
		lpResult->flDecayLFRatio = (float)exp( (log(lpStart->flDecayLFRatio) * flInvRatio) + (log(lpFinish->flDecayLFRatio) * flRatio) );
	
	// Reflections
	if (lpStart->lReflections == lpFinish->lReflections)
		lpResult->lReflections = lpStart->lReflections;
	else
		lpResult->lReflections = (int)( ((float)lpStart->lReflections * flInvRatio) + ((float)lpFinish->lReflections * flRatio) );
	
	// Reflections Delay
	if (lpStart->flReflectionsDelay == lpFinish->flReflectionsDelay)
		lpResult->flReflectionsDelay = lpStart->flReflectionsDelay;
	else
		lpResult->flReflectionsDelay = (float)exp( (log(lpStart->flReflectionsDelay+0.0001f) * flInvRatio) + (log(lpFinish->flReflectionsDelay+0.0001f) * flRatio) );

	// Reflections Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors vectors are clamped to a length of 1.0f
	StartVector = lpStart->vReflectionsPan;
	FinalVector = lpFinish->vReflectionsPan;

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->vReflectionsPan.x == lpFinish->vReflectionsPan.x)
		lpResult->vReflectionsPan.x = lpStart->vReflectionsPan.x;
	else
		lpResult->vReflectionsPan.x = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));
	
	if (lpStart->vReflectionsPan.y == lpFinish->vReflectionsPan.y)
		lpResult->vReflectionsPan.y = lpStart->vReflectionsPan.y;
	else
		lpResult->vReflectionsPan.y = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));
	
	if (lpStart->vReflectionsPan.z == lpFinish->vReflectionsPan.z)
		lpResult->vReflectionsPan.z = lpStart->vReflectionsPan.z;
	else
		lpResult->vReflectionsPan.z = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));
	
	// Reverb
	if (lpStart->lReverb == lpFinish->lReverb)
		lpResult->lReverb = lpStart->lReverb;
	else
		lpResult->lReverb = (int)( ((float)lpStart->lReverb * flInvRatio) + ((float)lpFinish->lReverb * flRatio) );
	
	// Reverb Delay
	if (lpStart->flReverbDelay == lpFinish->flReverbDelay)
		lpResult->flReverbDelay = lpStart->flReverbDelay;
	else
		lpResult->flReverbDelay = (float)exp( (log(lpStart->flReverbDelay+0.0001f) * flInvRatio) + (log(lpFinish->flReverbDelay+0.0001f) * flRatio) );
	
	// Reverb Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors are clamped to a length of 1.0f	
	StartVector = lpStart->vReverbPan;
	FinalVector = lpFinish->vReverbPan;

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->vReverbPan.x == lpFinish->vReverbPan.x)
		lpResult->vReverbPan.x = lpStart->vReverbPan.x;
	else
		lpResult->vReverbPan.x = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));
	
	if (lpStart->vReverbPan.y == lpFinish->vReverbPan.y)
		lpResult->vReverbPan.y = lpStart->vReverbPan.y;
	else
		lpResult->vReverbPan.y = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));
	
	if (lpStart->vReverbPan.z == lpFinish->vReverbPan.z)
		lpResult->vReverbPan.z = lpStart->vReverbPan.z;
	else
		lpResult->vReverbPan.z = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));
	
	// Echo Time
	if (lpStart->flEchoTime == lpFinish->flEchoTime)
		lpResult->flEchoTime = lpStart->flEchoTime;
	else
		lpResult->flEchoTime = (float)exp( (log(lpStart->flEchoTime) * flInvRatio) + (log(lpFinish->flEchoTime) * flRatio) );
	
	// Echo Depth
	if (lpStart->flEchoDepth == lpFinish->flEchoDepth)
		lpResult->flEchoDepth = lpStart->flEchoDepth;
	else
		lpResult->flEchoDepth = (lpStart->flEchoDepth * flInvRatio) + (lpFinish->flEchoDepth * flRatio);

	// Modulation Time
	if (lpStart->flModulationTime == lpFinish->flModulationTime)
		lpResult->flModulationTime = lpStart->flModulationTime;
	else
		lpResult->flModulationTime = (float)exp( (log(lpStart->flModulationTime) * flInvRatio) + (log(lpFinish->flModulationTime) * flRatio) );
	
	// Modulation Depth
	if (lpStart->flModulationDepth == lpFinish->flModulationDepth)
		lpResult->flModulationDepth = lpStart->flModulationDepth;
	else
		lpResult->flModulationDepth = (lpStart->flModulationDepth * flInvRatio) + (lpFinish->flModulationDepth * flRatio);
	
	// Air Absorption HF
	if (lpStart->flAirAbsorptionHF == lpFinish->flAirAbsorptionHF)
		lpResult->flAirAbsorptionHF = lpStart->flAirAbsorptionHF;
	else
		lpResult->flAirAbsorptionHF = (lpStart->flAirAbsorptionHF * flInvRatio) + (lpFinish->flAirAbsorptionHF * flRatio);
	
	// HF Reference
	if (lpStart->flHFReference == lpFinish->flHFReference)
		lpResult->flHFReference = lpStart->flHFReference;
	else
		lpResult->flHFReference = (float)exp( (log(lpStart->flHFReference) * flInvRatio) + (log(lpFinish->flHFReference) * flRatio) );
	
	// LF Reference
	if (lpStart->flLFReference == lpFinish->flLFReference)
		lpResult->flLFReference = lpStart->flLFReference;
	else
		lpResult->flLFReference = (float)exp( (log(lpStart->flLFReference) * flInvRatio) + (log(lpFinish->flLFReference) * flRatio) );
	
	// Room Rolloff Factor
	if (lpStart->flRoomRolloffFactor == lpFinish->flRoomRolloffFactor)
		lpResult->flRoomRolloffFactor = lpStart->flRoomRolloffFactor;
	else
		lpResult->flRoomRolloffFactor = (lpStart->flRoomRolloffFactor * flInvRatio) + (lpFinish->flRoomRolloffFactor * flRatio);
	
	// Flags
	lpResult->ulFlags = (lpStart->ulFlags & lpFinish->ulFlags);

	// Clamp Delays
	if (lpResult->flReflectionsDelay > EAXLISTENER_MAXREFLECTIONSDELAY)
		lpResult->flReflectionsDelay = EAXLISTENER_MAXREFLECTIONSDELAY;

	if (lpResult->flReverbDelay > EAXLISTENER_MAXREVERBDELAY)
		lpResult->flReverbDelay = EAXLISTENER_MAXREVERBDELAY;

	return true;
}


/*
	CheckEAX3LP
	Checks that the parameters in the EAX 3 Listener Properties structure are in-range
*/
bool CheckEAX3LP(LPEAXLISTENERPROPERTIES lpEAX3LP)
{
	if ( (lpEAX3LP->lRoom < EAXLISTENER_MINROOM) || (lpEAX3LP->lRoom > EAXLISTENER_MAXROOM) )
		return false;

	if ( (lpEAX3LP->lRoomHF < EAXLISTENER_MINROOMHF) || (lpEAX3LP->lRoomHF > EAXLISTENER_MAXROOMHF) )
		return false;

	if ( (lpEAX3LP->lRoomLF < EAXLISTENER_MINROOMLF) || (lpEAX3LP->lRoomLF > EAXLISTENER_MAXROOMLF) )
		return false;

	if ( (lpEAX3LP->ulEnvironment < EAXLISTENER_MINENVIRONMENT) || (lpEAX3LP->ulEnvironment > EAXLISTENER_MAXENVIRONMENT) )
		return false;

	if ( (lpEAX3LP->flEnvironmentSize < EAXLISTENER_MINENVIRONMENTSIZE) || (lpEAX3LP->flEnvironmentSize > EAXLISTENER_MAXENVIRONMENTSIZE) )
		return false;

	if ( (lpEAX3LP->flEnvironmentDiffusion < EAXLISTENER_MINENVIRONMENTDIFFUSION) || (lpEAX3LP->flEnvironmentDiffusion > EAXLISTENER_MAXENVIRONMENTDIFFUSION) )
		return false;

	if ( (lpEAX3LP->flDecayTime < EAXLISTENER_MINDECAYTIME) || (lpEAX3LP->flDecayTime > EAXLISTENER_MAXDECAYTIME) )
		return false;

	if ( (lpEAX3LP->flDecayHFRatio < EAXLISTENER_MINDECAYHFRATIO) || (lpEAX3LP->flDecayHFRatio > EAXLISTENER_MAXDECAYHFRATIO) )
		return false;

	if ( (lpEAX3LP->flDecayLFRatio < EAXLISTENER_MINDECAYLFRATIO) || (lpEAX3LP->flDecayLFRatio > EAXLISTENER_MAXDECAYLFRATIO) )
		return false;

	if ( (lpEAX3LP->lReflections < EAXLISTENER_MINREFLECTIONS) || (lpEAX3LP->lReflections > EAXLISTENER_MAXREFLECTIONS) )
		return false;

	if ( (lpEAX3LP->flReflectionsDelay < EAXLISTENER_MINREFLECTIONSDELAY) || (lpEAX3LP->flReflectionsDelay > EAXLISTENER_MAXREFLECTIONSDELAY) )
		return false;

	if ( (lpEAX3LP->lReverb < EAXLISTENER_MINREVERB) || (lpEAX3LP->lReverb > EAXLISTENER_MAXREVERB) )
		return false;

	if ( (lpEAX3LP->flReverbDelay < EAXLISTENER_MINREVERBDELAY) || (lpEAX3LP->flReverbDelay > EAXLISTENER_MAXREVERBDELAY) )
		return false;

	if ( (lpEAX3LP->flEchoTime < EAXLISTENER_MINECHOTIME) || (lpEAX3LP->flEchoTime > EAXLISTENER_MAXECHOTIME) )
		return false;

	if ( (lpEAX3LP->flEchoDepth < EAXLISTENER_MINECHODEPTH) || (lpEAX3LP->flEchoDepth > EAXLISTENER_MAXECHODEPTH) )
		return false;

	if ( (lpEAX3LP->flModulationTime < EAXLISTENER_MINMODULATIONTIME) || (lpEAX3LP->flModulationTime > EAXLISTENER_MAXMODULATIONTIME) )
		return false;

	if ( (lpEAX3LP->flModulationDepth < EAXLISTENER_MINMODULATIONDEPTH) || (lpEAX3LP->flModulationDepth > EAXLISTENER_MAXMODULATIONDEPTH) )
		return false;

	if ( (lpEAX3LP->flAirAbsorptionHF < EAXLISTENER_MINAIRABSORPTIONHF) || (lpEAX3LP->flAirAbsorptionHF > EAXLISTENER_MAXAIRABSORPTIONHF) )
		return false;

	if ( (lpEAX3LP->flHFReference < EAXLISTENER_MINHFREFERENCE) || (lpEAX3LP->flHFReference > EAXLISTENER_MAXHFREFERENCE) )
		return false;

	if ( (lpEAX3LP->flLFReference < EAXLISTENER_MINLFREFERENCE) || (lpEAX3LP->flLFReference > EAXLISTENER_MAXLFREFERENCE) )
		return false;

	if ( (lpEAX3LP->flRoomRolloffFactor < EAXLISTENER_MINROOMROLLOFFFACTOR) || (lpEAX3LP->flRoomRolloffFactor > EAXLISTENER_MAXROOMROLLOFFFACTOR) )
		return false;

	if (lpEAX3LP->ulFlags & EAXLISTENERFLAGS_RESERVED)
		return false;

	return true;
}

/*
	Clamp
	Clamps the length of the vector to 1.0f
*/
void Clamp(EAXVECTOR *eaxVector)
{
	float flMagnitude;
	float flInvMagnitude;

	flMagnitude = (float)sqrt((eaxVector->x*eaxVector->x) + (eaxVector->y*eaxVector->y) + (eaxVector->z*eaxVector->z));

	if (flMagnitude <= 1.0f)
		return;

	flInvMagnitude = 1.0f / flMagnitude;

	eaxVector->x *= flInvMagnitude;
	eaxVector->y *= flInvMagnitude;
	eaxVector->z *= flInvMagnitude;
}


/***********************************************************************************************\
*
* To assist those developers wishing to add EAX effects to their level editors, each of the 

* List of string names of the various EAX 3.0 presets defined in eax-util.h
* Arrays to group together presets of the same scenario
*
\***********************************************************************************************/


//////////////////////////////////////////////////////
// Array of scenario names							//
//////////////////////////////////////////////////////

const char* EAX30_SCENARIO_NAMES[] =				
{	
	"Castle", 
	"Factory", 
	"IcePalace", 
	"SpaceStation", 
	"WoodenShip",
	"Sports",
	"Prefab",
	"Domes and Pipes",
	"Outdoors",
	"Mood",
	"Driving",
	"City",
	"Miscellaneous",
	"Original"
};

//////////////////////////////////////////////////////
// Array of standardised location names				//
//////////////////////////////////////////////////////

const char* EAX30_LOCATION_NAMES[] =				
{	
	"Hall", 
	"Large Room", 
	"Medium Room", 
	"Small Room", 
	"Cupboard", 
	"Alcove", 
	"Long Passage", 
	"Short Passage", 
	"Courtyard"
};

//////////////////////////////////////////////////////
// Standardised Location effects can be accessed	//
// from a matrix									//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_STANDARD_PRESETS[EAX30_NUM_STANDARD_SCENARIOS][EAX30_NUM_LOCATIONS]=
{
	{EAX30_PRESET_CASTLE_HALL,		EAX30_PRESET_CASTLE_LARGEROOM,		EAX30_PRESET_CASTLE_MEDIUMROOM,			EAX30_PRESET_CASTLE_SMALLROOM,		EAX30_PRESET_CASTLE_CUPBOARD,		EAX30_PRESET_CASTLE_ALCOVE,			EAX30_PRESET_CASTLE_LONGPASSAGE,		EAX30_PRESET_CASTLE_SHORTPASSAGE,		EAX30_PRESET_CASTLE_COURTYARD},
	{EAX30_PRESET_FACTORY_HALL,		EAX30_PRESET_FACTORY_LARGEROOM,		EAX30_PRESET_FACTORY_MEDIUMROOM,		EAX30_PRESET_FACTORY_SMALLROOM,		EAX30_PRESET_FACTORY_CUPBOARD,		EAX30_PRESET_FACTORY_ALCOVE,		EAX30_PRESET_FACTORY_LONGPASSAGE,		EAX30_PRESET_FACTORY_SHORTPASSAGE,		EAX30_PRESET_FACTORY_COURTYARD},
	{EAX30_PRESET_ICEPALACE_HALL,	EAX30_PRESET_ICEPALACE_LARGEROOM,	EAX30_PRESET_ICEPALACE_MEDIUMROOM,		EAX30_PRESET_ICEPALACE_SMALLROOM,	EAX30_PRESET_ICEPALACE_CUPBOARD,	EAX30_PRESET_ICEPALACE_ALCOVE,		EAX30_PRESET_ICEPALACE_LONGPASSAGE,		EAX30_PRESET_ICEPALACE_SHORTPASSAGE,	EAX30_PRESET_ICEPALACE_COURTYARD},
	{EAX30_PRESET_SPACESTATION_HALL,EAX30_PRESET_SPACESTATION_LARGEROOM,EAX30_PRESET_SPACESTATION_MEDIUMROOM,	EAX30_PRESET_SPACESTATION_SMALLROOM,EAX30_PRESET_SPACESTATION_CUPBOARD,	EAX30_PRESET_SPACESTATION_ALCOVE,	EAX30_PRESET_SPACESTATION_LONGPASSAGE,	EAX30_PRESET_SPACESTATION_SHORTPASSAGE, EAX30_PRESET_SPACESTATION_HALL},
	{EAX30_PRESET_WOODEN_HALL,		EAX30_PRESET_WOODEN_LARGEROOM,		EAX30_PRESET_WOODEN_MEDIUMROOM,			EAX30_PRESET_WOODEN_SMALLROOM,		EAX30_PRESET_WOODEN_CUPBOARD,		EAX30_PRESET_WOODEN_ALCOVE,			EAX30_PRESET_WOODEN_LONGPASSAGE,		EAX30_PRESET_WOODEN_SHORTPASSAGE,		EAX30_PRESET_WOODEN_COURTYARD},
};


/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of original environment names				//
//////////////////////////////////////////////////////

const char* EAX30_ORIGINAL_PRESET_NAMES[] =
{
	"Generic",
	"Padded Cell",
	"Room",
	"Bathroom",
	"Living Room",
	"Stone Room",
	"Auditorium",
	"Concert Hall",
	"Cave",
	"Arena",
	"Hangar",
	"Carpetted Hallway",
	"Hallway",
	"Stone Corridor",
	"Alley",
	"Forest",
	"City",
	"Mountains",
	"Quarry",
	"Plain",
	"Parking Lot",
	"Sewer Pipe",
	"Underwater",
	"Drugged",
	"Dizzy",
	"Psychotic"
};

//////////////////////////////////////////////////////
// Sports effects matrix							//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES		EAX30_ORIGINAL_PRESETS[] =
{
	EAX30_PRESET_GENERIC,
	EAX30_PRESET_PADDEDCELL,
	EAX30_PRESET_ROOM,
	EAX30_PRESET_BATHROOM,
	EAX30_PRESET_LIVINGROOM,
	EAX30_PRESET_STONEROOM,
	EAX30_PRESET_AUDITORIUM,
	EAX30_PRESET_CONCERTHALL,
	EAX30_PRESET_CAVE,
	EAX30_PRESET_ARENA,
	EAX30_PRESET_HANGAR,
	EAX30_PRESET_CARPETTEDHALLWAY,
	EAX30_PRESET_HALLWAY,
	EAX30_PRESET_STONECORRIDOR,
	EAX30_PRESET_ALLEY,
	EAX30_PRESET_FOREST,
	EAX30_PRESET_CITY,
	EAX30_PRESET_MOUNTAINS,
	EAX30_PRESET_QUARRY,
	EAX30_PRESET_PLAIN,
	EAX30_PRESET_PARKINGLOT,
	EAX30_PRESET_SEWERPIPE,
	EAX30_PRESET_UNDERWATER,
	EAX30_PRESET_DRUGGED,
	EAX30_PRESET_DIZZY,
	EAX30_PRESET_PSYCHOTIC
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of sport environment names					//
//////////////////////////////////////////////////////

const char* EAX30_SPORTS_PRESET_NAMES[] =
{
	"Empty Stadium",
	"Full Stadium", 
	"Stadium Tannoy",
	"Squash Court",
	"Small Swimming Pool", 
	"Large Swimming Pool",
	"Gymnasium"
};

//////////////////////////////////////////////////////
// Sports effects matrix							//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES		EAX30_SPORTS_PRESETS[] =
{
	EAX30_PRESET_SPORT_EMPTYSTADIUM,
	EAX30_PRESET_SPORT_FULLSTADIUM,
	EAX30_PRESET_SPORT_STADIUMTANNOY,
	EAX30_PRESET_SPORT_SQUASHCOURT,
	EAX30_PRESET_SPORT_SMALLSWIMMINGPOOL,
	EAX30_PRESET_SPORT_LARGESWIMMINGPOOL,
	EAX30_PRESET_SPORT_GYMNASIUM
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of prefab environment names				//
//////////////////////////////////////////////////////

const char* EAX30_PREFAB_PRESET_NAMES[] =
{
	"Workshop",
	"School Room",
	"Practise Room",
	"Outhouse",
	"Caravan"
};

//////////////////////////////////////////////////////
// Prefab effects matrix							//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_PREFAB_PRESETS[] =
{
	EAX30_PRESET_PREFAB_WORKSHOP,
	EAX30_PRESET_PREFAB_SCHOOLROOM,
	EAX30_PRESET_PREFAB_PRACTISEROOM,
	EAX30_PRESET_PREFAB_OUTHOUSE,
	EAX30_PRESET_PREFAB_CARAVAN
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Domes & Pipes environment names			//
//////////////////////////////////////////////////////

const char* EAX30_DOMESNPIPES_PRESET_NAMES[] =
{
	"Domed Tomb",
	"Saint Paul's Dome",
	"Small Pipe",
	"Long Thin Pipe", 
	"Large Pipe",
	"Resonant Pipe"
};

//////////////////////////////////////////////////////
// Domes & Pipes effects matrix						//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_DOMESNPIPES_PRESETS[] =
{
	EAX30_PRESET_DOME_TOMB,
	EAX30_PRESET_DOME_SAINTPAULS,
	EAX30_PRESET_PIPE_SMALL,
	EAX30_PRESET_PIPE_LONGTHIN,
	EAX30_PRESET_PIPE_LARGE,
	EAX30_PRESET_PIPE_RESONANT
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Outdoors environment names				//
//////////////////////////////////////////////////////

const char* EAX30_OUTDOORS_PRESET_NAMES[] =
{
	"Backyard", 
	"Rolling Plains",
	"Deep Canyon",
	"Creek",
	"Valley"
};

//////////////////////////////////////////////////////
// Outdoors effects matrix							//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_OUTDOORS_PRESETS[] =
{
	EAX30_PRESET_OUTDOORS_BACKYARD,
	EAX30_PRESET_OUTDOORS_ROLLINGPLAINS,
	EAX30_PRESET_OUTDOORS_DEEPCANYON,
	EAX30_PRESET_OUTDOORS_CREEK,
	EAX30_PRESET_OUTDOORS_VALLEY
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Mood environment names					//
//////////////////////////////////////////////////////

const char* EAX30_MOOD_PRESET_NAMES[] =
{
	"Heaven",
	"Hell",
	"Memory"
};

//////////////////////////////////////////////////////
// Mood effects matrix								//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_MOOD_PRESETS[] =
{
	EAX30_PRESET_MOOD_HEAVEN,
	EAX30_PRESET_MOOD_HELL,
	EAX30_PRESET_MOOD_MEMORY
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of driving environment names				//
//////////////////////////////////////////////////////

const char* EAX30_DRIVING_PRESET_NAMES[] =
{
	"Race Commentator",
	"Pit Garage",
	"In-car (Stripped out racer)",
	"In-car (Sportscar)",
	"In-car (Luxury)",
	"Full Grandstand",
	"Empty Grandstand",
	"Tunnel"
};

//////////////////////////////////////////////////////
// Driving effects matrix							//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_DRIVING_PRESETS[] =
{
	EAX30_PRESET_DRIVING_COMMENTATOR,
	EAX30_PRESET_DRIVING_PITGARAGE,
	EAX30_PRESET_DRIVING_INCAR_RACER,
	EAX30_PRESET_DRIVING_INCAR_SPORTS,
	EAX30_PRESET_DRIVING_INCAR_LUXURY,
	EAX30_PRESET_DRIVING_FULLGRANDSTAND,
	EAX30_PRESET_DRIVING_EMPTYGRANDSTAND,
	EAX30_PRESET_DRIVING_TUNNEL
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of City environment names					//
//////////////////////////////////////////////////////

const char* EAX30_CITY_PRESET_NAMES[] =
{
	"City Streets",
	"Subway",
	"Museum",
	"Library",
	"Underpass",
	"Abandoned City"
};

//////////////////////////////////////////////////////
// City effects matrix								//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_CITY_PRESETS[] =
{
	EAX30_PRESET_CITY_STREETS,
	EAX30_PRESET_CITY_SUBWAY,
	EAX30_PRESET_CITY_MUSEUM,
	EAX30_PRESET_CITY_LIBRARY,
	EAX30_PRESET_CITY_UNDERPASS,
	EAX30_PRESET_CITY_ABANDONED
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Misc environment names					//
//////////////////////////////////////////////////////

const char* EAX30_MISC_PRESET_NAMES[] =
{
	"Dusty Box Room",
	"Chapel",
	"Small Water Room"
};

//////////////////////////////////////////////////////
// Misc effects matrix								//
//////////////////////////////////////////////////////

EAXLISTENERPROPERTIES EAX30_MISC_PRESETS[] =
{
	EAX30_PRESET_DUSTYROOM,
	EAX30_PRESET_CHAPEL,
	EAX30_PRESET_SMALLWATERROOM
};


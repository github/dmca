#pragma once
#include "Mono.h"

class MelonLoader_Base
{
public:
	enum MelonCompatibility
	{
		UNIVERSAL,
		COMPATIBLE,
		NOATTRIBUTE,
		INCOMPATIBLE
	};

	static bool HasInitialized;
	static MonoMethod* startup;
	
	static void Initialize();
	static void Startup();
};
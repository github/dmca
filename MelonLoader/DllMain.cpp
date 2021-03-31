#include <Windows.h>
#include "MelonLoader.h"
#include "MelonLoader_Base.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	MelonLoader::thisdll = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
#ifndef DEBUG
		DisableThreadLibraryCalls(MelonLoader::thisdll);
#endif
		MelonLoader::Main();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		MelonLoader::UNLOAD();
		FreeLibrary(MelonLoader::thisdll);
	}
	return TRUE;
}
#include <Windows.h>
#include <string>

extern "C" FARPROC GetFileVersionInfoA_Original = NULL;
extern "C" FARPROC GetFileVersionInfoByHandle_Original = NULL;
extern "C" FARPROC GetFileVersionInfoExA_Original = NULL;
extern "C" FARPROC GetFileVersionInfoExW_Original = NULL;
extern "C" FARPROC GetFileVersionInfoSizeA_Original = NULL;
extern "C" FARPROC GetFileVersionInfoSizeExA_Original = NULL;
extern "C" FARPROC GetFileVersionInfoSizeExW_Original = NULL;
extern "C" FARPROC GetFileVersionInfoSizeW_Original = NULL;
extern "C" FARPROC GetFileVersionInfoW_Original = NULL;
extern "C" FARPROC VerFindFileA_Original = NULL;
extern "C" FARPROC VerFindFileW_Original = NULL;
extern "C" FARPROC VerInstallFileA_Original = NULL;
extern "C" FARPROC VerInstallFileW_Original = NULL;
extern "C" FARPROC VerLanguageNameA_Original = NULL;
extern "C" FARPROC VerLanguageNameW_Original = NULL;
extern "C" FARPROC VerQueryValueA_Original = NULL;
extern "C" FARPROC VerQueryValueW_Original = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	LPSTR filepath = new CHAR[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), filepath, MAX_PATH);
	if (strstr(filepath, "UnityCrashHandler") != NULL)
	{
		HANDLE hProcess = GetCurrentProcess();
		if (hProcess != NULL)
		{
			TerminateProcess(hProcess, NULL);
			CloseHandle(hProcess);
		}
		return FALSE;
	}

	std::string filepathstr = filepath;
	HINSTANCE originaldll = LoadLibrary((filepathstr.substr(0, filepathstr.find_last_of(".")) + "_original.dll").c_str());
	if (originaldll == NULL)
	{
		char* path = new char[MAX_PATH];
		if (GetSystemDirectory(path, MAX_PATH) != NULL)
			originaldll = LoadLibrary((std::string(path) + "\\version.dll").c_str());
		else
		{
			MessageBox(NULL, "Failed to Get System32 Directory!", "MelonLoader", MB_ICONERROR | MB_OK);
			return FALSE;
		}
	}
	if (originaldll != NULL)
	{
		GetFileVersionInfoA_Original = GetProcAddress(originaldll, "GetFileVersionInfoA");
		GetFileVersionInfoByHandle_Original = GetProcAddress(originaldll, "GetFileVersionInfoByHandle");
		GetFileVersionInfoExA_Original = GetProcAddress(originaldll, "GetFileVersionInfoExA");
		GetFileVersionInfoExW_Original = GetProcAddress(originaldll, "GetFileVersionInfoExW");
		GetFileVersionInfoSizeA_Original = GetProcAddress(originaldll, "GetFileVersionInfoSizeA");
		GetFileVersionInfoSizeExA_Original = GetProcAddress(originaldll, "GetFileVersionInfoSizeExA");
		GetFileVersionInfoSizeExW_Original = GetProcAddress(originaldll, "GetFileVersionInfoSizeExW");
		GetFileVersionInfoSizeW_Original = GetProcAddress(originaldll, "GetFileVersionInfoSizeW");
		GetFileVersionInfoW_Original = GetProcAddress(originaldll, "GetFileVersionInfoW");
		VerFindFileA_Original = GetProcAddress(originaldll, "VerFindFileA");
		VerFindFileW_Original = GetProcAddress(originaldll, "VerFindFileW");
		VerInstallFileA_Original = GetProcAddress(originaldll, "VerInstallFileA");
		VerInstallFileW_Original = GetProcAddress(originaldll, "VerInstallFileW");
		VerLanguageNameA_Original = GetProcAddress(originaldll, "VerLanguageNameA");
		VerLanguageNameW_Original = GetProcAddress(originaldll, "VerLanguageNameW");
		VerQueryValueA_Original = GetProcAddress(originaldll, "VerQueryValueA");
		VerQueryValueW_Original = GetProcAddress(originaldll, "VerQueryValueW");
		if (strstr(GetCommandLine(), "--no-mods") == NULL)
		{
			HINSTANCE melonloaderdll = LoadLibrary("MelonLoader\\MelonLoader.dll");
			if (melonloaderdll == NULL)
				MessageBox(NULL, "Failed to Load MelonLoader.dll!", "MelonLoader", MB_ICONERROR | MB_OK);
		}
		return TRUE;
	}
	else
		MessageBox(NULL, "Failed to Load version.dll!", "MelonLoader", MB_ICONERROR | MB_OK);
	return FALSE;
}

extern "C" void GetFileVersionInfoA_ML();
extern "C" void GetFileVersionInfoByHandle_ML();
extern "C" void GetFileVersionInfoExA_ML();
extern "C" void GetFileVersionInfoExW_ML();
extern "C" void GetFileVersionInfoSizeA_ML();
extern "C" void GetFileVersionInfoSizeExA_ML();
extern "C" void GetFileVersionInfoSizeExW_ML();
extern "C" void GetFileVersionInfoSizeW_ML();
extern "C" void GetFileVersionInfoW_ML();
extern "C" void VerFindFileA_ML();
extern "C" void VerFindFileW_ML();
extern "C" void VerInstallFileA_ML();
extern "C" void VerInstallFileW_ML();
extern "C" void VerLanguageNameA_ML();
extern "C" void VerLanguageNameW_ML();
extern "C" void VerQueryValueA_ML();
extern "C" void VerQueryValueW_ML();
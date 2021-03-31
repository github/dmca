#pragma once
#include <string>
#include <Windows.h>
#include <vector>

class AssertionManager
{
public:
	static const char* FileName;
	static const char* Position;
	static bool Result;

	static void Start(const char* filename, const char* position);
	static void ThrowError(std::string msg, const char* filepath = NULL);
	static void ThrowInternalError(std::string msg);
	static void Decide(void* thing, const char* name);
	static HMODULE LoadLib(const char* name, const char* filepath);
	static HMODULE GetModuleHandlePtr(const char* name);
	static FARPROC GetExport(HMODULE mod, const char* export_name);
};
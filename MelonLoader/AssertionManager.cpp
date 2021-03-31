#include "AssertionManager.h"
#include "MelonLoader.h"
#include "Logger.h"
#include "Mono.h"

bool AssertionManager::Result = false;
const char* AssertionManager::FileName = NULL;
const char* AssertionManager::Position = NULL;

void AssertionManager::Start(const char* filename, const char* position)
{
	FileName = filename;
	Position = position;
}

void AssertionManager::ThrowError(std::string msg, const char* filepath)
{
	if (!Result)
	{
		msg += (" for [ " + std::string(FileName) + " | " + Position + " ]");
		if (filepath != NULL)
			msg += " in { " + std::string(filepath) + "}";
		ThrowInternalError(msg);
		MelonLoader::UNLOAD(true);
	}
}

void AssertionManager::ThrowInternalError(std::string msg)
{
	if (!Result)
	{
		Result = true;
		Logger::LogTimestamp(ConsoleColor_Red);
		Logger::LogFile << "[Error] " << msg << std::endl;
		Console::Write("[MelonLoader] ", ConsoleColor_Red);
		Console::WriteLine(("[Error] " + msg), ConsoleColor_Red);
		if (MelonLoader::DebugMode)
			MessageBox(NULL, msg.c_str(), "MelonLoader - INTERNAL FAILURE", MB_OK | MB_ICONERROR);
		else
			MessageBox(NULL, "Please Post your Latest Log File\non #internal-failure in the MelonLoader Discord!", "MelonLoader - INTERNAL FAILURE!", MB_OK | MB_ICONERROR);
	}
}

void AssertionManager::Decide(void* thing, const char* name)
{
	if (!Result && (thing == NULL))
		ThrowError((std::string(name) + " is NULL"));
}

HMODULE AssertionManager::LoadLib(const char* name, const char* filepath)
{
	HMODULE returnval = NULL;
	if (!Result)
	{
		returnval = LoadLibrary(filepath);
		if (returnval == NULL)
			ThrowError((std::string("Failed to LoadLib ( ") + name + " )"));
	}
	return returnval;
}

HMODULE AssertionManager::GetModuleHandlePtr(const char* name)
{
	HMODULE returnval = NULL;
	if (!Result)
	{
		returnval = GetModuleHandle(name);
		if (returnval == NULL)
			ThrowError((std::string("Failed to GetModuleHandlePtr ( ") + name + " )"));
	}
	return returnval;
}

FARPROC AssertionManager::GetExport(HMODULE mod, const char* export_name)
{
	FARPROC returnval = NULL;
	if (!Result)
	{
		returnval = GetProcAddress(mod, export_name);
		if (returnval == NULL)
			ThrowError((std::string("Failed to GetExport ( ") + export_name + " )"));
	}
	return returnval;
}
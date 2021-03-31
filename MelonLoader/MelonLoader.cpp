#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include "MelonLoader.h"
#include "Console.h"
#include "Mono.h"
#include "HookManager.h"
#include "Logger.h"
#include "MelonLoader_Base.h"
#include <signal.h>
#include "DisableAnalytics.h"
#pragma warning(disable:4996)

HINSTANCE MelonLoader::thisdll = NULL;
int MelonLoader::CommandLineC = NULL;
char* MelonLoader::CommandLineV[64];
bool MelonLoader::IsGameIl2Cpp = false;
bool MelonLoader::DebugMode = false;
bool MelonLoader::QuitFix = false;
bool MelonLoader::AG_Force_Regenerate = false;
char* MelonLoader::ExePath = NULL;;
char* MelonLoader::GamePath = NULL;
char* MelonLoader::DataPath = NULL;
char* MelonLoader::CompanyName = NULL;
char* MelonLoader::ProductName = NULL;
char* MelonLoader::ForceUnhollowerVersion = NULL;
char* MelonLoader::ForceUnityVersion = NULL;
MelonLoader::LoadMode MelonLoader::LoadMode_Plugins = MelonLoader::LoadMode::NORMAL;
MelonLoader::LoadMode MelonLoader::LoadMode_Mods = MelonLoader::LoadMode::NORMAL;

void MelonLoader::Main()
{
	if (CheckOSVersion())
	{
		ParseCommandLine();

		LPSTR filepath = new CHAR[MAX_PATH];
		HMODULE exe_module = GetModuleHandle(NULL);
		GetModuleFileName(exe_module, filepath, MAX_PATH);

		std::string filepathstr = filepath;
		ExePath = new char[filepathstr.size() + 1];
		std::copy(filepathstr.begin(), filepathstr.end(), ExePath);
		ExePath[filepathstr.size()] = '\0';

		filepathstr = filepathstr.substr(0, filepathstr.find_last_of("\\/"));
		GamePath = new char[filepathstr.size() + 1];
		std::copy(filepathstr.begin(), filepathstr.end(), GamePath);
		GamePath[filepathstr.size()] = '\0';

		std::string gameassemblypath = filepathstr + "\\GameAssembly.dll";
		WIN32_FIND_DATA data;
		HANDLE h = FindFirstFile(gameassemblypath.c_str(), &data);
		if (h != INVALID_HANDLE_VALUE)
			IsGameIl2Cpp = true;

		Logger::Initialize(filepathstr);

#ifdef DEBUG
		DebugMode = true;
		Console::Create();
#endif

		std::string pdatapath = filepathstr + "\\*_Data";
		h = FindFirstFile(pdatapath.c_str(), &data);
		if (h != INVALID_HANDLE_VALUE)
		{
			char* nPtr = new char[lstrlen(data.cFileName) + 1];
			for (int i = 0; i < lstrlen(data.cFileName); i++)
				nPtr[i] = char(data.cFileName[i]);
			nPtr[lstrlen(data.cFileName)] = '\0';

			std::string ndatapath = filepathstr + "\\" + std::string(nPtr);
			DataPath = new char[ndatapath.size() + 1];
			std::copy(ndatapath.begin(), ndatapath.end(), DataPath);
			DataPath[ndatapath.size()] = '\0';

			std::string assemblypath = std::string();
			std::string basepath = std::string();
			std::string configpath = std::string();
			if (IsGameIl2Cpp)
			{
				assemblypath = filepathstr + "\\MelonLoader\\Managed";
				basepath = filepathstr + "\\MelonLoader\\Dependencies";
				configpath = ndatapath + "\\il2cpp_data\\etc";
			}
			else
			{
				assemblypath = ndatapath + "\\Managed";
				std::string newbasepath = filepathstr + "\\Mono";
				h = FindFirstFile(newbasepath.c_str(), &data);
				if (h == INVALID_HANDLE_VALUE)
				{
					newbasepath = ndatapath + "\\Mono";
					h = FindFirstFile(newbasepath.c_str(), &data);
				}
				if (h == INVALID_HANDLE_VALUE)
				{
					newbasepath = filepathstr + "\\MonoBleedingEdge";
					h = FindFirstFile(newbasepath.c_str(), &data);
				}
				if (h == INVALID_HANDLE_VALUE)
				{
					newbasepath = ndatapath + "\\MonoBleedingEdge";
					h = FindFirstFile(newbasepath.c_str(), &data);
				}
				if (h != INVALID_HANDLE_VALUE)
				{
					basepath = newbasepath + "\\EmbedRuntime";
					configpath = newbasepath + "\\etc";
				}
			}
			if (!assemblypath.empty())
			{
				Mono::AssemblyPath = new char[assemblypath.size() + 1];
				std::copy(assemblypath.begin(), assemblypath.end(), Mono::AssemblyPath);
				Mono::AssemblyPath[assemblypath.size()] = '\0';
			}
			if (!basepath.empty())
			{
				Mono::BasePath = new char[basepath.size() + 1];
				std::copy(basepath.begin(), basepath.end(), Mono::BasePath);
				Mono::BasePath[basepath.size()] = '\0';
			}
			if (!configpath.empty())
			{
				Mono::ConfigPath = new char[configpath.size() + 1];
				std::copy(configpath.begin(), configpath.end(), Mono::ConfigPath);
				Mono::ConfigPath[configpath.size()] = '\0';
			}

			ReadAppInfo();

			DisableAnalytics::Setup();
			HookManager::LoadLibraryW_Hook();
		}
	}
}

void MelonLoader::ParseCommandLine()
{
	char* next = NULL;
	char* curchar = strtok_s(GetCommandLine(), " ", &next);
	while (curchar && (CommandLineC < 63))
	{
		CommandLineV[CommandLineC++] = curchar;
		curchar = strtok_s(0, " ", &next);
	}
	CommandLineV[CommandLineC] = 0;
	if (CommandLineC > 0)
	{
		for (int i = 0; i < CommandLineC; i++)
		{
			const char* command = CommandLineV[i];
			if (command != NULL)
			{
				if (strstr(command, "--quitfix") != NULL)
					QuitFix = true;
				else if (strstr(command, "--melonloader.magenta"))
					Console::ChromiumMode = true;
				else if (strstr(command, "--melonloader.rainbow") != NULL)
					Console::HordiniMode = true;
				else if (strstr(command, "--melonloader.randomrainbow") != NULL)
					Console::HordiniMode_Random = true;
				else if (strstr(command, "--melonloader.consoleontop") != NULL)
					Console::AlwaysOnTop = true;
				else if (strstr(command, "--melonloader.loadmodeplugins") != NULL)
				{
					int loadmode = atoi(CommandLineV[i + 1]);
					if (loadmode < 0)
						loadmode = 0;
					else if (loadmode > 2)
						loadmode = 0;
					LoadMode_Plugins = (LoadMode)loadmode;
				}
				else if (strstr(command, "--melonloader.loadmodemods") != NULL)
				{
					int loadmode = atoi(CommandLineV[i + 1]);
					if (loadmode < 0)
						loadmode = 0;
					else if (loadmode > 2)
						loadmode = 0;
					LoadMode_Mods = (LoadMode)loadmode;
				}
				else if (strstr(command, "--melonloader.agregenerate") != NULL)
					AG_Force_Regenerate = true;
				else if (strstr(command, "--melonloader.agfvunhollower"))
					ForceUnhollowerVersion = CommandLineV[i + 1];
				//else if (strstr(command, "--melonloader.agfvunity"))
				//	ForceUnityVersion = CommandLineV[i + 1];
#ifndef DEBUG
				else if (strstr(command, "--melonloader.maxlogs") != NULL)
					Logger::MaxLogs = GetIntFromConstChar(CommandLineV[i + 1], 10);
				else if (strstr(command, "--melonloader.maxwarnings") != NULL)
					Logger::MaxWarnings = GetIntFromConstChar(CommandLineV[i + 1], 10);
				else if (strstr(command, "--melonloader.maxerrors") != NULL)
					Logger::MaxErrors = GetIntFromConstChar(CommandLineV[i + 1], 10);
				else if (strstr(command, "--melonloader.hideconsole") != NULL)
					Console::Enabled = false;
				else if (strstr(command, "--melonloader.hidewarnings") != NULL)
					Console::HideWarnings = false;
				else if (strstr(command, "--melonloader.debug") != NULL)
				{
					DebugMode = true;
					Console::Create();
				}
#endif
			}
		}
	}
}

void MelonLoader::ReadAppInfo()
{
	std::ifstream appinfofile((std::string(DataPath) + "\\app.info"));
	std::string line;
	while (std::getline(appinfofile, line, '\n'))
	{
		if (CompanyName == NULL)
		{
			CompanyName = new char[line.size() + 1];
			std::copy(line.begin(), line.end(), CompanyName);
			CompanyName[line.size()] = '\0';
		}
		else
		{
			ProductName = new char[line.size() + 1];
			std::copy(line.begin(), line.end(), ProductName);
			ProductName[line.size()] = '\0';
		}
	}
	appinfofile.close();
}

bool MelonLoader::CheckOSVersion()
{
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&info);
	if ((info.dwMajorVersion < 6) || ((info.dwMajorVersion == 6) && (info.dwMinorVersion < 1)))
	{
		int result = MessageBox(NULL, "You are running on an Unsupported OS.\nWe can not offer support if there are any issues.\nContinue with MelonLoader?", "MelonLoader", MB_ICONWARNING | MB_YESNO);
		if (result == IDYES)
			return true;
		return false;
	}
	return true;
}

void MelonLoader::UNLOAD(bool doquitfix)
{
	HookManager::UnhookAll();
	Logger::Stop();
	if (doquitfix && MelonLoader::QuitFix)
		MelonLoader::KillProcess();
}

void MelonLoader::KillProcess()
{
	HANDLE hProcess = GetCurrentProcess();
	if (hProcess != NULL)
	{
		TerminateProcess(hProcess, NULL);
		CloseHandle(hProcess);
	}
}

int MelonLoader::GetIntFromConstChar(const char* str, int defaultval)
{
	if ((str == NULL) || (*str == '\0'))
		return defaultval;
    bool negate = (str[0] == '-');
    if ((*str == '+') || (*str == '-'))
        ++str;
	if (*str == '\0')
		return defaultval;
    int result = 0;
    while(*str)
    {
		if ((*str >= '0') && (*str <= '9'))
			result = ((result * 10) - (*str - '0'));
		else
			return defaultval;
        ++str;
    }
    return (negate ? result : -result);
}
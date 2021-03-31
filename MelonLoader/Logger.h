#pragma once
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include "Console.h"
#include "MelonLoader_Base.h"

class LogStream
{
public:
	std::ofstream coss;
	template <class T>
	LogStream& operator<< (T val)
	{
		if (coss.is_open())
			coss << val;
		return *this;
	}
	LogStream& operator<< (std::ostream& (*pfun)(std::ostream&))
	{
		if (coss.is_open())
			pfun(coss);
		return *this;
	}
};

class Logger
{
public:
	static LogStream LogFile;
	static int MaxLogs;
	static int WarningCount;
	static int MaxWarnings;
	static int ErrorCount;
	static int MaxErrors;
	static const char* FilePrefix;
	static const char* FileExtention;

	static void Initialize(std::string filepathstr);
	static void Stop() { LogFile.coss.close(); }
	static void CleanOldLogs(std::string logFolderPath);

	static void Log(const char* txt);
	static void Log(const char* txt, ConsoleColor color);
	static void Log(const char* namesection, const char* txt);
	static void Log(const char* namesection, const char* txt, ConsoleColor color);
	static void Log(std::string txt) { Log(txt.c_str()); };
	static void Log(std::string txt, ConsoleColor color) { Log(txt.c_str(), color); }

	static void LogWarning(const char* txt);
	static void LogWarning(const char* namesection, const char* txt);
	static void LogWarning(std::string txt) { LogWarning(txt.c_str()); }

	static void LogError(const char* txt);
	static void LogError(const char* namesection, const char* txt);
	static void LogError(std::string txt) { LogError(txt.c_str()); }
	
	static void DebugLog(const char* txt) { if (MelonLoader::DebugMode) Log(txt); }
	static void DebugLog(const char* txt, ConsoleColor color) { if (MelonLoader::DebugMode) Log(txt, color); };
	static void DebugLog(std::string txt) { DebugLog(txt.c_str()); }
	static void DebugLog(std::string txt, ConsoleColor color) { DebugLog(txt.c_str(), color); }

	static void DebugLogWarning(const char* txt) { if (MelonLoader::DebugMode) LogWarning(txt); }
	static void DebugLogWarning(const char* namesection, const char* txt) { if (MelonLoader::DebugMode) LogWarning(namesection, txt); }
	static void DebugLogWarning(std::string txt) { DebugLogWarning(txt.c_str()); }

	static void DebugLogError(const char* txt) { if (MelonLoader::DebugMode) LogError(txt); }
	static void DebugLogError(const char* namesection, const char* txt) { if (MelonLoader::DebugMode) LogError(namesection, txt); }
	static void DebugLogError(std::string txt) { DebugLogError(txt.c_str()); }

	static void LogMelonError(const char* namesection, const char* txt);
	static void LogMelonCompatibility(MelonLoader_Base::MelonCompatibility comp);
	static void LogTimestamp(ConsoleColor color = ConsoleColor_Black);

private:
	static bool CompareWritetime(const std::filesystem::directory_entry& first, const std::filesystem::directory_entry& second) { return first.last_write_time().time_since_epoch() >= second.last_write_time().time_since_epoch(); }
	static bool DirectoryExists(const char* path) { struct stat info; if (stat(path, &info) != NULL) return false; if (info.st_mode & S_IFDIR) return true; return false; }
};
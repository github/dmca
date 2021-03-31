#include <fstream>
#include "Console.h"

HWND Console::hwndConsole = NULL;
int Console::rainbow = 1;
bool Console::Enabled = true;
bool Console::HideWarnings = false;
bool Console::HordiniMode = false;
bool Console::HordiniMode_Random = false;
bool Console::ChromiumMode = false;
bool Console::ShouldShowGameLogs = false;
bool Console::AlwaysOnTop = false;
HANDLE Console::OutputHandle = NULL;

void Console::Create()
{
	if (!IsInitialized())
	{
		if (AllocConsole())
		{
			hwndConsole = GetConsoleWindow();
			SetTitle(("MelonLoader " + (MelonLoader::DebugMode ? std::string("Debug") : std::string("Normal")) + " Console").c_str());
			SetForegroundWindow(hwndConsole);
			AlwaysOnTopCheck();			
			freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		}
		// if it returned false, console was already allocated, just grab the handle from it
		OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}
}

void Console::RainbowCheck()
{
	if (IsInitialized() && (HordiniMode || HordiniMode_Random))
	{
		if (HordiniMode_Random)
			SetColor((ConsoleColor)(1 + (rand() * (int)(15 - 1) / RAND_MAX)));
		else
		{
			SetColor((ConsoleColor)rainbow);
			rainbow++;
			if (rainbow > 15)
				rainbow = 1;
			else if (rainbow == 7)
				rainbow++;
		}
	}
}

void Console::ChromiumCheck()
{
	if (IsInitialized() && ChromiumMode)
		SetColor(ConsoleColor_Magenta);
}

void Console::AlwaysOnTopCheck()
{
	if (IsInitialized() && AlwaysOnTop)
	{
		SetWindowPos(hwndConsole, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(hwndConsole, SW_NORMAL);
	}
}

void Console::Write(const char* txt)
{
	if (IsInitialized())
	{
		ChromiumCheck();
		RainbowCheck();
		AlwaysOnTopCheck();
		std::cout << txt;
		ResetColor();
	}
};

void Console::Write(const char* txt, ConsoleColor color)
{
	if (IsInitialized())
	{
		SetColor(color);
		Write(txt);
		ResetColor();
	}
}
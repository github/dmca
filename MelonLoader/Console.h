#pragma once
#include <string>
#include <iostream>
#include "MelonLoader.h"

enum ConsoleColor
{
	//
	// Summary:
	//     The color black.
	ConsoleColor_Black = 0,
	//
	// Summary:
	//     The color dark blue.
	ConsoleColor_DarkBlue = 1,
	//
	// Summary:
	//     The color dark green.
	ConsoleColor_DarkGreen = 2,
	//
	// Summary:
	//     The color dark cyan (dark blue-green).
	ConsoleColor_DarkCyan = 3,
	//
	// Summary:
	//     The color dark red.
	ConsoleColor_DarkRed = 4,
	//
	// Summary:
	//     The color dark magenta (dark purplish-red).
	ConsoleColor_DarkMagenta = 5,
	//
	// Summary:
	//     The color dark yellow (ochre).
	ConsoleColor_DarkYellow = 6,
	//
	// Summary:
	//     The color gray.
	ConsoleColor_Gray = 7,
	//
	// Summary:
	//     The color dark gray.
	ConsoleColor_DarkGray = 8,
	//
	// Summary:
	//     The color blue.
	ConsoleColor_Blue = 9,
	//
	// Summary:
	//     The color green.
	ConsoleColor_Green = 10,
	//
	// Summary:
	//     The color cyan (blue-green).
	ConsoleColor_Cyan = 11,
	//
	// Summary:
	//     The color red.
	ConsoleColor_Red = 12,
	//
	// Summary:
	//     The color magenta (purplish-red).
	ConsoleColor_Magenta = 13,
	//
	// Summary:
	//     The color yellow.
	ConsoleColor_Yellow = 14,
	//
	// Summary:
	//     The color white.
	ConsoleColor_White = 15
};

class Console
{
public:
	static HWND hwndConsole;
	static int rainbow;
	static bool Enabled;
	static bool HideWarnings;
	static bool HordiniMode;
	static bool HordiniMode_Random;
	static bool ChromiumMode;
	static bool ShouldShowGameLogs;
	static bool AlwaysOnTop;
	static HANDLE OutputHandle;

	static bool IsInitialized() { return (hwndConsole != NULL); }
	static void Create();

	static void SetTitle(const char* title) { SetConsoleTitle(title); }
	static void SetColor(ConsoleColor color) { SetConsoleTextAttribute(OutputHandle, color); }
	static void ResetColor() { SetColor(ConsoleColor_Gray); }
	static void RainbowCheck();
	static void ChromiumCheck();
	static void AlwaysOnTopCheck();

	static void Write(const char* txt);
	static void Write(const char* txt, ConsoleColor color);
	static void Write(std::string txt) { Write(txt.c_str()); }
	static void Write(std::string txt, ConsoleColor color) { Write(txt.c_str(), color); }

	static void WriteLine(const char* txt) { Write(txt); std::cout << std::endl; }
	static void WriteLine(const char* txt, ConsoleColor color) { Write(txt, color); std::cout << std::endl; }
	static void WriteLine(std::string txt) { WriteLine(txt.c_str()); }
	static void WriteLine(std::string txt, ConsoleColor color) { WriteLine(txt.c_str(), color); }
};
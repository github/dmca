#pragma once

class CDebug
{
	enum
	{
		MAX_LINES = 15,
		MAX_STR_LEN = 80,

		MAX_SCREEN_STRS = 100,
		MAX_DEBUG_LINES = 100,
	};

	static int16 ms_nCurrentTextLine;
	static char ms_aTextBuffer[MAX_LINES][MAX_STR_LEN];

	// custom
	struct ScreenStr {
		int x, y;
		char str[256];
	};
	static ScreenStr ms_aScreenStrs[MAX_SCREEN_STRS];
	static int ms_nScreenStrs;

	struct Line {
		CVector p1, p2;
		uint32 c1, c2;
	};
	static Line ms_aLines[MAX_DEBUG_LINES];
	static int ms_nLines;

public:
	static void DebugInitTextBuffer();
	static void DebugDisplayTextBuffer();
	static void DebugAddText(const char *str);

	// custom
	static void PrintAt(const char *str, int x, int y);
	static void DisplayScreenStrings();

	static void AddLine(CVector p1, CVector p2, uint32 c1, uint32 c2);
	static void DrawLines(void);
};

extern bool gbDebugStuffInRelease;

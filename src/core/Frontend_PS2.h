#pragma once
#include "Sprite2d.h"

enum
{
	PAGE_STATS,
	PAGE_LOAD,
	PAGE_BRIEFS,
	PAGE_CONTROLS,
	PAGE_AUDIO,
	PAGE_DISPLAY,
	PAGE_LANGUAGE,

	NUM_PAGES,
	PAGE_FIRST = PAGE_STATS,
	PAGE_LAST = PAGE_LANGUAGE,
};

enum 
{
	PAGESTATE_NORMAL = 0,
	PAGESTATE_HIGHLIGHTED,
	PAGESTATE_SELECTED
};


enum eFrontendSprites
{
	FE2_MAINPANEL_UL,
	FE2_MAINPANEL_UR,
	FE2_MAINPANEL_DL,
	FE2_MAINPANEL_DR,
	FE2_MAINPANEL_DR2,
	FE2_TABACTIVE,
	FE_ICONBRIEF,
	FE_ICONSTATS,
	FE_ICONCONTROLS,
	FE_ICONSAVE,
	FE_ICONAUDIO,
	FE_ICONDISPLAY,
	FE_ICONLANGUAGE,
	FE_CONTROLLER,
	FE_CONTROLLERSH,
	FE_ARROWS1,
	FE_ARROWS2,
	FE_ARROWS3,
	FE_ARROWS4,
	FE_RADIO1,
	FE_RADIO2,
	FE_RADIO3,
	FE_RADIO4,
	FE_RADIO5,
	FE_RADIO6,
	FE_RADIO7,
	FE_RADIO8,
	FE_RADIO9,

	NUM_FE_SPRITES
};


class CSprite2d;
class CVector2D;

#ifdef GTA_PC
enum eControlMethod
{
	CONTROL_STANDARD = 0,
	CONTROL_CLASSIC,
};
#endif

class CMenuManager
{
public:
	enum LANGUAGE
	{
		LANGUAGE_AMERICAN,
		LANGUAGE_FRENCH,
		LANGUAGE_GERMAN,
		LANGUAGE_ITALIAN,
		LANGUAGE_SPANISH,
#ifdef MORE_LANGUAGES
		LANGUAGE_POLISH,
		LANGUAGE_RUSSIAN,
		LANGUAGE_JAPANESE,
#endif
	};
	
	enum CONTRCONFIG
	{
		CONFIG_1 = 0,
		CONFIG_2,
		CONFIG_3,
		CONFIG_4,
	};

	enum
	{
		NUM_SPRIRES = 28,
	};
	
	enum
	{
		PAGESTATE_NORMAL = 0,
		PAGESTATE_HIGHLIGHTED = 1,
		PAGESTATE_SELELECTED = 2,
	};
	
	enum
	{
		SLIDE_TO_BOTTOM = 0,
		SLIDE_TO_RIGHT,
		SLIDE_TO_TOP,
		SLIDE_TO_LEFT,
		SLIDE_MAX
	};
	
	int32 m_currentPage;
	int32 m_newPage;
	int32 m_pageState;
	uint32 m_nPageLeftTimer;
	uint32 m_nPageRightTimer;
	uint32 m_nChangePageTimer;
	int field_18;
	uint8 m_fade;
	uint8 m_someAlpha;
	//char field_1E; // unused ?
	//char field_1F; // unused ?
	uint32 m_nStartPauseTimer;
	uint32 m_nEndPauseTimer;
	CVector2D m_position;
	uint8 m_nSlidingDir;
	//char field_31; // unused ?
	//char field_32; // unused ?
	//char field_33; // unused ?
	bool m_bInitialised;
	bool m_bWantToUpdateContent;
	bool m_bMenuActive;
	bool m_bWantToRestart;
	//char field_38; //unused ?
	bool m_bRenderGameInMenu;
	bool m_bSaveMenuActive;
	bool m_bInSaveZone;
	char field_3C;
	bool m_bTexturesLoaded;
	//char field_3E; //unused ?
	//char field_3F; //unused ?
	CSprite2d m_sprites[NUM_SPRIRES];
	
	static int32 m_PrefsSfxVolume;
	static int32 m_PrefsMusicVolume;
	static int32 m_PrefsBrightness;
	static bool m_PrefsShowTrails;
	static bool m_PrefsShowSubtitles;
	static bool m_PrefsAllowNastyGame;
	static int32 m_PrefsRadioStation;
	static int32 m_PrefsStereoMono;
	static int8 m_PrefsUseWideScreen;
	static int32 m_PrefsLanguage;
	static CONTRCONFIG m_PrefsControllerConfig;
	static bool m_PrefsUseVibration;

#define ISLAND_LOADING_IS(p)
#define ISLAND_LOADING_ISNT(p)
#ifdef GTA_PC
	bool m_bQuitGameNoCD;
	
	int32 m_nMouseTempPosX;
	int32 m_nMouseTempPosY;
	int32 m_nPrefsVideoMode;
	int32 m_nDisplayVideoMode;
	int8 m_nPrefsAudio3DProviderIndex;
	
	static int32 OS_Language;
	static int8 m_PrefsVsync;
	static int8 m_PrefsVsyncDisp;
	static int8 m_PrefsFrameLimiter;
	static int8 m_PrefsSpeakers;
	static int32 m_ControlMethod;
	static int8 m_PrefsDMA;
	static float m_PrefsLOD;
	static char m_PrefsSkinFile[256];

#ifndef MASTER
	static bool m_PrefsMarketing;
	static bool m_PrefsDisableTutorials;
#endif // !MASTER

#ifdef MENU_MAP
	static bool bMenuMapActive;
	static float fMapSize;
	static float fMapCenterY;
	static float fMapCenterX;
#endif

#ifdef IMPROVED_VIDEOMODE	
	int32 m_nPrefsWidth = 640;
	int32 m_nPrefsHeight = 480;
	int32 m_nPrefsDepth = 32;
	int32 m_nPrefsWindowed = 1;
	int32 m_nPrefsSubsystem;
	int32 m_nSelectedScreenMode;
#endif

	void WaitForUserCD() { }
#endif

	bool GetIsMenuActive() {return !!m_bMenuActive;}

	CMenuManager(void);
#ifdef FIX_BUGS
	~CMenuManager(void)
	{
		UnloadTextures();
	}
#endif
	
	void LoadAllTextures(void);
	void UnloadTextures(void);
	
	void InitialiseMenusOnce(void);
	void InitialiseChangedLanguageSettings(void);
	void InitialiseMenuContents(void);
	void AnaliseMenuContents(void);
	void InitialiseMenuContentsAfterLoadingGame(void);
	void DrawFrontEnd(void);
	void DrawFrontEndNormal(void);
	void DrawFrontEndSaveZone(void);
	void DrawMemoryCardStartUpMenus(void);
	void Process(void);
	void WorkOutMenuState(uint8 bExit);
	void ProcessControllerInput(void);
	void ProcessDPadLeftJustDown(void);
	void ProcessDPadRightJustDown(void);
	void ProcessDPadUpJustDown(void);
	void ProcessDPadDownJustDown(void);
	void ProcessDPadTriangleJustDown(void);
	void ProcessDPadCrossJustDown(void);
	void DoHackingMenusAtPageBrowse(void);
	void SetSoundLevelsForMusicMenu(void);
	void FilterOutColorMarkersFromString(wchar *string, CRGBA &color);
};

extern CMenuManager FrontEndMenuManager;
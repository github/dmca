#pragma once

#ifdef SCREEN_DROPLETS

class CParticleObject;

class ScreenDroplets
{
public:
	enum {
		MAXDROPS = 2000,
		MAXDROPSMOVING = 700
	};

	class ScreenDrop
	{
	public:
		float x, y, time;		// shorts on xbox (short float?)
		float size, magnification, lifetime;	// "
		CRGBA color;
		bool active;
		bool fades;

		void Fade(void);
	};

	struct ScreenDropMoving
	{
		ScreenDrop *drop;
		float dist;
	};

	static int ms_initialised;
	static RwTexture *ms_maskTex;
	static RwTexture *ms_screenTex;

	static bool ms_enabled;
	static bool ms_movingEnabled;

	static ScreenDrop ms_drops[MAXDROPS];
	static int ms_numDrops;
	static ScreenDropMoving ms_dropsMoving[MAXDROPSMOVING];
	static int ms_numDropsMoving;

	static CVector ms_prevCamUp;
	static CVector ms_prevCamPos;
	static CVector ms_camMoveDelta;
	static float ms_camMoveDist;
	static CVector ms_screenMoveDelta;
	static float ms_screenMoveDist;
	static float ms_camUpAngle;

	static int ms_splashDuration;
	static CParticleObject *ms_splashObject;

	static void Initialise(void);
	static void InitDraw(void);
	static void Shutdown(void);
	static void Process(void);
	static void Render(void);
	static void AddToRenderList(ScreenDrop *drop);

	static void Clear(void);
	static ScreenDrop *NewDrop(float x, float y, float size, float lifetime, bool fades, int r = 255, int g = 255, int b = 255);
	static void SetMoving(ScreenDroplets::ScreenDrop *drop);
	static void FillScreen(int n);
	static void FillScreenMoving(float amount, bool isBlood = false);
	static void RegisterSplash(CParticleObject *pobj);

	static void ProcessCameraMovement(void);
	static void SprayDrops(void);
	static void NewTrace(ScreenDroplets::ScreenDropMoving *moving);
	static void MoveDrop(ScreenDropMoving *moving);
	static void ProcessMoving(void);
	static void Fade(void);
};

#endif

#pragma once

#ifdef EXTENDED_COLOURFILTER

class CPostFX
{
public:
	enum {
		POSTFX_OFF,
		POSTFX_SIMPLE,
		POSTFX_NORMAL,
		POSTFX_MOBILE
	};
	static RwRaster *pFrontBuffer;
	static RwRaster *pBackBuffer;
	static bool bJustInitialised;
	static int EffectSwitch;
	static bool MotionBlurOn;	// or use CMblur for that?
	static float Intensity;

	static void InitOnce(void);
	static void Open(RwCamera *cam);
	static void Close(void);
	static void RenderOverlayBlur(RwCamera *cam, int32 r, int32 g, int32 b, int32 a);
	static void RenderOverlaySimple(RwCamera *cam, int32 r, int32 g, int32 b, int32 a);
	static void RenderOverlaySniper(RwCamera *cam, int32 r, int32 g, int32 b, int32 a);
	static void RenderOverlayShader(RwCamera *cam, int32 r, int32 g, int32 b, int32 a);
	static void RenderMotionBlur(RwCamera *cam, uint32 blur);
	static void Render(RwCamera *cam, uint32 red, uint32 green, uint32 blue, uint32 blur, int32 type, uint32 bluralpha);
	static bool NeedBackBuffer(void);
	static bool NeedFrontBuffer(int32 type);
	static void GetBackBuffer(RwCamera *cam);
	static bool UseBlurColours(void) { return EffectSwitch != POSTFX_SIMPLE; }
};

#endif

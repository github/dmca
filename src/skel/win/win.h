
// DON'T include directly. crossplatform.h includes this if you're using D3D9 backend(win.cpp).

#if (!defined(_PLATFORM_WIN_H))
#define _PLATFORM_WIN_H

#if (!defined(RSREGSETBREAKALLOC))
#define RSREGSETBREAKALLOC(_name) /* No op */
#endif /* (!defined(RSREGSETBREAKALLOC)) */

#ifdef __DINPUT_INCLUDED__
/* platform specfic global data */
typedef struct
{
	HWND		window;
	HINSTANCE	instance;
	RwBool		fullScreen;
	RwV2d		lastMousePos;
	
	DWORD field_14;

	LPDIRECTINPUT8		 dinterface;
	LPDIRECTINPUTDEVICE8 mouse;
	LPDIRECTINPUTDEVICE8 joy1;
	LPDIRECTINPUTDEVICE8 joy2;
}
psGlobalType;

#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

enum eJoypads
{
	JOYSTICK1 = 0,
	JOYSTICK2,
	MAX_JOYSTICKS
};

enum eJoypadState
{
	JOYPAD_UNUSED,
	JOYPAD_ATTACHED,
};

struct tJoy
{
	eJoypadState m_State;
	bool         m_bInitialised;
	bool         m_bHasAxisZ;
	bool         m_bHasAxisR;
	int          m_nVendorID;
	int          m_nProductID;
};

class CJoySticks
{
public:
	tJoy m_aJoys[MAX_JOYSTICKS];
	
	CJoySticks();
	void ClearJoyInfo(int joyID);
};

extern CJoySticks AllValidWinJoys;
#endif

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

#ifdef __DINPUT_INCLUDED__
HRESULT _InputInitialise();
HRESULT CapturePad(RwInt32 padID);
void _InputAddJoyStick(LPDIRECTINPUTDEVICE8 lpDevice, INT num);
HRESULT _InputAddJoys();
HRESULT _InputGetMouseState(DIMOUSESTATE2 *state);
void _InputShutdown();
BOOL CALLBACK _InputEnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL _InputTranslateKey(RsKeyCodes *rs, UINT flag, UINT key);
BOOL _InputTranslateShiftKey(RsKeyCodes *rs, UINT key, BOOLEAN bDown);
BOOL _InputIsExtended(INT flag);
#endif

void CenterVideo(void);
void CloseClip(void);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* (!defined(_PLATFORM_WIN_H)) */

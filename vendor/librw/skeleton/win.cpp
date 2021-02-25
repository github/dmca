#ifdef _WIN32
#include <windows.h>
#include <rw.h>
#include "skeleton.h"

using namespace sk;
using namespace rw;

#ifdef RW_D3D9

#ifndef VK_OEM_NEC_EQUAL
#define VK_OEM_NEC_EQUAL 0x92
#endif

static int keymap[256];
static void
initkeymap(void)
{
	int i;
	for(i = 0; i < 256; i++)
		keymap[i] = KEY_NULL;
	keymap[VK_SPACE] = ' ';
	keymap[VK_OEM_7] = '\'';
	keymap[VK_OEM_COMMA] = ',';
	keymap[VK_OEM_MINUS] = '-';
	keymap[VK_OEM_PERIOD] = '.';
	keymap[VK_OEM_2] = '/';
	for(i = '0'; i <= '9'; i++)
		keymap[i] = i;
	keymap[VK_OEM_1] = ';';
	keymap[VK_OEM_NEC_EQUAL] = '=';
	for(i = 'A'; i <= 'Z'; i++)
		keymap[i] = i;
	keymap[VK_OEM_4] = '[';
	keymap[VK_OEM_5] = '\\';
	keymap[VK_OEM_6] = ']';
	keymap[VK_OEM_3] = '`';
	keymap[VK_ESCAPE] = KEY_ESC;
	keymap[VK_RETURN] = KEY_ENTER;
	keymap[VK_TAB] = KEY_TAB;
	keymap[VK_BACK] = KEY_BACKSP;
	keymap[VK_INSERT] = KEY_INS;
	keymap[VK_DELETE] = KEY_DEL;
	keymap[VK_RIGHT] = KEY_RIGHT;
	keymap[VK_LEFT] = KEY_LEFT;
	keymap[VK_DOWN] = KEY_DOWN;
	keymap[VK_UP] = KEY_UP;
	keymap[VK_PRIOR] = KEY_PGUP;
	keymap[VK_NEXT] = KEY_PGDN;
	keymap[VK_HOME] = KEY_HOME;
	keymap[VK_END] = KEY_END;
	keymap[VK_MODECHANGE] = KEY_CAPSLK;
	for(i = VK_F1; i <= VK_F24; i++)
		keymap[i] = i-VK_F1+KEY_F1;
	keymap[VK_LSHIFT] = KEY_LSHIFT;
	keymap[VK_LCONTROL] = KEY_LCTRL;
	keymap[VK_LMENU] = KEY_LALT;
	keymap[VK_RSHIFT] = KEY_RSHIFT;
	keymap[VK_RCONTROL] = KEY_RCTRL;
	keymap[VK_RMENU] = KEY_RALT;
}
bool running;

static void KeyUp(int key) { EventHandler(KEYUP, &key); }
static void KeyDown(int key) { EventHandler(KEYDOWN, &key); }

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int resizing = 0;
	static int buttons = 0;
	POINTS p;

	MouseState ms;
	switch(msg){
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if(wParam == VK_MENU){
			if(GetKeyState(VK_LMENU) & 0x8000) KeyDown(keymap[VK_LMENU]);
			if(GetKeyState(VK_RMENU) & 0x8000) KeyDown(keymap[VK_RMENU]);
		}else if(wParam == VK_CONTROL){
			if(GetKeyState(VK_LCONTROL) & 0x8000) KeyDown(keymap[VK_LCONTROL]);
			if(GetKeyState(VK_RCONTROL) & 0x8000) KeyDown(keymap[VK_RCONTROL]);
		}else if(wParam == VK_SHIFT){
			if(GetKeyState(VK_LSHIFT) & 0x8000) KeyDown(keymap[VK_LSHIFT]);
			if(GetKeyState(VK_RSHIFT) & 0x8000) KeyDown(keymap[VK_RSHIFT]);
		}else
			KeyDown(keymap[wParam]);
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if(wParam == VK_MENU){
			if((GetKeyState(VK_LMENU) & 0x8000) == 0) KeyUp(keymap[VK_LMENU]);
			if((GetKeyState(VK_RMENU) & 0x8000) == 0) KeyUp(keymap[VK_RMENU]);
		}else if(wParam == VK_CONTROL){
			if((GetKeyState(VK_LCONTROL) & 0x8000) == 0) KeyUp(keymap[VK_LCONTROL]);
			if((GetKeyState(VK_RCONTROL) & 0x8000) == 0) KeyUp(keymap[VK_RCONTROL]);
		}else if(wParam == VK_SHIFT){
			if((GetKeyState(VK_LSHIFT) & 0x8000) == 0) KeyUp(keymap[VK_LSHIFT]);
			if((GetKeyState(VK_RSHIFT) & 0x8000) == 0) KeyUp(keymap[VK_RSHIFT]);
		}else
			KeyUp(keymap[wParam]);
		break;

	case WM_CHAR:
		if(wParam > 0 && wParam < 0x10000)
			EventHandler(CHARINPUT, (void*)wParam);
		break;

	case WM_MOUSEMOVE:
		p = MAKEPOINTS(lParam);
		ms.posx = p.x;
		ms.posy = p.y;
		EventHandler(MOUSEMOVE, &ms);
		break;

	case WM_LBUTTONDOWN:
		buttons |= 1; goto mbtn;
	case WM_LBUTTONUP:
		buttons &= ~1; goto mbtn;
	case WM_MBUTTONDOWN:
		buttons |= 2; goto mbtn;
	case WM_MBUTTONUP:
		buttons &= ~2; goto mbtn;
	case WM_RBUTTONDOWN:
		buttons |= 4; goto mbtn;
	case WM_RBUTTONUP:
		buttons &= ~4;
	mbtn:
		ms.buttons = buttons;
		EventHandler(MOUSEBTN, &ms);
		break;

	case WM_SIZE:
		rw::Rect r;
		r.x = 0;
		r.y = 0;
		r.w = LOWORD(lParam);
		r.h = HIWORD(lParam);
		EventHandler(RESIZE, &r);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;

	case WM_QUIT:
		running = false;
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND
MakeWindow(HINSTANCE instance, int width, int height, const char *title)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "librwD3D9";
	if(!RegisterClass(&wc)){
		MessageBox(0, "RegisterClass() - FAILED", 0, 0);
		return 0;
	}

	int offx = 100;
	int offy = 100;
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	DWORD style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rect, style, FALSE);
	rect.right += -rect.left;
	rect.bottom += -rect.top;
	HWND win;
	win = CreateWindow("librwD3D9", title, style,
		offx, offy, rect.right, rect.bottom, 0, 0, instance, 0);
	if(!win){
		MessageBox(0, "CreateWindow() - FAILED", 0, 0);
		return 0;
	}
	ShowWindow(win, SW_SHOW);
	UpdateWindow(win);
	return win;
}

void
pollEvents(void)
{
	MSG msg;
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
		if(msg.message == WM_QUIT){
			running = false;
			break;
		}else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

int WINAPI
WinMain(HINSTANCE instance, HINSTANCE,
        PSTR cmdLine, int showCmd)
{
/*
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
*/

	INT64 ticks;
	INT64 ticksPerSecond;
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond))
		return 0;
	if(!QueryPerformanceCounter((LARGE_INTEGER*)&ticks))
		return 0;

#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
	args.argc = _argc;
	args.argv = _argv;
#else
	args.argc = __argc;
	args.argv = __argv;
#endif

	if(EventHandler(INITIALIZE, nil) == EVENTERROR)
		return 0;

	HWND win = MakeWindow(instance,
		sk::globals.width, sk::globals.height,
		sk::globals.windowtitle);
	if(win == 0){
		MessageBox(0, "MakeWindow() - FAILED", 0, 0);
		return 0;
	}
	engineOpenParams.window = win;
	initkeymap();

	if(EventHandler(RWINITIALIZE, nil) == EVENTERROR)
		return 0;

	INT64 lastTicks;
	QueryPerformanceCounter((LARGE_INTEGER *)&lastTicks);
	running = true;
	while((pollEvents(), running) && !globals.quit){
		QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
		float timeDelta = (float)(ticks - lastTicks)/ticksPerSecond;

		EventHandler(IDLE, &timeDelta);

		lastTicks = ticks;
	}

	EventHandler(RWTERMINATE, nil);

	return 0;
}

namespace sk {

void
SetMousePosition(int x, int y)
{
	POINT pos = { x, y };
	ClientToScreen(engineOpenParams.window, &pos);
	SetCursorPos(pos.x, pos.y);
}

}

#endif

#ifdef RW_OPENGL
int main(int argc, char *argv[]);

int WINAPI
WinMain(HINSTANCE instance, HINSTANCE,
        PSTR cmdLine, int showCmd)
{
/*
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
*/

#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
	return main(_argc, _argv);
#else
	return main(__argc, __argv);
#endif
}
#endif
#endif

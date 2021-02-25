#ifdef LIBRW_SDL2

#include <rw.h>
#include "skeleton.h"

using namespace sk;
using namespace rw;

#ifdef RW_OPENGL

SDL_Window *window;

static int keyCodeToSkKey(SDL_Keycode keycode) {
	switch (keycode) {
	case SDLK_SPACE: return ' ';
	case SDLK_QUOTE: return '\'';
	case SDLK_COMMA: return ',';
	case SDLK_MINUS: return '-';
	case SDLK_PERIOD: return '.';
	case SDLK_SLASH: return '/';

	case SDLK_0: return '0';
	case SDLK_1: return '1';
	case SDLK_2: return '2';
	case SDLK_3: return '3';
	case SDLK_4: return '4';
	case SDLK_5: return '5';
	case SDLK_6: return '6';
	case SDLK_7: return '7';
	case SDLK_8: return '8';
	case SDLK_9: return '9';

	case SDLK_SEMICOLON: return ';';
	case SDLK_EQUALS: return '=';

	case SDLK_a: return 'A';
	case SDLK_b: return 'B';
	case SDLK_c: return 'C';
	case SDLK_d: return 'D';
	case SDLK_e: return 'E';
	case SDLK_f: return 'F';
	case SDLK_g: return 'G';
	case SDLK_h: return 'H';
	case SDLK_i: return 'I';
	case SDLK_j: return 'J';
	case SDLK_k: return 'K';
	case SDLK_l: return 'L';
	case SDLK_m: return 'M';
	case SDLK_n: return 'N';
	case SDLK_o: return 'O';
	case SDLK_p: return 'P';
	case SDLK_q: return 'Q';
	case SDLK_r: return 'R';
	case SDLK_s: return 'S';
	case SDLK_t: return 'T';
	case SDLK_u: return 'U';
	case SDLK_v: return 'V';
	case SDLK_w: return 'W';
	case SDLK_x: return 'X';
	case SDLK_y: return 'Y';
	case SDLK_z: return 'Z';

	case SDLK_LEFTBRACKET: return '[';
	case SDLK_BACKSLASH: return '\\';
	case SDLK_RIGHTBRACKET: return ']';
	case SDLK_BACKQUOTE: return '`';
	case SDLK_ESCAPE: return KEY_ESC;
	case SDLK_RETURN: return KEY_ENTER;
	case SDLK_TAB: return KEY_TAB;
	case SDLK_BACKSPACE: return KEY_BACKSP;
	case SDLK_INSERT: return KEY_INS;
	case SDLK_DELETE: return KEY_DEL;
	case SDLK_RIGHT: return KEY_RIGHT;
	case SDLK_DOWN: return KEY_DOWN;
	case SDLK_UP: return KEY_UP;
	case SDLK_PAGEUP: return KEY_PGUP;
	case SDLK_PAGEDOWN: return KEY_PGDN;
	case SDLK_HOME: return KEY_HOME;
	case SDLK_END: return KEY_END;
	case SDLK_CAPSLOCK: return KEY_CAPSLK;
	case SDLK_SCROLLLOCK: return KEY_NULL;
	case SDLK_NUMLOCKCLEAR: return KEY_NULL;
	case SDLK_PRINTSCREEN: return KEY_NULL;
	case SDLK_PAUSE: return KEY_NULL;

	case SDLK_F1: return KEY_F1;
	case SDLK_F2: return KEY_F2;
	case SDLK_F3: return KEY_F3;
	case SDLK_F4: return KEY_F4;
	case SDLK_F5: return KEY_F5;
	case SDLK_F6: return KEY_F6;
	case SDLK_F7: return KEY_F7;
	case SDLK_F8: return KEY_F8;
	case SDLK_F9: return KEY_F9;
	case SDLK_F10: return KEY_F10;
	case SDLK_F11: return KEY_F11;
	case SDLK_F12: return KEY_F12;
	case SDLK_F13: return KEY_NULL;
	case SDLK_F14: return KEY_NULL;
	case SDLK_F15: return KEY_NULL;
	case SDLK_F16: return KEY_NULL;
	case SDLK_F17: return KEY_NULL;
	case SDLK_F18: return KEY_NULL;
	case SDLK_F19: return KEY_NULL;
	case SDLK_F20: return KEY_NULL;
	case SDLK_F21: return KEY_NULL;
	case SDLK_F22: return KEY_NULL;
	case SDLK_F23: return KEY_NULL;
	case SDLK_F24: return KEY_NULL;

	case SDLK_KP_0: return KEY_NULL;
	case SDLK_KP_1: return KEY_NULL;
	case SDLK_KP_2: return KEY_NULL;
	case SDLK_KP_3: return KEY_NULL;
	case SDLK_KP_4: return KEY_NULL;
	case SDLK_KP_5: return KEY_NULL;
	case SDLK_KP_6: return KEY_NULL;
	case SDLK_KP_7: return KEY_NULL;
	case SDLK_KP_8: return KEY_NULL;
	case SDLK_KP_9: return KEY_NULL;
	case SDLK_KP_DECIMAL: return KEY_NULL;
	case SDLK_KP_DIVIDE: return KEY_NULL;
	case SDLK_KP_MULTIPLY: return KEY_NULL;
	case SDLK_KP_MINUS: return KEY_NULL;
	case SDLK_KP_PLUS: return KEY_NULL;
	case SDLK_KP_ENTER: return KEY_NULL;
	case SDLK_KP_EQUALS: return KEY_NULL;

	case SDLK_LSHIFT: return KEY_LSHIFT;
	case SDLK_LCTRL: return KEY_LCTRL;
	case SDLK_LALT: return KEY_LALT;
	case SDLK_LGUI: return KEY_NULL;
	case SDLK_RSHIFT: return KEY_RSHIFT;
	case SDLK_RCTRL: return KEY_RCTRL;
	case SDLK_RALT: return KEY_RALT;
	case SDLK_RGUI: return KEY_NULL;
	case SDLK_MENU: return KEY_NULL;
	}
	return KEY_NULL;
}

#if 0
static void
keypress(SDL_Window *window, int key, int scancode, int action, int mods)
{
	if(key >= 0 && key <= GLFW_KEY_LAST){
		if(action == GLFW_RELEASE) KeyUp(keymap[key]);
		else if(action == GLFW_PRESS)   KeyDown(keymap[key]);
		else if(action == GLFW_REPEAT)  KeyDown(keymap[key]);
	}
}

static void
charinput(GLFWwindow *window, unsigned int c)
{
	EventHandler(CHARINPUT, (void*)(uintptr)c);
}

static void
resize(GLFWwindow *window, int w, int h)
{
	rw::Rect r;
	r.x = 0;
	r.y = 0;
	r.w = w;
	r.h = h;
	EventHandler(RESIZE, &r);
}

static void
mousebtn(GLFWwindow *window, int button, int action, int mods)
{
	static int buttons = 0;
	sk::MouseState ms;

	switch(button){
	case GLFW_MOUSE_BUTTON_LEFT:
		if(action == GLFW_PRESS)
			buttons |= 1;
		else
			buttons &= ~1;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		if(action == GLFW_PRESS)
			buttons |= 2;
		else
			buttons &= ~2;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if(action == GLFW_PRESS)
			buttons |= 4;
		else
			buttons &= ~4;
		break;
	}

	sk::MouseState ms;
	ms.buttons = buttons;
	EventHandler(MOUSEBTN, &ms);
}
#endif

enum mousebutton {
BUTTON_LEFT = 0x1,
BUTTON_MIDDLE = 0x2,
BUTTON_RIGHT = 0x4,
};

int
main(int argc, char *argv[])
{
	args.argc = argc;
	args.argv = argv;

	if(EventHandler(INITIALIZE, nil) == EVENTERROR)
		return 0;

	engineOpenParams.width = sk::globals.width;
	engineOpenParams.height = sk::globals.height;
	engineOpenParams.windowtitle = sk::globals.windowtitle;
	engineOpenParams.window = &window;

	if(EventHandler(RWINITIALIZE, nil) == EVENTERROR)
		return 0;

	float lastTime = SDL_GetTicks();
	SDL_Event event;
	int mouseButtons = 0;

	SDL_StartTextInput();

	while(!sk::globals.quit){
		while(SDL_PollEvent(&event)){
			switch(event.type){
			case SDL_QUIT:
				sk::globals.quit = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					rw::Rect r;
					SDL_GetWindowPosition(window, &r.x, &r.y);
					r.w = event.window.data1;
					r.h = event.window.data2;
					EventHandler(RESIZE, &r);
					break;
				}
				}
				break;
			case SDL_KEYUP: {
				int c = keyCodeToSkKey(event.key.keysym.sym);
				EventHandler(KEYUP, &c);
				break;
			}
			case SDL_KEYDOWN: {
				int c = keyCodeToSkKey(event.key.keysym.sym);
				EventHandler(KEYDOWN, &c);
				break;
			}
			case SDL_TEXTINPUT: {
				char *c = event.text.text;
				while (int ci = *c) {
					EventHandler(CHARINPUT, (void*)(uintptr)ci);
					++c;
				}
				break;
			}
			case SDL_MOUSEMOTION: {
				sk::MouseState ms;
				ms.posx = event.motion.x;
				ms.posy = event.motion.y;
				EventHandler(MOUSEMOVE, &ms);
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				switch (event.button.button) {
				case SDL_BUTTON_LEFT: mouseButtons |= BUTTON_LEFT; break;
				case SDL_BUTTON_MIDDLE: mouseButtons |= BUTTON_MIDDLE; break;
				case SDL_BUTTON_RIGHT: mouseButtons |= BUTTON_RIGHT; break;
				}
				sk::MouseState ms;
				ms.buttons = mouseButtons;
				EventHandler(MOUSEBTN, &ms);
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				switch (event.button.button) {
				case SDL_BUTTON_LEFT: mouseButtons &= ~BUTTON_LEFT; break;
				case SDL_BUTTON_MIDDLE: mouseButtons &= ~BUTTON_MIDDLE; break;
				case SDL_BUTTON_RIGHT: mouseButtons &= ~BUTTON_RIGHT; break;
				}
				sk::MouseState ms;
				ms.buttons = mouseButtons;
				EventHandler(MOUSEBTN, &ms);
				break;
			}
			}
		}
		float currTime  = SDL_GetTicks();
		float timeDelta = (currTime - lastTime) * 0.001f;

		EventHandler(IDLE, &timeDelta);

		lastTime = currTime;
	}

	SDL_StopTextInput();

	EventHandler(RWTERMINATE, nil);

	return 0;
}

namespace sk {

void
SetMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(*engineOpenParams.window, x, y);
}

}

#endif
#endif

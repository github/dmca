#include <rw.h>
#include <skeleton.h>
#include <assert.h>

#include "imgui/imgui.h"
#include "imgui_impl_rw.h"

using namespace rw::RWDEVICE;

static rw::Texture *g_FontTexture;
static Im2DVertex *g_vertbuf;
static int g_vertbufSize;

void
ImGui_ImplRW_RenderDrawLists(ImDrawData* draw_data)
{
	ImGuiIO &io = ImGui::GetIO();

	// minimized
	if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
		return;

	if(g_vertbuf == nil || g_vertbufSize < draw_data->TotalVtxCount){
		if(g_vertbuf){
			rwFree(g_vertbuf);
			g_vertbuf = nil;
		}
		g_vertbufSize = draw_data->TotalVtxCount + 5000;
		g_vertbuf = rwNewT(Im2DVertex, g_vertbufSize, 0);
	}

	float xoff = 0.0f;
	float yoff = 0.0f;
#ifdef RWHALFPIXEL
	xoff = -0.5;
	yoff = 0.5;
#endif

	rw::Camera *cam = (rw::Camera*)rw::engine->currentCamera;
	Im2DVertex *vtx_dst = g_vertbuf;
	float recipZ = 1.0f/cam->nearPlane;
	for(int n = 0; n < draw_data->CmdListsCount; n++){
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		const ImDrawVert *vtx_src = cmd_list->VtxBuffer.Data;
		for(int i = 0; i < cmd_list->VtxBuffer.Size; i++){
			vtx_dst[i].setScreenX(vtx_src[i].pos.x + xoff);
			vtx_dst[i].setScreenY(vtx_src[i].pos.y + yoff);
			vtx_dst[i].setScreenZ(rw::im2d::GetNearZ());
			vtx_dst[i].setCameraZ(cam->nearPlane);
			vtx_dst[i].setRecipCameraZ(recipZ);
			vtx_dst[i].setColor(vtx_src[i].col&0xFF, vtx_src[i].col>>8 & 0xFF, vtx_src[i].col>>16 & 0xFF, vtx_src[i].col>>24 & 0xFF);
			vtx_dst[i].setU(vtx_src[i].uv.x, recipZ);
			vtx_dst[i].setV(vtx_src[i].uv.y, recipZ);
		}
		vtx_dst += cmd_list->VtxBuffer.Size;
	}

	rw::SetRenderState(rw::VERTEXALPHA, 1);
	rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
	rw::SetRenderState(rw::DESTBLEND, rw::BLENDINVSRCALPHA);
	rw::SetRenderState(rw::ZTESTENABLE, 0);

	int vtx_offset = 0;
	for(int n = 0; n < draw_data->CmdListsCount; n++){
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		int idx_offset = 0;
		for(int i = 0; i < cmd_list->CmdBuffer.Size; i++){
			const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[i];
			if(pcmd->UserCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			else{
				rw::Texture *tex = (rw::Texture*)pcmd->TextureId;
				if(tex && tex->raster){
					rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
					rw::SetRenderState(rw::TEXTUREADDRESSU, tex->getAddressU());
					rw::SetRenderState(rw::TEXTUREADDRESSV, tex->getAddressV());
					rw::SetRenderState(rw::TEXTUREFILTER, tex->getFilter());
				}else
					rw::SetRenderStatePtr(rw::TEXTURERASTER, nil);
				rw::im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST,
					g_vertbuf+vtx_offset, cmd_list->VtxBuffer.Size,
					cmd_list->IdxBuffer.Data+idx_offset, pcmd->ElemCount);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}

bool
ImGui_ImplRW_Init(void)
{
	using namespace sk;
	ImGuiIO &io = ImGui::GetIO();

	io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = KEY_PGUP;
	io.KeyMap[ImGuiKey_PageDown] = KEY_PGDN;
	io.KeyMap[ImGuiKey_Home] = KEY_HOME;
	io.KeyMap[ImGuiKey_End] = KEY_END;
	io.KeyMap[ImGuiKey_Delete] = KEY_DEL;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSP;
	io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = KEY_ESC;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = ImGui_ImplRW_RenderDrawLists;

	return true;
}

void
ImGui_ImplRW_Shutdown(void)
{
}

static bool
ImGui_ImplRW_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO &io = ImGui::GetIO();
	unsigned char *pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, nil);

	rw::Image *image;
	image = rw::Image::create(width, height, 32);
	image->allocate();
	for(int y = 0; y < height; y++)
		memcpy(image->pixels + image->stride*y, pixels + width*4* y, width*4);
	g_FontTexture = rw::Texture::create(rw::Raster::createFromImage(image));
	g_FontTexture->setFilter(rw::Texture::LINEAR);
	image->destroy();
	
	// Store our identifier
	io.Fonts->TexID = (void*)g_FontTexture;

	return true;
}

bool
ImGui_ImplRW_CreateDeviceObjects()
{
//	if(!g_pd3dDevice)
//		return false;
	if(!ImGui_ImplRW_CreateFontsTexture())
		return false;
	return true;
}

void
ImGui_ImplRW_NewFrame(float timeDelta)
{
	if(!g_FontTexture)
		ImGui_ImplRW_CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();

	io.DisplaySize = ImVec2(sk::globals.width, sk::globals.height);
	io.DeltaTime = timeDelta;

	io.KeyCtrl = io.KeysDown[sk::KEY_LCTRL] || io.KeysDown[sk::KEY_RCTRL];
	io.KeyShift = io.KeysDown[sk::KEY_LSHIFT] || io.KeysDown[sk::KEY_RSHIFT];
	io.KeyAlt = io.KeysDown[sk::KEY_LALT] || io.KeysDown[sk::KEY_RALT];
	io.KeySuper = false;

	if(io.WantMoveMouse)
		sk::SetMousePosition(io.MousePos.x, io.MousePos.y);

	ImGui::NewFrame();
}

sk::EventStatus
ImGuiEventHandler(sk::Event e, void *param)
{
	using namespace sk;

	ImGuiIO &io = ImGui::GetIO();
	MouseState *ms;
	uint c;

	switch(e){
	case KEYDOWN:
		c = *(int*)param;
		if(c < 256)
			io.KeysDown[c] = 1;
		return EVENTPROCESSED;
	case KEYUP:
		c = *(int*)param;
		if(c < 256)
			io.KeysDown[c] = 0;
		return EVENTPROCESSED;
	case CHARINPUT:
		c = (uint)(uintptr)param;
		io.AddInputCharacter((unsigned short)c);
		return EVENTPROCESSED;
	case MOUSEMOVE:
		ms = (MouseState*)param;
		io.MousePos.x = ms->posx;
		io.MousePos.y = ms->posy;
		return EVENTPROCESSED;
	case MOUSEBTN:
		ms = (MouseState*)param;
		io.MouseDown[0] = !!(ms->buttons & 1);
		io.MouseDown[2] = !!(ms->buttons & 2);
		io.MouseDown[1] = !!(ms->buttons & 4);
		return EVENTPROCESSED;
	}
	return EVENTPROCESSED;
}

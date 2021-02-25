#include <rw.h>
#include <skeleton.h>
#include <assert.h>

rw::V3d zero = { 0.0f, 0.0f, 0.0f };
struct SceneGlobals {
	rw::World *world;
	rw::Camera *camera;
} Scene;
rw::EngineOpenParams engineOpenParams;

void
Init(void)
{
	sk::globals.windowtitle = "ImGui test";
	sk::globals.width = 1280;
	sk::globals.height = 800;
	sk::globals.quit = 0;
}

bool
attachPlugins(void)
{
	rw::ps2::registerPDSPlugin(40);
	rw::ps2::registerPluginPDSPipes();

	rw::registerMeshPlugin();
	rw::registerNativeDataPlugin();
	rw::registerAtomicRightsPlugin();
	rw::registerMaterialRightsPlugin();
	rw::xbox::registerVertexFormatPlugin();
	rw::registerSkinPlugin();
	rw::registerUserDataPlugin();
	rw::registerHAnimPlugin();
	rw::registerMatFXPlugin();
	rw::registerUVAnimPlugin();
	rw::ps2::registerADCPlugin();
	return true;
}

bool
InitRW(void)
{
//	rw::platform = rw::PLATFORM_D3D8;
	if(!sk::InitRW())
		return false;

	Scene.world = rw::World::create();

	rw::Light *ambient = rw::Light::create(rw::Light::AMBIENT);
	ambient->setColor(0.2f, 0.2f, 0.2f);
	Scene.world->addLight(ambient);

	rw::V3d xaxis = { 1.0f, 0.0f, 0.0f };
	rw::Light *direct = rw::Light::create(rw::Light::DIRECTIONAL);
	direct->setColor(0.8f, 0.8f, 0.8f);
	direct->setFrame(rw::Frame::create());
	direct->getFrame()->rotate(&xaxis, 180.0f, rw::COMBINEREPLACE);
	Scene.world->addLight(direct);

	Scene.camera = sk::CameraCreate(sk::globals.width, sk::globals.height, 1);
	Scene.world->addCamera(Scene.camera);

	ImGui_ImplRW_Init();
	ImGui::StyleColorsClassic();

	return true;
}

void
Draw(float timeDelta)
{
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	rw::RGBA clearcol = rw::makeRGBA(clear_color.x*255, clear_color.y*255, clear_color.z*255, clear_color.w*255);
	Scene.camera->clear(&clearcol, rw::Camera::CLEARIMAGE|rw::Camera::CLEARZ);
	Scene.camera->beginUpdate();

	ImGui_ImplRW_NewFrame(timeDelta);

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");                           // Some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats as a color
		if(ImGui::Button("Demo Window"))                       // Use buttons to toggle our bools. We could use Checkbox() as well.
			show_demo_window ^= 1;
		if(ImGui::Button("Another Window"))
			show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name the window.
	if(show_another_window){
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		ImGui::End();
	}

        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow().
	if(show_demo_window){
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&show_demo_window);
	}


	ImGui::EndFrame();
	ImGui::Render();

	Scene.camera->endUpdate();
	Scene.camera->showRaster(0);
}


void
KeyUp(int key)
{
}

void
KeyDown(int key)
{
	switch(key){
	case sk::KEY_ESC:
		sk::globals.quit = 1;
		break;
	}
}

sk::EventStatus
AppEventHandler(sk::Event e, void *param)
{
	using namespace sk;
	Rect *r;

	ImGuiEventHandler(e, param);

	switch(e){
	case INITIALIZE:
		Init();
		return EVENTPROCESSED;
	case RWINITIALIZE:
		return ::InitRW() ? EVENTPROCESSED : EVENTERROR;
	case PLUGINATTACH:
		return attachPlugins() ? EVENTPROCESSED : EVENTERROR;
	case KEYDOWN:
		KeyDown(*(int*)param);
		return EVENTPROCESSED;
	case KEYUP:
		KeyUp(*(int*)param);
		return EVENTPROCESSED;
	case RESIZE:
		r = (Rect*)param;
		// TODO: register when we're minimized
		if(r->w == 0) r->w = 1;
		if(r->h == 0) r->h = 1;

		sk::globals.width = r->w;
		sk::globals.height = r->h;
		// TODO: set aspect ratio
		if(Scene.camera)
			sk::CameraSize(Scene.camera, r);
		break;
	case IDLE:
		Draw(*(float*)param);
		return EVENTPROCESSED;
	}
	return sk::EVENTNOTPROCESSED;
}

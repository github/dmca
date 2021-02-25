#include <rw.h>
#include <skeleton.h>
#include <assert.h>

rw::V3d zero = { 0.0f, 0.0f, 0.0f };
struct SceneGlobals {
	rw::World *world;
	rw::Camera *camera;
} Scene;
rw::EngineOpenParams engineOpenParams;
float FOV = 70.0f;

rw::V3d Xaxis = { 1.0f, 0.0, 0.0f };
rw::V3d Yaxis = { 0.0f, 1.0, 0.0f };
rw::V3d Zaxis = { 0.0f, 0.0, 1.0f };

rw::Light *BaseAmbientLight;
bool BaseAmbientLightOn;

rw::Light *CurrentLight;
rw::Light *AmbientLight;
rw::Light *PointLight;
rw::Light *DirectLight;
rw::Light *SpotLight;
rw::Light *SpotSoftLight;

float LightRadius = 100.0f;
float LightConeAngle = 45.0f;
rw::V3d LightPos = {0.0f, 0.0f, 75.0f};

void
Init(void)
{
	sk::globals.windowtitle = "Light test";
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

rw::Light*
CreateBaseAmbientLight(void)
{
	rw::Light *light = rw::Light::create(rw::Light::AMBIENT);
	assert(light);
	light->setColor(0.5f, 0.5f, 0.5f);
	return light;
}

rw::Light*
CreateAmbientLight(void)
{
	return rw::Light::create(rw::Light::AMBIENT);
}

rw::Light*
CreateDirectLight(void)
{
	rw::Light *light = rw::Light::create(rw::Light::DIRECTIONAL);
	assert(light);
	rw::Frame *frame = rw::Frame::create();
	assert(frame);
	frame->rotate(&Xaxis, 45.0f, rw::COMBINEREPLACE);
	rw::V3d pos = LightPos;
	frame->translate(&pos, rw::COMBINEPOSTCONCAT);
	light->setFrame(frame);
	return light;
}

rw::Light*
CreatePointLight(void)
{
	rw::Light *light = rw::Light::create(rw::Light::POINT);
	assert(light);
	light->radius = LightRadius;
	rw::Frame *frame = rw::Frame::create();
	assert(frame);
	rw::V3d pos = LightPos;
	frame->translate(&pos, rw::COMBINEREPLACE);
	light->setFrame(frame);
	return light;
}

rw::Light*
CreateSpotLight(void)
{
	rw::Light *light = rw::Light::create(rw::Light::SPOT);
	assert(light);
	light->radius = LightRadius;
	light->setAngle(LightConeAngle/180.0f*M_PI);
	rw::Frame *frame = rw::Frame::create();
	assert(frame);
	frame->rotate(&Xaxis, 45.0f, rw::COMBINEREPLACE);
	rw::V3d pos = LightPos;
	frame->translate(&pos, rw::COMBINEPOSTCONCAT);
	light->setFrame(frame);
	return light;
}

rw::Light*
CreateSpotSoftLight(void)
{
	rw::Light *light = rw::Light::create(rw::Light::SOFTSPOT);
	assert(light);
	light->radius = LightRadius;
	light->setAngle(LightConeAngle/180.0f*M_PI);
	rw::Frame *frame = rw::Frame::create();
	assert(frame);
	frame->rotate(&Xaxis, 45.0f, rw::COMBINEREPLACE);
	rw::V3d pos = LightPos;
	frame->translate(&pos, rw::COMBINEPOSTCONCAT);
	light->setFrame(frame);
	return light;
}

bool
CreateTestScene(rw::World *world)
{
	rw::Clump *clump;
	rw::StreamFile in;
	const char *filename = "checker.dff";
	if(in.open(filename, "rb") == NULL){
		printf("couldn't open file\n");
		return false;
	}
	if(!rw::findChunk(&in, rw::ID_CLUMP, NULL, NULL))
		return false;
	clump = rw::Clump::streamRead(&in);
	in.close();
	if(clump == nil)
		return false;

	rw::Clump *clone;
	rw::Frame *clumpFrame;
	rw::V3d pos;
	float zOffset = 75.0f;

	// Bottom panel
	clumpFrame = clump->getFrame();
	clumpFrame->rotate(&Xaxis, 90.0f, rw::COMBINEREPLACE);

	pos.x = 0.0f;
	pos.y = -25.0f;
	pos.z = zOffset;
	clumpFrame->translate(&pos, rw::COMBINEPOSTCONCAT);

	// only need to add once
	world->addClump(clump);

	// Top panel
	clone = clump->clone();
	clumpFrame = clone->getFrame();
	clumpFrame->rotate(&Xaxis, -90.0f, rw::COMBINEREPLACE);

	pos.x = 0.0f;
	pos.y = 25.0f;
	pos.z = zOffset;
	clumpFrame->translate(&pos, rw::COMBINEPOSTCONCAT);

	// Left panel
	clone = clump->clone();
	clumpFrame = clone->getFrame();
	clumpFrame->rotate(&Xaxis, 0.0f, rw::COMBINEREPLACE);
	clumpFrame->rotate(&Yaxis, 90.0f, rw::COMBINEPOSTCONCAT);

	pos.x = 25.0f;
	pos.y = 0.0f;
	pos.z = zOffset;
	clumpFrame->translate(&pos, rw::COMBINEPOSTCONCAT);

	// Right panel
	clone = clump->clone();
	clumpFrame = clone->getFrame();
	clumpFrame->rotate(&Xaxis, 0.0f, rw::COMBINEREPLACE);
	clumpFrame->rotate(&Yaxis, -90.0f, rw::COMBINEPOSTCONCAT);

	pos.x = -25.0f;
	pos.y = 0.0f;
	pos.z = zOffset;
	clumpFrame->translate(&pos, rw::COMBINEPOSTCONCAT);

	// Back panel
	clone = clump->clone();
	clumpFrame = clone->getFrame();
	clumpFrame->rotate(&Xaxis, 0.0f, rw::COMBINEREPLACE);

	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = zOffset + 25.0f;
	clumpFrame->translate(&pos, rw::COMBINEPOSTCONCAT);

	return 1;
}

bool
InitRW(void)
{
//	rw::platform = rw::PLATFORM_D3D8;
	if(!sk::InitRW())
		return false;

	Scene.world = rw::World::create();

	BaseAmbientLight = CreateBaseAmbientLight();
	AmbientLight = CreateAmbientLight();
	DirectLight = CreateDirectLight();
	PointLight = CreatePointLight();
	SpotLight = CreateSpotLight();
	SpotSoftLight = CreateSpotSoftLight();

	Scene.camera = sk::CameraCreate(sk::globals.width, sk::globals.height, 1);
	Scene.camera->setNearPlane(0.1f);
	Scene.camera->setFarPlane(300.0f);
	Scene.camera->setFOV(FOV, (float)sk::globals.width/sk::globals.height);
	Scene.world->addCamera(Scene.camera);

	CreateTestScene(Scene.world);

	ImGui_ImplRW_Init();
	ImGui::StyleColorsClassic();

	return true;
}

void
SwitchToLight(rw::Light *light)
{
	if(CurrentLight)
		Scene.world->removeLight(CurrentLight);
	CurrentLight = light;
	Scene.world->addLight(CurrentLight);
}

void
Gui(void)
{
//	ImGui::ShowDemoWindow(&show_demo_window);

	static bool showLightWindow = true;
	ImGui::Begin("Lights", &showLightWindow);
	static int lightswitch = 0;
	if(ImGui::RadioButton("Light Off", &lightswitch, 0)){
		if(CurrentLight)
			Scene.world->removeLight(CurrentLight);
		CurrentLight = nil;
	}
	if(ImGui::RadioButton("Ambient Light", &lightswitch, 1)){
		SwitchToLight(AmbientLight);
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("Directional Light", &lightswitch, 2)){
		SwitchToLight(DirectLight);
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("Point Light", &lightswitch, 3)){
		SwitchToLight(PointLight);
	}
	if(ImGui::RadioButton("Spot Light", &lightswitch, 4)){
		SwitchToLight(SpotLight);
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("Soft Spot Light", &lightswitch, 5)){
		SwitchToLight(SpotSoftLight);
	}
	ImGui::End();
}

void
Draw(float timeDelta)
{
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	rw::RGBA clearcol = rw::makeRGBA(clear_color.x*255, clear_color.y*255, clear_color.z*255, clear_color.w*255);
	Scene.camera->clear(&clearcol, rw::Camera::CLEARIMAGE|rw::Camera::CLEARZ);
	Scene.camera->beginUpdate();

	ImGui_ImplRW_NewFrame(timeDelta);

	Scene.world->render();

	Gui();

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
		if(Scene.camera){
			sk::CameraSize(Scene.camera, r);
			Scene.camera->setFOV(FOV, (float)sk::globals.width/sk::globals.height);
		}
		break;
	case IDLE:
		Draw(*(float*)param);
		return EVENTPROCESSED;
	}
	return sk::EVENTNOTPROCESSED;
}

#include <rw.h>
#include <skeleton.h>
#include "camera.h"
#include <assert.h>

rw::V3d zero = { 0.0f, 0.0f, 0.0f };
Camera *camera;
struct SceneGlobals {
	rw::World *world;
	rw::Camera *camera;
	rw::Clump *clump;
} Scene;
rw::Texture *tex, *tex2;
rw::Raster *testras;
rw::EngineOpenParams engineOpenParams;

rw::Texture *frontbuffer;

bool dosoftras = 0;

namespace gen {
void tlTest(rw::Clump *clump);
}
void genIm3DTransform(void *vertices, rw::int32 numVertices, rw::Matrix *xform);
void genIm3DRenderIndexed(rw::PrimitiveType prim, void *indices, rw::int32 numIndices);
void genIm3DEnd(void);
void initFont(void);
void printScreen(const char *s, float x, float y);

void initsplines(void);
void rendersplines(void);

rw::Charset *testfont;

//#include <Windows.h>

void
Init(void)
{
//	AllocConsole();
//	freopen("CONIN$", "r", stdin);
//	freopen("CONOUT$", "w", stdout);
//	freopen("CONOUT$", "w", stderr);

	sk::globals.windowtitle = "Clump viewer";
	sk::globals.width = 640;
	sk::globals.height = 448;
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

void
dumpUserData(rw::UserDataArray *ar)
{
	int i;
	printf("name: %s\n", ar->name);
	for(i = 0; i < ar->numElements; i++){
		switch(ar->datatype){
		case rw::USERDATAINT:
			printf("	%d\n", ar->getInt(i));
			break;
		case rw::USERDATAFLOAT:
			printf("	%f\n", ar->getFloat(i));
			break;
		case rw::USERDATASTRING:
			printf("	%s\n", ar->getString(i));
			break;
		}
	}
}

static rw::Frame*
dumpFrameUserDataCB(rw::Frame *f, void*)
{
	using namespace rw;
	int32 i;
	UserDataArray *ar;
	int32 n = UserDataArray::frameGetCount(f);
	for(i = 0; i < n; i++){
		ar = UserDataArray::frameGet(f, i);
		dumpUserData(ar);
	}
	f->forAllChildren(dumpFrameUserDataCB, nil);
	return f;
}

void
dumpUserData(rw::Clump *clump)
{
	printf("Frames\n");
	dumpFrameUserDataCB(clump->getFrame(), nil);
}

static rw::Frame*
getHierCB(rw::Frame *f, void *data)
{
	using namespace rw;
	HAnimData *hd = rw::HAnimData::get(f);
	if(hd->hierarchy){
		*(HAnimHierarchy**)data = hd->hierarchy;
		return nil;
	}
	f->forAllChildren(getHierCB, data);
	return f;
}

rw::HAnimHierarchy*
getHAnimHierarchyFromClump(rw::Clump *clump)
{
	using namespace rw;
	HAnimHierarchy *hier = nil;
	getHierCB(clump->getFrame(), &hier);
	return hier;
}

void
setupAtomic(rw::Atomic *atomic)
{
	using namespace rw;
	// just remove pipelines that we can't handle for now
//	if(atomic->pipeline && atomic->pipeline->platform != rw::platform)
		atomic->pipeline = NULL;

	// Attach hierarchy to atomic if we're skinned
	HAnimHierarchy *hier = getHAnimHierarchyFromClump(atomic->clump);
	if(hier)
		Skin::setHierarchy(atomic, hier);
}

static void
initHierFromFrames(rw::HAnimHierarchy *hier)
{
	using namespace rw;
	int32 i;
	for(i = 0; i < hier->numNodes; i++){
		if(hier->nodeInfo[i].frame){
			hier->matrices[hier->nodeInfo[i].index] = *hier->nodeInfo[i].frame->getLTM();
		}else
			assert(0);
	}
}

void
setupClump(rw::Clump *clump)
{
	using namespace rw;
	HAnimHierarchy *hier = getHAnimHierarchyFromClump(clump);
	if(hier){
		hier->attach();
		initHierFromFrames(hier);
	}

	FORLIST(lnk, clump->atomics){
		rw::Atomic *a = rw::Atomic::fromClump(lnk);
		setupAtomic(a);
	}
}

#define MUL(x, y) ((x)*(y)/255)

int
calcVCfx(int fb, int col, int a, int iter)
{
	int prev = fb;
	int col2 = col*2;
	if(col2 > 255) col2 = 255;
	for(int i = 0; i < iter; i++){
		int tmp = MUL(fb, 255-a) + MUL(MUL(prev, col2), a);
		tmp += MUL(prev, col);
		tmp += MUL(prev, col);
		prev = tmp > 255 ? 255 : tmp;
	}
	return prev;
}

int
calcIIIfx(int fb, int col, int a, int iter)
{
	int prev = fb;
	for(int i = 0; i < iter; i++){
		int tmp = MUL(fb, 255-a) + MUL(MUL(prev, col), a);
		prev = tmp > 255 ? 255 : tmp;
	}
	return prev;
}

void
postfxtest(void)
{
	rw::Image *img = rw::Image::create(256, 256, 32);
	img->allocate();
	int x, y;
	int iter;
	static char filename[100];
	for(iter = 0; iter < 10; iter++){
		for(y = 0; y < 256; y++)
			for(x = 0; x < 256; x++){
				int res = calcVCfx(y, x, 30, iter);
//				int res = calcIIIfx(y, x, 30, iter);
				if(0 && res == y){
					img->pixels[y*img->stride + x*img->bpp + 0] = 255;
					img->pixels[y*img->stride + x*img->bpp + 1] = 0;
					img->pixels[y*img->stride + x*img->bpp + 2] = 0;
				}else{
					img->pixels[y*img->stride + x*img->bpp + 0] = res;
					img->pixels[y*img->stride + x*img->bpp + 1] = res;
					img->pixels[y*img->stride + x*img->bpp + 2] = res;
				}
				img->pixels[y*img->stride + x*img->bpp + 3] = 255;
			}
		sprintf(filename, "vcfx_%02d.bmp", iter);
//		sprintf(filename, "iiifx_%02d.bmp", iter);
		rw::writeBMP(img, filename);
	}
	exit(0);
}

bool
InitRW(void)
{
//	rw::platform = rw::PLATFORM_D3D8;
	if(!sk::InitRW())
		return false;

	rw::d3d::isP8supported = false;

//	postfxtest();

	initFont();

	rw::RGBA foreground = { 255, 255, 0, 255 };
	rw::RGBA background = { 0, 0, 0, 0 };
	rw::Charset::open();
	testfont = rw::Charset::create(&foreground, &background);
	assert(testfont);
	foreground.blue = 255.0f;
	testfont->setColors(&foreground, &background);

	tex = rw::Texture::read("maze", nil);
	tex2 = rw::Texture::read("checkers", nil);

	const char *filename = "teapot2.dff";
	if(sk::args.argc > 1)
		filename = sk::args.argv[1];
	rw::StreamFile in;
	if(in.open(filename, "rb") == NULL){
		printf("couldn't open file\n");
		return false;
	}
	rw::findChunk(&in, rw::ID_CLUMP, NULL, NULL);
	Scene.clump = rw::Clump::streamRead(&in);
	assert(Scene.clump);
	in.close();

	// TEST - Set texture to the all materials of the clump
//	FORLIST(lnk, Scene.clump->atomics){
//		rw::Atomic *a = rw::Atomic::fromClump(lnk);
//		for(int i = 0; i < a->geometry->matList.numMaterials; i++)
//			a->geometry->matList.materials[i]->setTexture(tex);
//	}

	Scene.clump->getFrame()->translate(&zero, rw::COMBINEREPLACE);

	dumpUserData(Scene.clump);
	setupClump(Scene.clump);

	Scene.world = rw::World::create();

	rw::Light *ambient = rw::Light::create(rw::Light::AMBIENT);
	ambient->setColor(0.3f, 0.3f, 0.3f);
	Scene.world->addLight(ambient);

	rw::V3d xaxis = { 1.0f, 0.0f, 0.0f };
	rw::Light *direct = rw::Light::create(rw::Light::DIRECTIONAL);
	direct->setColor(0.8f, 0.8f, 0.8f);
	direct->setFrame(rw::Frame::create());
	direct->getFrame()->rotate(&xaxis, 180.0f, rw::COMBINEREPLACE);
	Scene.world->addLight(direct);

	camera = new Camera;
	Scene.camera = sk::CameraCreate(sk::globals.width, sk::globals.height, 1);
	camera->m_rwcam = Scene.camera;
	camera->m_aspectRatio = 640.0f/448.0f;
//	camera->m_near = 0.5f;
	camera->m_near = 1.5f;
//	camera->m_far = 450.0f;
	camera->m_far = 15.0f;
	camera->m_target.set(0.0f, 0.0f, 0.0f);
	camera->m_position.set(0.0f, -10.0f, 0.0f);
//	camera->setPosition(Vec3(0.0f, 5.0f, 0.0f));
//	camera->setPosition(Vec3(0.0f, -70.0f, 0.0f));
//	camera->setPosition(Vec3(0.0f, -1.0f, 3.0f));
	camera->update();

	Scene.world->addCamera(camera->m_rwcam);

	initsplines();

	return true;
}

void
im2dtest(void)
{
	using namespace rw::RWDEVICE;
	int i;
	static struct
	{
		float x, y;
		rw::uint8 r, g, b, a;
		float u, v;
	} vs[4] = {
		{   0.0f,   0.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ 640.0f,   0.0f,   0, 255, 0, 128,    1.0f, 0.0f },
		{   0.0f, 448.0f,   0, 0, 255, 128,    0.0f, 1.0f },
		{ 640.0f, 448.0f,   0, 255, 255, 128,  1.0f, 1.0f },
/*
		{   0.0f,   0.0f,   255, 0, 0, 128,    0.0f, 1.0f },
		{ 640.0f,   0.0f,   0, 255, 0, 128,    0.0f, 0.0f },
		{   0.0f, 448.0f,   0, 0, 255, 128,    1.0f, 1.0f },
		{ 640.0f, 448.0f,   0, 255, 255, 128,  1.0f, 0.0f },
*/
	};
	Im2DVertex verts[4];
	static short indices[] = {
		0, 1, 2, 3
	};

	float recipZ = 1.0f/Scene.camera->nearPlane;
	for(i = 0; i < 4; i++){
		verts[i].setScreenX(vs[i].x);
		verts[i].setScreenY(vs[i].y);
		verts[i].setScreenZ(rw::im2d::GetNearZ());
		verts[i].setCameraZ(Scene.camera->nearPlane);
		verts[i].setRecipCameraZ(recipZ);
		verts[i].setColor(vs[i].r, vs[i].g, vs[i].b, vs[i].a);
		if(dosoftras)
			verts[i].setColor(255, 255, 255, 255);
		verts[i].setU(vs[i].u + 0.5f/640.0f, recipZ);
		verts[i].setV(vs[i].v + 0.5f/448.0f, recipZ);
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
	if(dosoftras)
		rw::SetRenderStatePtr(rw::TEXTURERASTER, testras);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);
	rw::SetRenderState(rw::VERTEXALPHA, 1);
	rw::im2d::RenderIndexedPrimitive(rw::PRIMTYPETRISTRIP,
		&verts, 4, &indices, 4);
}

void
im2dtest2(void)
{
	using namespace rw::RWDEVICE;
	int i;
	rw::Camera *cam = Scene.camera;
	float n = cam->nearPlane;
	float f = cam->farPlane;
	float mid = (n+f)/4.0f;
	struct
	{
		float x, y, z;
		rw::uint8 r, g, b, a;
		float u, v;
	} vs[4] = {
		{ 0.5f,  0.5f,   n,  255, 255, 255, 255,  0.0f, 0.0f },
		{ 0.5f,  0.5f, mid,  255, 255, 255, 255,  1.0f, 0.0f },
		{ 0.5f, -0.5f,   n,  255, 255, 255, 255,  0.0f, 1.0f },
		{ 0.5f, -0.5f, mid,  255, 255, 255, 255,  1.0f, 1.0f },
	};
	Im2DVertex verts[4];
	static short indices[] = {
		0, 1, 2, 3
	};

	for(i = 0; i < 4; i++){
		float recipZ = 1.0f/vs[i].z;
		verts[i].setScreenX((vs[i].x*recipZ + 0.5f) * 640.0f);
		verts[i].setScreenY((vs[i].y*recipZ + 0.5f) * 448.0f);
		verts[i].setScreenZ(recipZ * cam->zScale + cam->zShift);
//		verts[i].setCameraZ(vs[i].z);
		verts[i].setRecipCameraZ(recipZ);
		verts[i].setColor(vs[i].r, vs[i].g, vs[i].b, vs[i].a);
		if(dosoftras)
			verts[i].setColor(255, 255, 255, 255);
		verts[i].setU(vs[i].u + 0.5f/640.0f, recipZ);
		verts[i].setV(vs[i].v + 0.5f/448.0f, recipZ);
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);
	rw::SetRenderState(rw::VERTEXALPHA, 1);
	rw::im2d::RenderIndexedPrimitive(rw::PRIMTYPETRISTRIP,
		&verts, 4, &indices, 4);
}

void
im3dtest(void)
{
	using namespace rw::RWDEVICE;
	int i;
	static struct
	{
		float x, y, z;
		rw::uint8 r, g, b, a;
		float u, v;
	} vs[8] = {
		{ -1.0f, -1.0f, -1.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ -1.0f,  1.0f, -1.0f,   0, 255, 0, 128,    0.0f, 1.0f },
		{  1.0f, -1.0f, -1.0f,   0, 0, 255, 128,    1.0f, 0.0f },
		{  1.0f,  1.0f, -1.0f,   255, 0, 255, 128,  1.0f, 1.0f },

		{ -1.0f, -1.0f,  1.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ -1.0f,  1.0f,  1.0f,   0, 255, 0, 128,    0.0f, 1.0f },
		{  1.0f, -1.0f,  1.0f,   0, 0, 255, 128,    1.0f, 0.0f },
		{  1.0f,  1.0f,  1.0f,   255, 0, 255, 128,  1.0f, 1.0f },
	};
	Im3DVertex verts[8];
	static short indices[2*6] = {
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7
	};

	for(i = 0; i < 8; i++){
		verts[i].setX(vs[i].x);
		verts[i].setY(vs[i].y);
		verts[i].setZ(vs[i].z);
		verts[i].setColor(vs[i].r, vs[i].g, vs[i].b, vs[i].a);
		verts[i].setU(vs[i].u);
		verts[i].setV(vs[i].v);
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
//	rw::SetRenderStatePtr(rw::TEXTURERASTER, testfont->raster);
//	rw::SetRenderStatePtr(rw::TEXTURERASTER, frontbuffer->raster);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);

/*
	genIm3DTransform(verts, 8, nil);
	genIm3DRenderIndexed(rw::PRIMTYPETRILIST, indices, 12);
	genIm3DEnd();
*/
	rw::im3d::Transform(verts, 8, nil, rw::im3d::EVERYTHING);
	rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, indices, 12);
	rw::im3d::End();
}

void
getFrontBuffer(void)
{
	rw::Raster *fb = Scene.camera->frameBuffer;

	if(frontbuffer == nil || fb->width > frontbuffer->raster->width || fb->height > frontbuffer->raster->height){
		int w, h;
		for(w = 1; w < fb->width; w <<= 1);
		for(h = 1; h < fb->height; h <<= 1);
		rw::Raster *ras = rw::Raster::create(w, h, fb->depth,  rw::Raster::CAMERATEXTURE);
		if(frontbuffer){
			frontbuffer->raster->destroy();
			frontbuffer->raster = ras;
		}else
			frontbuffer = rw::Texture::create(ras);
		printf("created FB with %d %d %d\n", ras->width, ras->height, ras->depth);
	}

	rw::Raster::pushContext(frontbuffer->raster);
	fb->renderFast(0, 0);
	rw::Raster::popContext();
}

void
Draw(float timeDelta)
{
	getFrontBuffer();

	rw::SetRenderState(rw::FOGCOLOR, 0xFF0000FF);
	rw::SetRenderState(rw::FOGENABLE, 1);
	camera->m_rwcam->fogPlane = camera->m_rwcam->nearPlane;

	static rw::RGBA clearcol = { 161, 161, 161, 0xFF };
	camera->m_rwcam->clear(&clearcol, rw::Camera::CLEARIMAGE|rw::Camera::CLEARZ);
	camera->update();
	camera->m_rwcam->beginUpdate();

extern void beginSoftras(void);
	beginSoftras();

//	gen::tlTest(Scene.clump);
void drawtest(void);
//	drawtest();

extern void endSoftras(void);
	if(dosoftras){
		endSoftras();
	}
		//im2dtest();
		im2dtest2();

//	Scene.clump->render();
//	im3dtest();
//	printScreen("Hello, World!", 10, 10);

//	testfont->print("foo ABC", 200, 200, true);

//	rendersplines();

	camera->m_rwcam->endUpdate();

	camera->m_rwcam->showRaster(0);
}


void
KeyUp(int key)
{
}

void
KeyDown(int key)
{
	switch(key){
	case 'W':
		camera->orbit(0.0f, 0.1f);
		break;
	case 'S':
		camera->orbit(0.0f, -0.1f);
		break;
	case 'A':
		camera->orbit(-0.1f, 0.0f);
		break;
	case 'D':
		camera->orbit(0.1f, 0.0f);
		break;
	case sk::KEY_UP:
		camera->turn(0.0f, 0.1f);
		break;
	case sk::KEY_DOWN:
		camera->turn(0.0f, -0.1f);
		break;
	case sk::KEY_LEFT:
		camera->turn(0.1f, 0.0f);
		break;
	case sk::KEY_RIGHT:
		camera->turn(-0.1f, 0.0f);
		break;
	case 'R':
		camera->zoom(0.1f);
		break;
	case 'F':
		camera->zoom(-0.1f);
		break;
	case 'V':
		dosoftras = !dosoftras;
		break;
	case sk::KEY_ESC:
		sk::globals.quit = 1;
		break;
	}
}

void
MouseMove(int x, int y)
{
}

void
MouseButton(int buttons)
{
}

sk::EventStatus
AppEventHandler(sk::Event e, void *param)
{
	using namespace sk;
	Rect *r;
	MouseState *ms;

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
	case MOUSEBTN:
		ms = (MouseState*)param;
		MouseButton(ms->buttons);
		return EVENTPROCESSED;
	case MOUSEMOVE:
		ms = (MouseState*)param;
		MouseMove(ms->posx, ms->posy);
		return EVENTPROCESSED;
	case RESIZE:
		r = (Rect*)param;
		// TODO: register when we're minimized
		if(r->w == 0) r->w = 1;
		if(r->h == 0) r->h = 1;

		sk::globals.width = r->w;
		sk::globals.height = r->h;
		if(camera)
			camera->m_aspectRatio = (float)r->w/r->h;
		if(Scene.camera)
			sk::CameraSize(Scene.camera, r);
		break;
	case IDLE:
		Draw(*(float*)param);
		return EVENTPROCESSED;
	}
	return sk::EVENTNOTPROCESSED;
}

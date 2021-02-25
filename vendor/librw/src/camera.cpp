#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

#define PLUGIN_ID ID_CAMERA

namespace rw {

int32 Camera::numAllocated;

PluginList Camera::s_plglist(sizeof(Camera));

void
defaultBeginUpdateCB(Camera *cam)
{
	engine->currentCamera = cam;
	Frame::syncDirty();
	engine->device.beginUpdate(cam);
}

void
defaultEndUpdateCB(Camera *cam)
{
	engine->device.endUpdate(cam);
}

static void
buildPlanes(Camera *cam)
{
	V3d *c = cam->frustumCorners;
	FrustumPlane *p = cam->frustumPlanes;
	V3d v51 = sub(c[1], c[5]);
	V3d v73 = sub(c[3], c[7]);

	/* Far plane */
	p[0].plane.normal = cam->getFrame()->getLTM()->at;
	p[0].plane.distance = dot(p[0].plane.normal, c[4]);
	p[0].closestX = p[0].plane.normal.x < 0.0f ? 0 : 1;
	p[0].closestY = p[0].plane.normal.y < 0.0f ? 0 : 1;
	p[0].closestZ = p[0].plane.normal.z < 0.0f ? 0 : 1;

	/* Near plane */
	p[1].plane.normal = neg(p[0].plane.normal);
	p[1].plane.distance = dot(p[1].plane.normal, c[0]);
	p[1].closestX = p[1].plane.normal.x < 0.0f ? 0 : 1;
	p[1].closestY = p[1].plane.normal.y < 0.0f ? 0 : 1;
	p[1].closestZ = p[1].plane.normal.z < 0.0f ? 0 : 1;

	/* Right plane */
	p[2].plane.normal = normalize(cross(v51,
	                                    sub(c[6], c[5])));
	p[2].plane.distance = dot(p[2].plane.normal, c[1]);
	p[2].closestX = p[2].plane.normal.x < 0.0f ? 0 : 1;
	p[2].closestY = p[2].plane.normal.y < 0.0f ? 0 : 1;
	p[2].closestZ = p[2].plane.normal.z < 0.0f ? 0 : 1;

	/* Top plane */
	p[3].plane.normal = normalize(cross(sub(c[4], c[5]),
	                                    v51));
	p[3].plane.distance = dot(p[3].plane.normal, c[1]);
	p[3].closestX = p[3].plane.normal.x < 0.0f ? 0 : 1;
	p[3].closestY = p[3].plane.normal.y < 0.0f ? 0 : 1;
	p[3].closestZ = p[3].plane.normal.z < 0.0f ? 0 : 1;

	/* Left plane */
	p[4].plane.normal = normalize(cross(v73,
	                                    sub(c[4], c[7])));
	p[4].plane.distance = dot(p[4].plane.normal, c[3]);
	p[4].closestX = p[4].plane.normal.x < 0.0f ? 0 : 1;
	p[4].closestY = p[4].plane.normal.y < 0.0f ? 0 : 1;
	p[4].closestZ = p[4].plane.normal.z < 0.0f ? 0 : 1;

	/* Bottom plane */
	p[5].plane.normal = normalize(cross(sub(c[6], c[7]),
	                                    v73));
	p[5].plane.distance = dot(p[5].plane.normal, c[3]);
	p[5].closestX = p[5].plane.normal.x < 0.0f ? 0 : 1;
	p[5].closestY = p[5].plane.normal.y < 0.0f ? 0 : 1;
	p[5].closestZ = p[5].plane.normal.z < 0.0f ? 0 : 1;
}

static void
buildClipPersp(Camera *cam)
{
	Matrix *ltm = cam->getFrame()->getLTM();

	/* First we calculate the 4 points on the view window. */
	V3d up = scale(ltm->up, cam->viewWindow.y);
	V3d left = scale(ltm->right, cam->viewWindow.x);
	V3d *c = cam->frustumCorners;
	c[0] = add(add(ltm->at, up), left);	// top left
	c[1] = sub(add(ltm->at, up), left);	// top right
	c[2] = sub(sub(ltm->at, up), left);	// bottom right
	c[3] = add(sub(ltm->at, up), left);	// bottom left

	/* Now Calculate near and far corners. */
	V3d off = sub(scale(ltm->up, cam->viewOffset.y),
	              scale(ltm->right, cam->viewOffset.x));
	for(int32 i = 0; i < 4; i++){
		V3d corner = sub(cam->frustumCorners[i], off);
		V3d pos = add(ltm->pos, off);
		c[i] = add(scale(corner, cam->nearPlane), pos);
		c[i+4] = add(scale(corner, cam->farPlane), pos);
	}

	buildPlanes(cam);
}

static void
buildClipParallel(Camera *cam)
{
	Matrix *ltm = cam->getFrame()->getLTM();
	float32 nearoffx = -(1.0f - cam->nearPlane)*cam->viewOffset.x;
	float32 nearoffy = (1.0f - cam->nearPlane)*cam->viewOffset.y;
	float32 faroffx = -(1.0f - cam->farPlane)*cam->viewOffset.x;
	float32 faroffy = (1.0f - cam->farPlane)*cam->viewOffset.y;

	V3d *c = cam->frustumCorners;
	c[0].x = nearoffx + cam->viewWindow.x;
	c[0].y = nearoffy + cam->viewWindow.y;
	c[0].z = cam->nearPlane;

	c[1].x = nearoffx - cam->viewWindow.x;
	c[1].y = nearoffy + cam->viewWindow.y;
	c[1].z = cam->nearPlane;

	c[2].x = nearoffx - cam->viewWindow.x;
	c[2].y = nearoffy - cam->viewWindow.y;
	c[2].z = cam->nearPlane;

	c[3].x = nearoffx + cam->viewWindow.x;
	c[3].y = nearoffy - cam->viewWindow.y;
	c[3].z = cam->nearPlane;

	c[4].x = faroffx + cam->viewWindow.x;
	c[4].y = faroffy + cam->viewWindow.y;
	c[4].z = cam->farPlane;

	c[5].x = faroffx - cam->viewWindow.x;
	c[5].y = faroffy + cam->viewWindow.y;
	c[5].z = cam->farPlane;

	c[6].x = faroffx - cam->viewWindow.x;
	c[6].y = faroffy - cam->viewWindow.y;
	c[6].z = cam->farPlane;

	c[7].x = faroffx + cam->viewWindow.x;
	c[7].y = faroffy - cam->viewWindow.y;
	c[7].z = cam->farPlane;

	V3d::transformPoints(c, c, 8, ltm);

	buildPlanes(cam);
}

static void
cameraSync(ObjectWithFrame *obj)
{
	/*
	 * RW projection matrix looks like this:
	 *       (cf. Camera View Matrix white paper)
	 * w = viewWindow width
	 * h = viewWindow height
	 * o = view offset
	 *
	 * perspective:
	 * 1/2w       0    ox/2w + 1/2   -ox/2w
	 *    0   -1/2h   -oy/2h + 1/2    oy/2h
	 *    0       0              1        0
	 *    0       0              1        0
	 *
	 * parallel:
	 * 1/2w       0    ox/2w   -ox/2w + 1/2
	 *    0   -1/2h   -oy/2h    oy/2h + 1/2
	 *    0       0        1              0
	 *    0       0        0              1
	 *
	 * The view matrix transforms from world to clip space, it is however
	 * not used for OpenGL or D3D since transformation to camera space
	 * and to clip space are handled by separate matrices there.
	 * On these platforms the two matrices are built in the platform's
	 * beginUpdate function.
	 * On the PS2 the z- and w-rows are the same and the 
	 * 1/2 translation/shear is removed again on the VU1 by
	 * subtracting the w-row/2 from the x- and y-rows.
	 *
	 * perspective:
	 * 1/2w       0    ox/2w   -ox/2w
	 *    0   -1/2h   -oy/2h    oy/2h
	 *    0       0        1        0
	 *    0       0        1        0
	 *
	 * parallel:
	 * 1/2w       0    ox/2w   -ox/2w
	 *    0   -1/2h   -oy/2h    oy/2h
	 *    0       0        1        0
	 *    0       0        0        1
	 *
	 * RW builds this matrix directly without using explicit
	 * inversion and matrix multiplication.
	 */

	Camera *cam = (Camera*)obj;
	Matrix inv, proj;
	Matrix::invertOrthonormal(&inv, cam->getFrame()->getLTM());

	inv.right.x = -inv.right.x;
	inv.up.x = -inv.up.x;
	inv.at.x = -inv.at.x;
	inv.pos.x = -inv.pos.x;

	float32 xscl = 1.0f/(2.0f*cam->viewWindow.x);
	float32 yscl = 1.0f/(2.0f*cam->viewWindow.y);

	proj.flags = 0;
	proj.right.x = xscl;
	proj.right.y = 0.0f;
	proj.right.z = 0.0f;

	proj.up.x = 0.0f;
	proj.up.y = -yscl;
	proj.up.z = 0.0f;

	if(cam->projection == Camera::PERSPECTIVE){
		proj.pos.x = -cam->viewOffset.x*xscl;
		proj.pos.y = cam->viewOffset.y*yscl;
		proj.pos.z = 0.0f;

		proj.at.x = -proj.pos.x + 0.5f;
		proj.at.y = -proj.pos.y + 0.5f;
		proj.at.z = 1.0f;
		proj.optimize();
		Matrix::mult(&cam->viewMatrix, &inv, &proj);
		buildClipPersp(cam);
	}else{
		proj.at.x = cam->viewOffset.x*xscl;
		proj.at.y = -cam->viewOffset.y*yscl;
		proj.at.z = 1.0f;

		proj.pos.x = -proj.at.x + 0.5f;
		proj.pos.y = -proj.at.y + 0.5f;
		proj.pos.z = 0.0f;
		proj.optimize();
		Matrix::mult(&cam->viewMatrix, &inv, &proj);
		buildClipParallel(cam);
	}
	cam->frustumBoundBox.calculate(cam->frustumCorners, 8);
}

void
worldBeginUpdateCB(Camera *cam)
{
	engine->currentWorld = cam->world;
	cam->originalBeginUpdate(cam);
}

void
worldEndUpdateCB(Camera *cam)
{
	cam->originalEndUpdate(cam);
}

static void
worldCameraSync(ObjectWithFrame *obj)
{
	Camera *camera = (Camera*)obj;
	camera->originalSync(obj);
}

Camera*
Camera::create(void)
{
	Camera *cam = (Camera*)rwMalloc(s_plglist.size, MEMDUR_EVENT | ID_CAMERA);
	if(cam == nil){
		RWERROR((ERR_ALLOC, s_plglist.size));
		return nil;
	}
	numAllocated++;
	cam->object.object.init(Camera::ID, 0);
	cam->object.syncCB = cameraSync;
	cam->beginUpdateCB = defaultBeginUpdateCB;
	cam->endUpdateCB = defaultEndUpdateCB;
	cam->viewWindow.set(1.0f, 1.0f);
	cam->viewOffset.set(0.0f, 0.0f);
	cam->nearPlane = 0.05f;
	cam->farPlane = 10.0f;
	cam->fogPlane = 5.0f;
	cam->projection = Camera::PERSPECTIVE;

	cam->frameBuffer = nil;
	cam->zBuffer = nil;

	// clump extension
	cam->clump = nil;
	cam->inClump.init();

	// world extension
	cam->world = nil;
	cam->originalSync = cam->object.syncCB;
	cam->originalBeginUpdate = cam->beginUpdateCB;
	cam->originalEndUpdate = cam->endUpdateCB;
	cam->object.syncCB = worldCameraSync;
	cam->beginUpdateCB = worldBeginUpdateCB;
	cam->endUpdateCB = worldEndUpdateCB;

	s_plglist.construct(cam);
	return cam;
}

Camera*
Camera::clone(void)
{
	Camera *cam = Camera::create();
	if(cam == nil)
		return nil;
	cam->object.object.copy(&this->object.object);
	cam->setFrame(this->getFrame());
	cam->viewWindow = this->viewWindow;
	cam->viewOffset = this->viewOffset;
	cam->nearPlane = this->nearPlane;
	cam->farPlane = this->farPlane;
	cam->fogPlane = this->fogPlane;
	cam->projection = this->projection;

	cam->frameBuffer = this->frameBuffer;
	cam->zBuffer = this->zBuffer;

	if(this->world)
		this->world->addCamera(cam);

	s_plglist.copy(cam, this);
	return cam;
}

void
Camera::destroy(void)
{
	s_plglist.destruct(this);
	assert(this->clump == nil);
	assert(this->world == nil);
	this->setFrame(nil);
	rwFree(this);
	numAllocated--;
}

void
Camera::clear(RGBA *col, uint32 mode)
{
	engine->device.clearCamera(this, col, mode);
}

void
Camera::showRaster(uint32 flags)
{
	this->frameBuffer->show(flags);
}

void
calczShiftScale(Camera *cam)
{
	float32 n = cam->nearPlane;
	float32 f = cam->farPlane;
	float32 N = engine->device.zNear;
	float32 F = engine->device.zFar;
	// RW does this
	N += (F - N)/10000.0f;
	F -= (F - N)/10000.0f;
	if(cam->projection == Camera::PERSPECTIVE){
		cam->zScale = (N - F)*n*f/(f - n);
		cam->zShift = (F*f - N*n)/(f - n);
	}else{
		cam->zScale = (F - N)/(f -n);
		cam->zShift = (N*f - F*n)/(f - n);
	}
}

void
Camera::setNearPlane(float32 near)
{
	this->nearPlane = near;
	calczShiftScale(this);
	if(this->getFrame())
		this->getFrame()->updateObjects();
}

void
Camera::setFarPlane(float32 far)
{
	this->farPlane = far;
	calczShiftScale(this);
	if(this->getFrame())
		this->getFrame()->updateObjects();
}

void
Camera::setViewWindow(const V2d *window)
{
	this->viewWindow = *window;
	if(this->getFrame())
		this->getFrame()->updateObjects();
}

void
Camera::setViewOffset(const V2d *offset)
{
	this->viewOffset = *offset;
	if(this->getFrame())
		this->getFrame()->updateObjects();
}

void
Camera::setProjection(int32 proj)
{
	this->projection = proj;
	if(this->getFrame())
		this->getFrame()->updateObjects();
}

int32
Camera::frustumTestSphere(const Sphere *s) const
{
	int32 res = SPHEREINSIDE;
	const FrustumPlane *p = this->frustumPlanes;
	for(int32 i = 0; i < 6; i++){
		float32 dist = dot(p->plane.normal, s->center) - p->plane.distance;
		if(s->radius < dist)
			return SPHEREOUTSIDE;
		if(s->radius > -dist)
			res = SPHEREBOUNDARY;
		p++;
	}
	return res;
}

struct CameraChunkData
{
	V2d viewWindow;
	V2d viewOffset;
	float32 nearPlane, farPlane;
	float32 fogPlane;
	int32 projection;
};

Camera*
Camera::streamRead(Stream *stream)
{
	CameraChunkData buf;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	stream->read32(&buf, sizeof(CameraChunkData));
	Camera *cam = Camera::create();
	cam->viewWindow = buf.viewWindow;
	cam->viewOffset = buf.viewOffset;
	cam->nearPlane = buf.nearPlane;
	cam->farPlane = buf.farPlane;
	cam->fogPlane = buf.fogPlane;
	cam->projection = buf.projection;
	if(s_plglist.streamRead(stream, cam))
		return cam;
	cam->destroy();
	return nil;
}

bool
Camera::streamWrite(Stream *stream)
{
	CameraChunkData buf;
	writeChunkHeader(stream, ID_CAMERA, this->streamGetSize());
	writeChunkHeader(stream, ID_STRUCT, sizeof(CameraChunkData));
	buf.viewWindow = this->viewWindow;
	buf.viewOffset = this->viewOffset;
	buf.nearPlane = this->nearPlane;
	buf.farPlane  = this->farPlane;
	buf.fogPlane = this->fogPlane;
	buf.projection = this->projection;
	stream->write32(&buf, sizeof(CameraChunkData));
	s_plglist.streamWrite(stream, this);
	return true;
}

uint32
Camera::streamGetSize(void)
{
	return 12 + sizeof(CameraChunkData) + 12 +
	       s_plglist.streamGetSize(this);
}

// Assumes horizontal FOV for 4:3, but we convert to vertical FOV
void
Camera::setFOV(float32 hfov, float32 ratio)
{
	V2d v;
	float w, h;

	w = (float)this->frameBuffer->width;
	h = (float)this->frameBuffer->height;
	if(w < 1 || h < 1){
		w = 1;
		h = 1;
	}
	hfov = hfov*3.14159f/360.0f;	// deg to rad and halved

	float ar1 = 4.0f/3.0f;
	float ar2 = w/h;
	float vfov = atanf(tanf(hfov/2) / ar1) *2;
	hfov = atanf(tanf(vfov/2) * ar2) *2;

	float32 a = tanf(hfov);
	v.set(a, a/ratio);
	this->setViewWindow(&v);
	v.set(0.0f, 0.0f);
	this->setViewOffset(&v);
}

}

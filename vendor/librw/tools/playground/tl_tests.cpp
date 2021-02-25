#include <rw.h>
#include <skeleton.h>

extern bool dosoftras;

using namespace rw;
using namespace RWDEVICE;

void rastest_renderTriangles(RWDEVICE::Im2DVertex *scrverts, int32 verts, uint16 *indices, int32 numTris);

//
// This is a test to implement T&L in software and render with Im2D
//

namespace gen {

#define MAX_LIGHTS 8

struct Directional {
	V3d at;
	RGBAf color;
};
static Directional directionals[MAX_LIGHTS];
static int32 numDirectionals;
static RGBAf ambLight;

static void
enumLights(Matrix *lightmat)
{
	int32 n;
	World *world;

	world = (World*)engine->currentWorld;
	ambLight.red = 0.0;
	ambLight.green = 0.0;
	ambLight.blue = 0.0;
	ambLight.alpha = 0.0;
	numDirectionals = 0;
	// only unpositioned lights right now
	FORLIST(lnk, world->globalLights){
		Light *l = Light::fromWorld(lnk);
		if(l->getType() == Light::DIRECTIONAL){
			if(numDirectionals >= MAX_LIGHTS)
				continue;
			n = numDirectionals++;
			V3d::transformVectors(&directionals[n].at, &l->getFrame()->getLTM()->at, 1, lightmat);
			directionals[n].color = l->color;
			directionals[n].color.alpha = 0.0f;
		}else if(l->getType() == Light::AMBIENT){
			ambLight.red   += l->color.red;
			ambLight.green += l->color.green;
			ambLight.blue  += l->color.blue;
		}
	}
}

struct ObjSpace3DVertex
{
	V3d objVertex;
	V3d objNormal;
	RGBA color;
	TexCoords texCoords;
};

enum {
	CLIPXLO = 0x01,
	CLIPXHI = 0x02,
	CLIPX   = 0x03,
	CLIPYLO = 0x04,
	CLIPYHI = 0x08,
	CLIPY   = 0x0C,
	CLIPZLO = 0x10,
	CLIPZHI = 0x20,
	CLIPZ   = 0x30,
};

struct CamSpace3DVertex
{
	V3d camVertex;
	uint8 clipFlags;
	RGBAf color;
	TexCoords texCoords;
};

struct InstanceData
{
	uint16 *indices;
	int32 numIndices;
	ObjSpace3DVertex *vertices;
	int32 numVertices;
	// int vertStride;	// not really needed right now
	Material *material;
	Mesh *mesh;
};

struct InstanceDataHeader : public rw::InstanceDataHeader
{
	uint32 serialNumber;
	ObjSpace3DVertex *vertices;
	uint16 *indices;
	InstanceData *inst;
};

static void
instanceAtomic(Atomic *atomic)
{
	static V3d zeroNorm = { 0.0f, 0.0f, 0.0f };
	static RGBA black = { 0, 0, 0, 255 };
	static TexCoords zeroTex = { 0.0f, 0.0f };
	int i;
	uint j;
	int x, x1, x2, x3;
	Geometry *geo;
	MeshHeader *header;
	Mesh *mesh;
	InstanceDataHeader *insthead;
	InstanceData *inst;
	uint32 firstVert;
	uint16 *srcindices, *dstindices;

	geo = atomic->geometry;
	if(geo->instData)
		return;
	header = geo->meshHeader;
	int numTris;
	if(header->flags & MeshHeader::TRISTRIP)
		numTris = header->totalIndices - 2*header->numMeshes;
	else
		numTris = header->totalIndices / 3;
	int size;
	size = sizeof(InstanceDataHeader) + header->numMeshes*sizeof(InstanceData) +
		geo->numVertices*sizeof(ObjSpace3DVertex) + numTris*6*sizeof(uint16);
	insthead = (InstanceDataHeader*)rwNew(size, ID_GEOMETRY);
	geo->instData = insthead;
	insthead->platform = 0;
	insthead->serialNumber = header->serialNum;
	inst = (InstanceData*)(insthead+1);
	insthead->inst = inst;
	insthead->vertices = (ObjSpace3DVertex*)(inst+header->numMeshes);
	dstindices = (uint16*)(insthead->vertices+geo->numVertices);
	insthead->indices = dstindices;

	// TODO: morphing
	MorphTarget *mt = geo->morphTargets;
	for(i = 0; i < geo->numVertices; i++){
		insthead->vertices[i].objVertex = mt->vertices[i];
		if(geo->flags & Geometry::NORMALS)
			insthead->vertices[i].objNormal = mt->normals[i];
		else
			insthead->vertices[i].objNormal = zeroNorm;
		if(geo->flags & Geometry::PRELIT)
			insthead->vertices[i].color = geo->colors[i];
		else
			insthead->vertices[i].color = black;
		if(geo->numTexCoordSets > 0)
			insthead->vertices[i].texCoords = geo->texCoords[0][i];
		else
			insthead->vertices[i].texCoords = zeroTex;
	}

	mesh = header->getMeshes();
	for(i = 0; i < header->numMeshes; i++){
		findMinVertAndNumVertices(mesh->indices, mesh->numIndices,
		                          &firstVert, &inst->numVertices);
		inst->indices = dstindices;
		inst->vertices = &insthead->vertices[firstVert];
		inst->mesh = mesh;
		inst->material = mesh->material;
		srcindices = mesh->indices;
		if(header->flags & MeshHeader::TRISTRIP){
			inst->numIndices = 0;
			x = 0;
			for(j = 0; j < mesh->numIndices-2; j++){
				x1 = srcindices[j+x];
				x ^= 1;
				x2 = srcindices[j+x];
				x3 = srcindices[j+2];
				if(x1 != x2 && x2 != x3 && x1 != x3){
					dstindices[0] = x1;
					dstindices[1] = x2;
					dstindices[2] = x3;
					dstindices += 3;
					inst->numIndices += 3;
				}
			}
		}else{
			inst->numIndices = mesh->numIndices;
			for(j = 0; j < mesh->numIndices; j += 3){
				dstindices[0] = srcindices[j+0] - firstVert;
				dstindices[1] = srcindices[j+1] - firstVert;
				dstindices[2] = srcindices[j+2] - firstVert;
				dstindices += 3;
			}
		}

		inst++;
		mesh++;
	}
}

struct MeshState
{
	int32 flags;
	Matrix obj2cam;
	Matrix obj2world;
	int32 numVertices;
	int32 numPrimitives;
	SurfaceProperties surfProps;
	RGBA matCol;
};

static void
cam2screen(Im2DVertex *scrvert, CamSpace3DVertex *camvert)
{
	RGBA col;
	float32 recipZ;
	Camera *cam = (Camera*)engine->currentCamera;
	int32 width = cam->frameBuffer->width;
	int32 height = cam->frameBuffer->height;
	recipZ = 1.0f/camvert->camVertex.z;

//	scrvert->setScreenX(camvert->camVertex.x * recipZ * width);
//	scrvert->setScreenY(camvert->camVertex.y * recipZ * height);
	scrvert->setScreenX(camvert->camVertex.x * recipZ * width/2 + width/4);
	scrvert->setScreenY(camvert->camVertex.y * recipZ * height/2 + height/4);
	scrvert->setScreenZ(recipZ * cam->zScale + cam->zShift);
	scrvert->setCameraZ(camvert->camVertex.z);
	scrvert->setRecipCameraZ(recipZ);
	scrvert->setU(camvert->texCoords.u, recipZ);
	scrvert->setV(camvert->texCoords.v, recipZ);
	convColor(&col, &camvert->color);
	scrvert->setColor(col.red, col.green, col.blue, col.alpha);
}

static void
transform(MeshState *mstate, ObjSpace3DVertex *objverts, CamSpace3DVertex *camverts, Im2DVertex *scrverts)
{
	int32 i;
	float32 z;
	Camera *cam = (Camera*)engine->currentCamera;

	for(i = 0; i < mstate->numVertices; i++){
		V3d::transformPoints(&camverts[i].camVertex, &objverts[i].objVertex, 1, &mstate->obj2cam);
		convColor(&camverts[i].color, &objverts[i].color);
		camverts[i].texCoords = objverts[i].texCoords;

		camverts[i].clipFlags = 0;
		z = camverts[i].camVertex.z;
		// 0 < x < z
		if(camverts[i].camVertex.x >= z) camverts[i].clipFlags |= CLIPXHI;
		if(camverts[i].camVertex.x <= 0) camverts[i].clipFlags |= CLIPXLO;
		// 0 < y < z
		if(camverts[i].camVertex.y >= z) camverts[i].clipFlags |= CLIPYHI;
		if(camverts[i].camVertex.y <= 0) camverts[i].clipFlags |= CLIPYLO;
		// near < z < far
		if(z >= cam->farPlane) camverts[i].clipFlags |= CLIPZHI;
		if(z <= cam->nearPlane) camverts[i].clipFlags |= CLIPZLO;

		cam2screen(&scrverts[i], &camverts[i]);
	}
}

static void
light(MeshState *mstate, ObjSpace3DVertex *objverts, CamSpace3DVertex *camverts)
{
	int32 i;
	RGBAf colf;
	RGBAf amb = ambLight;
	amb = scale(ambLight, mstate->surfProps.ambient);
	for(i = 0; i < mstate->numVertices; i++){
		camverts[i].color = add(camverts[i].color, amb);
		if((mstate->flags & Geometry::NORMALS) == 0)
			continue;
		for(int32 k = 0; k < numDirectionals; k++){
			float32 f = dot(objverts[i].objNormal, neg(directionals[k].at));
			if(f <= 0.0f) continue;
			f *= mstate->surfProps.diffuse;
			colf = scale(directionals[k].color, f);
			camverts[i].color = add(camverts[i].color, colf);
		}
	}
}

static void
postlight(MeshState *mstate, CamSpace3DVertex *camverts, Im2DVertex *scrverts)
{
	int32 i;
	RGBA col;
	RGBAf colf;
	for(i = 0; i < mstate->numVertices; i++){
		convColor(&colf, &mstate->matCol);
		camverts[i].color = modulate(camverts[i].color, colf);
		clamp(&camverts[i].color);
		convColor(&col, &camverts[i].color);
		scrverts[i].setColor(col.red, col.green, col.blue, col.alpha);
	}
}

static int32
cullTriangles(MeshState *mstate, CamSpace3DVertex *camverts, uint16 *indices, uint16 *clipindices)
{
	int32 i;
	int32 x1, x2, x3;
	int32 newNumPrims;
	int32 numClip;

	newNumPrims = 0;
	numClip = 0;
	for(i = 0; i < mstate->numPrimitives; i++, indices += 3){
		x1 = indices[0];
		x2 = indices[1];
		x3 = indices[2];
		// Only a simple frustum call
		if(camverts[x1].clipFlags &
		   camverts[x2].clipFlags &
		   camverts[x3].clipFlags)
			continue;
		if(camverts[x1].clipFlags |
		   camverts[x2].clipFlags |
		   camverts[x3].clipFlags)
			numClip++;
		// The Triangle is in, probably
		clipindices[0] = x1;
		clipindices[1] = x2;
		clipindices[2] = x3;
		clipindices += 3;
		newNumPrims++;
	}
	mstate->numPrimitives = newNumPrims;
	return numClip;
}

static void
interpVertex(CamSpace3DVertex *out, CamSpace3DVertex *v1, CamSpace3DVertex *v2, float32 t)
{
	float32 z;
	float32 invt;
	Camera *cam = (Camera*)engine->currentCamera;

	invt = 1.0f - t;
	out->camVertex = add(scale(v1->camVertex, invt), scale(v2->camVertex, t));
	out->color = add(scale(v1->color, invt), scale(v2->color, t));
	out->texCoords.u = v1->texCoords.u*invt + v2->texCoords.u*t;
	out->texCoords.v = v1->texCoords.v*invt + v2->texCoords.v*t;

	out->clipFlags = 0;
	z = out->camVertex.z;
	// 0 < x < z
	if(out->camVertex.x >= z) out->clipFlags |= CLIPXHI;
	if(out->camVertex.x <= 0) out->clipFlags |= CLIPXLO;
	// 0 < y < z
	if(out->camVertex.y >= z) out->clipFlags |= CLIPYHI;
	if(out->camVertex.y <= 0) out->clipFlags |= CLIPYLO;
	// near < z < far
	if(z >= cam->farPlane) out->clipFlags |= CLIPZHI;
	if(z <= cam->nearPlane) out->clipFlags |= CLIPZLO;
}

static void
clipTriangles(MeshState *mstate, CamSpace3DVertex *camverts, Im2DVertex *scrverts, uint16 *indices, uint16 *clipindices)
{
	int32 i, j;
	int32 x1, x2, x3;
	int32 newNumPrims;
	CamSpace3DVertex buf[18];
	CamSpace3DVertex *in, *out, *tmp;
	int32 nin, nout;
	float32 t;
	Camera *cam = (Camera*)engine->currentCamera;

	newNumPrims = 0;
	for(i = 0; i < mstate->numPrimitives; i++, indices += 3){
		x1 = indices[0];
		x2 = indices[1];
		x3 = indices[2];

		if((camverts[x1].clipFlags |
		    camverts[x2].clipFlags |
		    camverts[x3].clipFlags) == 0){
			// all inside
			clipindices[0] = x1;
			clipindices[1] = x2;
			clipindices[2] = x3;
			clipindices += 3;
			newNumPrims++;
			continue;
		}

		// set up triangle
		in = &buf[0];
		out = &buf[9];
		in[0] = camverts[x1];
		in[1] = camverts[x2];
		in[2] = camverts[x3];
		nin = 3;
		nout = 0;

#define V(a) in[a].camVertex.

		// clip z near
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPZLO){
				t = (cam->nearPlane - V(x1)z)/(V(x2)z - V(x1)z);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPZLO) == 0)
				out[nout++] = in[x2];
		}
		// clip z far
		nin = nout; nout = 0;
		tmp = in; in = out; out = tmp;
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPZHI){
				t = (cam->farPlane - V(x1)z)/(V(x2)z - V(x1)z);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPZHI) == 0)
				out[nout++] = in[x2];
		}
		// clip y 0
		nin = nout; nout = 0;
		tmp = in; in = out; out = tmp;
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPYLO){
				t = -V(x1)y/(V(x2)y - V(x1)y);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPYLO) == 0)
				out[nout++] = in[x2];
		}
		// clip y z
		nin = nout; nout = 0;
		tmp = in; in = out; out = tmp;
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPYHI){
				t = (V(x1)z - V(x1)y)/(V(x1)z - V(x1)y + V(x2)y - V(x2)z);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPYHI) == 0)
				out[nout++] = in[x2];
		}
		// clip x 0
		nin = nout; nout = 0;
		tmp = in; in = out; out = tmp;
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPXLO){
				t = -V(x1)x/(V(x2)x - V(x1)x);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPXLO) == 0)
				out[nout++] = in[x2];
		}
		// clip x z
		nin = nout; nout = 0;
		tmp = in; in = out; out = tmp;
		for(j = 0; j < nin; j++){
			x1 = j;
			x2 = (j+1) % nin;
			if((in[x1].clipFlags ^ in[x2].clipFlags) & CLIPXHI){
				t = (V(x1)z - V(x1)x)/(V(x1)z - V(x1)x + V(x2)x - V(x2)z);
				interpVertex(&out[nout++], &in[x1], &in[x2], t);
			}
			if((in[x2].clipFlags & CLIPXHI) == 0)
				out[nout++] = in[x2];
		}

		// Insert new triangles
		x1 = mstate->numVertices;
		for(j = 0; j < nout; j++){
			x2 = mstate->numVertices++;
			camverts[x2] = out[j];
			cam2screen(&scrverts[x2], &camverts[x2]);
		}
		x2 = x1+1;
		for(j = 0; j < nout-2; j++){
			clipindices[0] = x1;
			clipindices[1] = x2++;
			clipindices[2] = x2;
			clipindices += 3;
			newNumPrims++;
		}
	}
	mstate->numPrimitives = newNumPrims;
}

static int32
clipPoly(CamSpace3DVertex *in, int32 nin, CamSpace3DVertex *out, Plane *plane)
{
	int32 j;
	int32 nout;
	int32 x1, x2;
	float32 d1, d2, t;

	nout = 0;
	for(j = 0; j < nin; j++){
		x1 = j;
		x2 = (j+1) % nin;

		d1 = dot(plane->normal, in[x1].camVertex) + plane->distance;
		d2 = dot(plane->normal, in[x2].camVertex) + plane->distance;
		if(d1*d2 < 0.0f){
			t = d1/(d1 - d2);
			interpVertex(&out[nout++], &in[x1], &in[x2], t);
		}
		if(d2 >= 0.0f)
			out[nout++] = in[x2];
	}
	return nout;
}

static void
clipTriangles2(MeshState *mstate, CamSpace3DVertex *camverts, Im2DVertex *scrverts, uint16 *indices, uint16 *clipindices)
{
	int32 i, j;
	int32 x1, x2, x3;
	int32 newNumPrims;
	CamSpace3DVertex buf[18];
	CamSpace3DVertex *in, *out;
	int32 nout;
	Camera *cam = (Camera*)engine->currentCamera;

	Plane planes[6] = {
		{ {  0.0f,  0.0f,  1.0f }, -cam->nearPlane },	// z = near
		{ {  0.0f,  0.0f, -1.0f },  cam->farPlane },	// z = far

		{ { -1.0f,  0.0f,  1.0f }, 0.0f },	// x = w
//		{ {  1.0f,  0.0f,  1.0f }, 0.0f },	// x = -w
		{ {  1.0f,  0.0f,  0.0f }, 0.0f },	// x = 0

		{ {  0.0f, -1.0f,  1.0f }, 0.0f },	// y = w
//		{ {  0.0f,  1.0f,  1.0f }, 0.0f }	// y = -1
		{ {  0.0f,  1.0f,  0.0f }, 0.0f }	// y = 0
	};

	newNumPrims = 0;
	for(i = 0; i < mstate->numPrimitives; i++, indices += 3){
		x1 = indices[0];
		x2 = indices[1];
		x3 = indices[2];

		if((camverts[x1].clipFlags |
		    camverts[x2].clipFlags |
		    camverts[x3].clipFlags) == 0){
			// all inside
			clipindices[0] = x1;
			clipindices[1] = x2;
			clipindices[2] = x3;
			clipindices += 3;
			newNumPrims++;
			continue;
		}

		// set up triangle
		in = &buf[0];
		out = &buf[9];
		in[0] = camverts[x1];
		in[1] = camverts[x2];
		in[2] = camverts[x3];
		nout = 0;

		// clip here
		if(nout = clipPoly(in,  3,    out, &planes[0]), nout == 0) continue;
		if(nout = clipPoly(out, nout, in,  &planes[1]), nout == 0) continue;
		if(nout = clipPoly(in,  nout, out, &planes[2]), nout == 0) continue;
		if(nout = clipPoly(out, nout, in,  &planes[3]), nout == 0) continue;
		if(nout = clipPoly(in,  nout, out, &planes[4]), nout == 0) continue;
		if(nout = clipPoly(out, nout, in,  &planes[5]), nout == 0) continue;
		out = in;

		// Insert new triangles
		x1 = mstate->numVertices;
		for(j = 0; j < nout; j++){
			x2 = mstate->numVertices++;
			camverts[x2] = out[j];
			cam2screen(&scrverts[x2], &camverts[x2]);
		}
		x2 = x1+1;
		for(j = 0; j < nout-2; j++){
			clipindices[0] = x1;
			clipindices[1] = x2++;
			clipindices[2] = x2;
			clipindices += 3;
			newNumPrims++;
		}
	}
	mstate->numPrimitives = newNumPrims;
}

static void
submitTriangles(RWDEVICE::Im2DVertex *scrverts, int32 numVerts, uint16 *indices, int32 numTris)
{
	rw::SetRenderStatePtr(rw::TEXTURERASTER, nil);
	if(dosoftras)
		rastest_renderTriangles(scrverts, numVerts, indices, numTris);
	else{
		//int i;
		//for(i = 0; i < numVerts; i++){
		//	scrverts[i].x = (int)(scrverts[i].x*16.0f) / 16.0f;
		//	scrverts[i].y = (int)(scrverts[i].y*16.0f) / 16.0f;
		//}
		im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, scrverts, numVerts,
			indices, numTris*3);
	}
}


static void
drawMesh(MeshState *mstate, ObjSpace3DVertex *objverts, uint16 *indices)
{
	CamSpace3DVertex *camverts;
	Im2DVertex *scrverts;
	uint16 *cullindices, *clipindices;
	uint32 numClip;

	camverts = rwNewT(CamSpace3DVertex, mstate->numVertices, MEMDUR_FUNCTION);
	scrverts = rwNewT(Im2DVertex, mstate->numVertices, MEMDUR_FUNCTION);
	cullindices = rwNewT(uint16, mstate->numPrimitives*3, MEMDUR_FUNCTION);

	transform(mstate, objverts, camverts, scrverts);

	numClip = cullTriangles(mstate, camverts, indices, cullindices);

//	int32 i;
//	for(i = 0; i < mstate->numVertices; i++){
//		if(camverts[i].clipFlags & CLIPX)
//			camverts[i].color.red = 255;
//		if(camverts[i].clipFlags & CLIPY)
//			camverts[i].color.green = 255;
//		if(camverts[i].clipFlags & CLIPZ)
//			camverts[i].color.blue = 255;
//	}

	light(mstate, objverts, camverts);

//	mstate->matCol.red = 255;
//	mstate->matCol.green = 255;
//	mstate->matCol.blue = 255;

	postlight(mstate, camverts, scrverts);

	// each triangle can have a maximum of 9 vertices (7 triangles) after clipping
	// so resize to whatever we may need
	camverts = rwResizeT(CamSpace3DVertex, camverts, mstate->numVertices + numClip*9, MEMDUR_FUNCTION);
	scrverts = rwResizeT(Im2DVertex, scrverts, mstate->numVertices + numClip*9, MEMDUR_FUNCTION);
	clipindices = rwNewT(uint16, mstate->numPrimitives*3 + numClip*7*3, MEMDUR_FUNCTION);

//	clipTriangles(mstate, camverts, scrverts, cullindices, clipindices);
	clipTriangles2(mstate, camverts, scrverts, cullindices, clipindices);

	submitTriangles(scrverts, mstate->numVertices, clipindices, mstate->numPrimitives);

	rwFree(camverts);
	rwFree(scrverts);
	rwFree(cullindices);
	rwFree(clipindices);
}

static void
drawAtomic(Atomic *atomic)
{
	MeshState mstate;
	Matrix lightmat;
	Geometry *geo;
	MeshHeader *header;
	InstanceData *inst;
	int i;
	Camera *cam = (Camera*)engine->currentCamera;

	instanceAtomic(atomic);

	mstate.obj2world = *atomic->getFrame()->getLTM();
	mstate.obj2cam = mstate.obj2world;
	mstate.obj2cam.transform(&cam->viewMatrix, COMBINEPOSTCONCAT);
	Matrix::invert(&lightmat, &mstate.obj2world);
	enumLights(&lightmat);

	geo = atomic->geometry;
	header = geo->meshHeader;
	inst = ((InstanceDataHeader*)geo->instData)->inst;
	for(i = 0; i < header->numMeshes; i++){
		mstate.flags = geo->flags;
		mstate.numVertices = inst->numVertices;
		mstate.numPrimitives = inst->numIndices / 3;
		mstate.surfProps = inst->material->surfaceProps;
		mstate.matCol = inst->material->color;
		drawMesh(&mstate, inst->vertices, inst->indices);
		inst++;
	}
}

void
tlTest(Clump *clump)
{
	FORLIST(lnk, clump->atomics){
		Atomic *a = Atomic::fromClump(lnk);
		drawAtomic(a);
	}
}

}

static Im2DVertex *clipverts;
static int32 numClipverts;

void
genIm3DTransform(void *vertices, int32 numVertices, Matrix *world)
{
	Im3DVertex *objverts;
	V3d pos;
	Matrix xform;
	Camera *cam;
	int32 i;
	objverts = (Im3DVertex*)vertices;

	cam = (Camera*)engine->currentCamera;
	int32 width = cam->frameBuffer->width;
	int32 height = cam->frameBuffer->height;


	xform = cam->viewMatrix;
	if(world)
		xform.transform(world, COMBINEPRECONCAT);

	clipverts = rwNewT(Im2DVertex, numVertices, MEMDUR_EVENT);
	numClipverts = numVertices;

	for(i = 0; i < numVertices; i++){
		V3d::transformPoints(&pos, &objverts[i].position, 1, &xform);

		float32 recipZ = 1.0f/pos.z;
		RGBA c = objverts[i].getColor();

		clipverts[i].setScreenX(pos.x * recipZ * width);
		clipverts[i].setScreenY(pos.y * recipZ * height);
		clipverts[i].setScreenZ(recipZ * cam->zScale + cam->zShift);
		clipverts[i].setCameraZ(pos.z);
		clipverts[i].setRecipCameraZ(recipZ);
		clipverts[i].setColor(c.red, c.green, c.blue, c.alpha);
		clipverts[i].setU(objverts[i].u, recipZ);
		clipverts[i].setV(objverts[i].v, recipZ);
	}
}

void
genIm3DRenderIndexed(PrimitiveType prim, void *indices, int32 numIndices)
{
	im2d::RenderIndexedPrimitive(prim, clipverts, numClipverts, indices, numIndices);
}

void
genIm3DEnd(void)
{
	rwFree(clipverts);
	clipverts = nil;
	numClipverts = 0;
}

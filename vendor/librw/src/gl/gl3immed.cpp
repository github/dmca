#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwrender.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#ifdef RW_OPENGL
#include "rwgl3.h"
#include "rwgl3impl.h"
#include "rwgl3shader.h"

namespace rw {
namespace gl3 {

uint32 im2DVbo, im2DIbo;
#ifdef RW_GL_USE_VAOS
uint32 im2DVao;
#endif

Shader *im2dOverrideShader;

static int32 u_xform;

#define STARTINDICES 10000
#define STARTVERTICES 10000

static Shader *im2dShader;
static AttribDesc im2dattribDesc[3] = {
	{ ATTRIB_POS,        GL_FLOAT,         GL_FALSE, 4,
		sizeof(Im2DVertex), 0 },
	{ ATTRIB_COLOR,      GL_UNSIGNED_BYTE, GL_TRUE,  4,
		sizeof(Im2DVertex), offsetof(Im2DVertex, r) },
	{ ATTRIB_TEXCOORDS0, GL_FLOAT,         GL_FALSE, 2,
		sizeof(Im2DVertex), offsetof(Im2DVertex, u) },
};

static int primTypeMap[] = {
	GL_POINTS,	// invalid
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_POINTS
};

void
openIm2D(void)
{
	u_xform = registerUniform("u_xform");

#include "shaders/im2d_gl.inc"
#include "shaders/simple_fs_gl.inc"
	const char *vs[] = { shaderDecl, header_vert_src, im2d_vert_src, nil };
	const char *fs[] = { shaderDecl, header_frag_src, simple_frag_src, nil };
	im2dShader = Shader::create(vs, fs);
	assert(im2dShader);

	glGenBuffers(1, &im2DIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, im2DIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, STARTINDICES*2, nil, GL_STREAM_DRAW);

	glGenBuffers(1, &im2DVbo);
	glBindBuffer(GL_ARRAY_BUFFER, im2DVbo);
	glBufferData(GL_ARRAY_BUFFER, STARTVERTICES*sizeof(Im2DVertex), nil, GL_STREAM_DRAW);

#ifdef RW_GL_USE_VAOS
	glGenVertexArrays(1, &im2DVao);
	glBindVertexArray(im2DVao);
	setAttribPointers(im2dattribDesc, 3);
#endif
}

void
closeIm2D(void)
{
	glDeleteBuffers(1, &im2DIbo);
	glDeleteBuffers(1, &im2DVbo);
#ifdef RW_GL_USE_VAOS
	glDeleteVertexArrays(1, &im2DVao);
#endif
	im2dShader->destroy();
	im2dShader = nil;
}

static Im2DVertex tmpprimbuf[3];

void
im2DRenderLine(void *vertices, int32 numVertices, int32 vert1, int32 vert2)
{
	Im2DVertex *verts = (Im2DVertex*)vertices;
	tmpprimbuf[0] = verts[vert1];
	tmpprimbuf[1] = verts[vert2];
	im2DRenderPrimitive(PRIMTYPELINELIST, tmpprimbuf, 2);
}

void
im2DRenderTriangle(void *vertices, int32 numVertices, int32 vert1, int32 vert2, int32 vert3)
{
	Im2DVertex *verts = (Im2DVertex*)vertices;
	tmpprimbuf[0] = verts[vert1];
	tmpprimbuf[1] = verts[vert2];
	tmpprimbuf[2] = verts[vert3];
	im2DRenderPrimitive(PRIMTYPETRILIST, tmpprimbuf, 3);
}

void
im2DSetXform(void)
{
	GLfloat xform[4];
	Camera *cam;
	cam = (Camera*)engine->currentCamera;
	xform[0] = 2.0f/cam->frameBuffer->width;
	xform[1] = -2.0f/cam->frameBuffer->height;
	xform[2] = -1.0f;
	xform[3] = 1.0f;
	glUniform4fv(currentShader->uniformLocations[u_xform], 1, xform);
}

void
im2DRenderPrimitive(PrimitiveType primType, void *vertices, int32 numVertices)
{
#ifdef RW_GL_USE_VAOS
	glBindVertexArray(im2DVao);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, im2DVbo);
	glBufferData(GL_ARRAY_BUFFER, STARTVERTICES*sizeof(Im2DVertex), nil, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Im2DVertex), vertices);

	if(im2dOverrideShader)
		im2dOverrideShader->use();
	else
		im2dShader->use();
#ifndef RW_GL_USE_VAOS
	setAttribPointers(im2dattribDesc, 3);
#endif

	im2DSetXform();

	flushCache();
	glDrawArrays(primTypeMap[primType], 0, numVertices);
#ifndef RW_GL_USE_VAOS
	disableAttribPointers(im2dattribDesc, 3);
#endif
}

void
im2DRenderIndexedPrimitive(PrimitiveType primType,
	void *vertices, int32 numVertices,
	void *indices, int32 numIndices)
{
#ifdef RW_GL_USE_VAOS
	glBindVertexArray(im2DVao);
#endif

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, im2DIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, STARTINDICES*2, nil, GL_STREAM_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numIndices*2, indices);

	glBindBuffer(GL_ARRAY_BUFFER, im2DVbo);
	glBufferData(GL_ARRAY_BUFFER, STARTVERTICES*sizeof(Im2DVertex), nil, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Im2DVertex), vertices);

	if(im2dOverrideShader)
		im2dOverrideShader->use();
	else
		im2dShader->use();
#ifndef RW_GL_USE_VAOS
	setAttribPointers(im2dattribDesc, 3);
#endif

	im2DSetXform();

	flushCache();
	glDrawElements(primTypeMap[primType], numIndices,
	               GL_UNSIGNED_SHORT, nil);
#ifndef RW_GL_USE_VAOS
	disableAttribPointers(im2dattribDesc, 3);
#endif
}


// Im3D


static Shader *im3dShader;
static AttribDesc im3dattribDesc[3] = {
	{ ATTRIB_POS,        GL_FLOAT,         GL_FALSE, 3,
		sizeof(Im3DVertex), 0 },
	{ ATTRIB_COLOR,      GL_UNSIGNED_BYTE, GL_TRUE,  4,
		sizeof(Im3DVertex), offsetof(Im3DVertex, r) },
	{ ATTRIB_TEXCOORDS0, GL_FLOAT,         GL_FALSE, 2,
		sizeof(Im3DVertex), offsetof(Im3DVertex, u) },
};
static uint32 im3DVbo, im3DIbo;
#ifdef RW_GL_USE_VAOS
static uint32 im3DVao;
#endif
static int32 num3DVertices;	// not actually needed here

void
openIm3D(void)
{
#include "shaders/im3d_gl.inc"
#include "shaders/simple_fs_gl.inc"
	const char *vs[] = { shaderDecl, header_vert_src, im3d_vert_src, nil };
	const char *fs[] = { shaderDecl, header_frag_src, simple_frag_src, nil };
	im3dShader = Shader::create(vs, fs);
	assert(im3dShader);

	glGenBuffers(1, &im3DIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, im3DIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, STARTINDICES*2, nil, GL_STREAM_DRAW);

	glGenBuffers(1, &im3DVbo);
	glBindBuffer(GL_ARRAY_BUFFER, im3DVbo);
	glBufferData(GL_ARRAY_BUFFER, STARTVERTICES*sizeof(Im3DVertex), nil, GL_STREAM_DRAW);

#ifdef RW_GL_USE_VAOS
	glGenVertexArrays(1, &im3DVao);
	glBindVertexArray(im3DVao);
	setAttribPointers(im3dattribDesc, 3);
#endif
}

void
closeIm3D(void)
{
	glDeleteBuffers(1, &im3DIbo);
	glDeleteBuffers(1, &im3DVbo);
#ifdef RW_GL_USE_VAOS
	glDeleteVertexArrays(1, &im3DVao);
#endif
	im3dShader->destroy();
	im3dShader = nil;
}

void
im3DTransform(void *vertices, int32 numVertices, Matrix *world, uint32 flags)
{
	if(world == nil){
		static Matrix ident;
		ident.setIdentity();
		world = &ident;
	}
	setWorldMatrix(world);
	im3dShader->use();

	if((flags & im3d::VERTEXUV) == 0)
		SetRenderStatePtr(TEXTURERASTER, nil);

#ifdef RW_GL_USE_VAOS
	glBindVertexArray(im2DVao);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, im3DVbo);
	glBufferData(GL_ARRAY_BUFFER, STARTVERTICES*sizeof(Im3DVertex), nil, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Im3DVertex), vertices);
#ifndef RW_GL_USE_VAOS
	setAttribPointers(im3dattribDesc, 3);
#endif
	num3DVertices = numVertices;
}

void
im3DRenderPrimitive(PrimitiveType primType)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, im3DIbo);

	flushCache();
	glDrawArrays(primTypeMap[primType], 0, num3DVertices);
}

void
im3DRenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, im3DIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, STARTINDICES*2, nil, GL_STREAM_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numIndices*2, indices);

	flushCache();
	glDrawElements(primTypeMap[primType], numIndices,
	               GL_UNSIGNED_SHORT, nil);
}

void
im3DEnd(void)
{
#ifndef RW_GL_USE_VAOS
	disableAttribPointers(im3dattribDesc, 3);
#endif
}

}
}

#endif

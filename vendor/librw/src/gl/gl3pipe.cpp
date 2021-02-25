#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"

#include "rwgl3.h"
#include "rwgl3shader.h"

namespace rw {
namespace gl3 {

// TODO: make some of these things platform-independent

#ifdef RW_OPENGL

void
freeInstanceData(Geometry *geometry)
{
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_GL3)
		return;
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	geometry->instData = nil;
	glDeleteBuffers(1, &header->ibo);
	glDeleteBuffers(1, &header->vbo);
#ifdef RW_GL_USE_VAOS
	glDeleteBuffers(1, &header->vao);
#endif
	rwFree(header->indexBuffer);
	rwFree(header->vertexBuffer);
	rwFree(header->attribDesc);
	rwFree(header->inst);
	rwFree(header);
}

void*
destroyNativeData(void *object, int32, int32)
{
	freeInstanceData((Geometry*)object);
	return object;
}

static InstanceDataHeader*
instanceMesh(rw::ObjPipeline *rwpipe, Geometry *geo)
{
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	MeshHeader *meshh = geo->meshHeader;
	geo->instData = header;
	header->platform = PLATFORM_GL3;

	header->serialNumber = meshh->serialNum;
	header->numMeshes = meshh->numMeshes;
	header->primType = meshh->flags == 1 ? GL_TRIANGLE_STRIP : GL_TRIANGLES;
	header->totalNumVertex = geo->numVertices;
	header->totalNumIndex = meshh->totalIndices;
	header->inst = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);

	header->indexBuffer = rwNewT(uint16, header->totalNumIndex, MEMDUR_EVENT | ID_GEOMETRY);
	InstanceData *inst = header->inst;
	Mesh *mesh = meshh->getMeshes();
	uint32 offset = 0;
	for(uint32 i = 0; i < header->numMeshes; i++){
		findMinVertAndNumVertices(mesh->indices, mesh->numIndices,
		                          &inst->minVert, &inst->numVertices);
		assert(inst->minVert != 0xFFFFFFFF);
		inst->numIndex = mesh->numIndices;
		inst->material = mesh->material;
		inst->vertexAlpha = 0;
		inst->program = 0;
		inst->offset = offset;
		memcpy((uint8*)header->indexBuffer + inst->offset,
		       mesh->indices, inst->numIndex*2);
		offset += inst->numIndex*2;
		mesh++;
		inst++;
	}

	header->vertexBuffer = nil;
	header->numAttribs = 0;
	header->attribDesc = nil;
	header->ibo = 0;
	header->vbo = 0;

#ifdef RW_GL_USE_VAOS
	glGenVertexArrays(1, &header->vao);
	glBindVertexArray(header->vao);
#endif
	glGenBuffers(1, &header->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, header->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, header->totalNumIndex*2,
			header->indexBuffer, GL_STATIC_DRAW);

	return header;
}

static void
instance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	// don't try to (re)instance native data
	if(geo->flags & Geometry::NATIVE)
		return;

	InstanceDataHeader *header = (InstanceDataHeader*)geo->instData;
	if(geo->instData){
		// Already have instanced data, so check if we have to reinstance
		assert(header->platform == PLATFORM_GL3);
		if(header->serialNumber != geo->meshHeader->serialNum){
			// Mesh changed, so reinstance everything
			freeInstanceData(geo);
		}
	}

	// no instance or complete reinstance
	if(geo->instData == nil){
		geo->instData = instanceMesh(rwpipe, geo);
		pipe->instanceCB(geo, (InstanceDataHeader*)geo->instData, 0);
	}else if(geo->lockedSinceInst)
		pipe->instanceCB(geo, (InstanceDataHeader*)geo->instData, 1);

	geo->lockedSinceInst = 0;
}

static void
uninstance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	assert(0 && "can't uninstance");
}

static void
render(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	pipe->instance(atomic);
	assert(geo->instData != nil);
	assert(geo->instData->platform == PLATFORM_GL3);
	if(pipe->renderCB)
		pipe->renderCB(atomic, (InstanceDataHeader*)geo->instData);
}

void
ObjPipeline::init(void)
{
	this->rw::ObjPipeline::init(PLATFORM_GL3);
	this->impl.instance = gl3::instance;
	this->impl.uninstance = gl3::uninstance;
	this->impl.render = gl3::render;
	this->instanceCB = nil;
	this->uninstanceCB = nil;
	this->renderCB = nil;
}

ObjPipeline*
ObjPipeline::create(void)
{
	ObjPipeline *pipe = rwNewT(ObjPipeline, 1, MEMDUR_GLOBAL);
	pipe->init();
	return pipe;
}

void
defaultInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance)
{
	AttribDesc *attribs, *a;

	bool isPrelit = !!(geo->flags & Geometry::PRELIT);
	bool hasNormals = !!(geo->flags & Geometry::NORMALS);

	if(!reinstance){
		AttribDesc tmpAttribs[12];
		uint32 stride;

		//
		// Create attribute descriptions
		//
		a = tmpAttribs;
		stride = 0;

		// Positions
		a->index = ATTRIB_POS;
		a->size = 3;
		a->type = GL_FLOAT;
		a->normalized = GL_FALSE;
		a->offset = stride;
		stride += 12;
		a++;

		// Normals
		// TODO: compress
		if(hasNormals){
			a->index = ATTRIB_NORMAL;
			a->size = 3;
			a->type = GL_FLOAT;
			a->normalized = GL_FALSE;
			a->offset = stride;
			stride += 12;
			a++;
		}

		// Prelighting
		if(isPrelit){
			a->index = ATTRIB_COLOR;
			a->size = 4;
			a->type = GL_UNSIGNED_BYTE;
			a->normalized = GL_TRUE;
			a->offset = stride;
			stride += 4;
			a++;
		}

		// Texture coordinates
		for(int32 n = 0; n < geo->numTexCoordSets; n++){
			a->index = ATTRIB_TEXCOORDS0+n;
			a->size = 2;
			a->type = GL_FLOAT;
			a->normalized = GL_FALSE;
			a->offset = stride;
			stride += 8;
			a++;
		}

		header->numAttribs = a - tmpAttribs;
		for(a = tmpAttribs; a != &tmpAttribs[header->numAttribs]; a++)
			a->stride = stride;
		header->attribDesc = rwNewT(AttribDesc, header->numAttribs, MEMDUR_EVENT | ID_GEOMETRY);
		memcpy(header->attribDesc, tmpAttribs,
		       header->numAttribs*sizeof(AttribDesc));

		//
		// Allocate vertex buffer
		//
		header->vertexBuffer = rwNewT(uint8, header->totalNumVertex*stride, MEMDUR_EVENT | ID_GEOMETRY);
		assert(header->vbo == 0);
		glGenBuffers(1, &header->vbo);
	}

	attribs = header->attribDesc;

	//
	// Fill vertex buffer
	//

	uint8 *verts = header->vertexBuffer;

	// Positions
	if(!reinstance || geo->lockedSinceInst&Geometry::LOCKVERTICES){
		for(a = attribs; a->index != ATTRIB_POS; a++)
			;
		instV3d(VERT_FLOAT3, verts + a->offset,
			geo->morphTargets[0].vertices,
			header->totalNumVertex, a->stride);
	}

	// Normals
	if(hasNormals && (!reinstance || geo->lockedSinceInst&Geometry::LOCKNORMALS)){
		for(a = attribs; a->index != ATTRIB_NORMAL; a++)
			;
		instV3d(VERT_FLOAT3, verts + a->offset,
			geo->morphTargets[0].normals,
			header->totalNumVertex, a->stride);
	}

	// Prelighting
	if(isPrelit && (!reinstance || geo->lockedSinceInst&Geometry::LOCKPRELIGHT)){
		for(a = attribs; a->index != ATTRIB_COLOR; a++)
			;
		int n = header->numMeshes;
		InstanceData *inst = header->inst;
		while(n--){
			assert(inst->minVert != 0xFFFFFFFF);
			inst->vertexAlpha = instColor(VERT_RGBA,
				verts + a->offset + a->stride*inst->minVert,
				geo->colors + inst->minVert,
				inst->numVertices, a->stride);
			inst++;
		}
	}

	// Texture coordinates
	for(int32 n = 0; n < geo->numTexCoordSets; n++){
		if(!reinstance || geo->lockedSinceInst&(Geometry::LOCKTEXCOORDS<<n)){
			for(a = attribs; a->index != ATTRIB_TEXCOORDS0+n; a++)
				;
			instTexCoords(VERT_FLOAT2, verts + a->offset,
				geo->texCoords[n],
				header->totalNumVertex, a->stride);
		}
	}

#ifdef RW_GL_USE_VAOS
	glBindVertexArray(header->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, header->ibo);
#endif
	glBindBuffer(GL_ARRAY_BUFFER, header->vbo);
	glBufferData(GL_ARRAY_BUFFER, header->totalNumVertex*attribs[0].stride,
	             header->vertexBuffer, GL_STATIC_DRAW);
#ifdef RW_GL_USE_VAOS
	setAttribPointers(header->attribDesc, header->numAttribs);
	glBindVertexArray(0);
#endif
}

void
defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header)
{
	assert(0 && "can't uninstance");
}

ObjPipeline*
makeDefaultPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->renderCB = defaultRenderCB;
	return pipe;
}

#else
void *destroyNativeData(void *object, int32, int32) { return object; }
#endif

}
}

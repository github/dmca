#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"
#include "rwanim.h"
#include "rwplugins.h"
#include "ps2/rwps2.h"
#include "ps2/rwps2plg.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"
#include "gl/rwwdgl.h"
#include "gl/rwgl3.h"

#define PLUGIN_ID 2

namespace rw {

static uint16 nextSerialNum = 1;

// Mesh

// Allocate a mesh header, meshes and optionally indices.
// If existing meshes already exist, retain their information.
MeshHeader*
Geometry::allocateMeshes(int32 numMeshes, uint32 numIndices, bool32 noIndices)
{
	uint32 sz;
	MeshHeader *mh;
	Mesh *m;
	uint16 *indices;
	int32 oldNumMeshes;
	int32 i;
	sz = sizeof(MeshHeader) + numMeshes*sizeof(Mesh);
	if(!noIndices)
		sz += numIndices*sizeof(uint16);
	if(this->meshHeader){
		oldNumMeshes = this->meshHeader->numMeshes;
		mh = (MeshHeader*)rwResize(this->meshHeader, sz, MEMDUR_EVENT | ID_GEOMETRY);
		this->meshHeader = mh;
	}else{
		oldNumMeshes = 0;
		mh = (MeshHeader*)rwNew(sz, MEMDUR_EVENT | ID_GEOMETRY);
		mh->flags = 0;
		this->meshHeader = mh;
	}
	mh->numMeshes = numMeshes;
	mh->serialNum = nextSerialNum++;
	mh->totalIndices = numIndices;
	m = mh->getMeshes();
	indices = (uint16*)&m[numMeshes];
	for(i = 0; i < mh->numMeshes; i++){
		// keep these
		if(i >= oldNumMeshes){
			m->material = nil;
			m->numIndices = 0;
		}
		// always init indices
		if(noIndices)
			m->indices = nil;
		else{
			m->indices = indices;
			indices += m->numIndices;
		}
		m++;
	}
	return mh;
}

void
MeshHeader::setupIndices(void)
{
	int32 i;
	uint16 *indices;
	Mesh *m;
	m = this->getMeshes();
	indices = m->indices;
	// return if native
	if(indices == nil)
		return;
	for(i = 0; i < this->numMeshes; i++){
		m->indices = indices;
		indices += m->numIndices;
		m++;
	}
}

struct MeshHeaderStream
{
	uint32 flags;
	uint32 numMeshes;
	uint32 totalIndices;
};

struct MeshStream
{
	uint32 numIndices;
	int32 matIndex;
};

static Stream*
readMesh(Stream *stream, int32 len, void *object, int32, int32)
{
	MeshHeaderStream mhs;
	MeshStream ms;
	MeshHeader *mh;
	Mesh *mesh;
	int32 indbuf[256];
	uint16 *indices;
	Geometry *geo = (Geometry*)object;

	stream->read32(&mhs, sizeof(MeshHeaderStream));
	// Have to do this dance for War Drum's meshes
	bool32 hasData = len > int32(sizeof(MeshHeaderStream)+mhs.numMeshes*sizeof(MeshStream));
	assert(geo->meshHeader == nil);
	geo->meshHeader = nil;
	mh = geo->allocateMeshes(mhs.numMeshes, mhs.totalIndices, 
		geo->flags & Geometry::NATIVE && !hasData);
	mh->flags = mhs.flags;

	mesh = mh->getMeshes();
	indices = mesh->indices;
	for(uint32 i = 0; i < mh->numMeshes; i++){
		stream->read32(&ms, sizeof(MeshStream));
		mesh->numIndices = ms.numIndices;
		mesh->material = geo->matList.materials[ms.matIndex];
		if(geo->flags & Geometry::NATIVE){
			// War Drum OpenGL stores uint16 indices here
			if(hasData){
				mesh->indices = indices;
				indices += mesh->numIndices;
				stream->read16(mesh->indices,
				            mesh->numIndices*2);
			}
		}else{
			mesh->indices = indices;
			indices += mesh->numIndices;
			uint16 *ind = mesh->indices;
			int32 numIndices = mesh->numIndices;
			for(; numIndices > 0; numIndices -= 256){
				int32 n = numIndices < 256 ? numIndices : 256;
				stream->read32(indbuf, n*4);
				for(int32 j = 0; j < n; j++)
					ind[j] = indbuf[j];
				ind += n;
			}
		}
		mesh++;
	}
	return stream;
}

static Stream*
writeMesh(Stream *stream, int32, void *object, int32, int32)
{
	MeshHeaderStream mhs;
	MeshStream ms;
	int32 indbuf[256];
	Geometry *geo = (Geometry*)object;
	mhs.flags = geo->meshHeader->flags;
	mhs.numMeshes = geo->meshHeader->numMeshes;
	mhs.totalIndices = geo->meshHeader->totalIndices;
	stream->write32(&mhs, sizeof(MeshHeaderStream));
	Mesh *mesh = geo->meshHeader->getMeshes();
	for(uint32 i = 0; i < geo->meshHeader->numMeshes; i++){
		ms.numIndices = mesh->numIndices;
		ms.matIndex = geo->matList.findIndex(mesh->material);
		stream->write32(&ms, sizeof(MeshStream));
		if(geo->flags & Geometry::NATIVE){
			assert(geo->instData != nil);
			if(geo->instData->platform == PLATFORM_WDGL)
				stream->write16(mesh->indices,
				            mesh->numIndices*2);
		}else{
			uint16 *ind = mesh->indices;
			int32 numIndices = mesh->numIndices;
			for(; numIndices > 0; numIndices -= 256){
				int32 n = numIndices < 256 ? numIndices : 256;
				for(int32 j = 0; j < n; j++)
					indbuf[j] = ind[j];
				stream->write32(indbuf, n*4);
				ind += n;
			}
		}
		mesh++;
	}
	return stream;
}

static int32
getSizeMesh(void *object, int32, int32)
{
	Geometry *geo = (Geometry*)object;
	if(geo->meshHeader == nil)
		return -1;
	int32 size = 12 + geo->meshHeader->numMeshes*8;
	if(geo->flags & Geometry::NATIVE){
		assert(geo->instData != nil);
		if(geo->instData->platform == PLATFORM_WDGL)
			size += geo->meshHeader->totalIndices*2;
	}else{
		size += geo->meshHeader->totalIndices*4;
	}
	return size;
}

void
registerMeshPlugin(void)
{
	Geometry::registerPlugin(0, ID_MESH, nil, nil, nil);
	Geometry::registerPluginStream(ID_MESH, readMesh, writeMesh, getSizeMesh);
}

// Returns the maximum number of triangles. Just so
// we can allocate enough before instancing. This does not
// take into account degerate triangles or ADC bits as
// we don't look at the data.
uint32
MeshHeader::guessNumTriangles(void)
{
	if(this->flags == MeshHeader::TRISTRIP)
		return this->totalIndices - 2*this->numMeshes;
	else
		return this->totalIndices/3;
}

// Native Data

static void*
destroyNativeData(void *object, int32 offset, int32 size)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil)
		return object;
	if(geometry->instData->platform == PLATFORM_PS2)
		return ps2::destroyNativeData(object, offset, size);
	if(geometry->instData->platform == PLATFORM_WDGL)
		return wdgl::destroyNativeData(object, offset, size);
	if(geometry->instData->platform == PLATFORM_XBOX)
		return xbox::destroyNativeData(object, offset, size);
	if(geometry->instData->platform == PLATFORM_D3D8)
		return d3d8::destroyNativeData(object, offset, size);
	if(geometry->instData->platform == PLATFORM_D3D9)
		return d3d9::destroyNativeData(object, offset, size);
	if(geometry->instData->platform == PLATFORM_GL3)
		return gl3::destroyNativeData(object, offset, size);
	return object;
}

static Stream*
readNativeData(Stream *stream, int32 len, void *object, int32 o, int32 s)
{
	ChunkHeaderInfo header;
	uint32 libid;
	uint32 platform;
	// ugly hack to find out platform
	stream->seek(-4);
	libid = stream->readU32();
	readChunkHeaderInfo(stream, &header);
	if(header.type == ID_STRUCT &&
	   libraryIDPack(header.version, header.build) == libid){
		platform = stream->readU32();
		stream->seek(-16);
		if(platform == PLATFORM_PS2)
			return ps2::readNativeData(stream, len, object, o, s);
		else if(platform == PLATFORM_XBOX)
			return xbox::readNativeData(stream, len, object, o, s);
		else if(platform == PLATFORM_D3D8)
			return d3d8::readNativeData(stream, len, object, o, s);
		else if(platform == PLATFORM_D3D9)
			return d3d9::readNativeData(stream, len, object, o, s);
		else{
			fprintf(stderr, "unknown platform %d\n", platform);
			stream->seek(len);
		}
	}else{
		stream->seek(-12);
		wdgl::readNativeData(stream, len, object, o, s);
	}
	return stream;
}

static Stream*
writeNativeData(Stream *stream, int32 len, void *object, int32 o, int32 s)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil)
		return stream;
	if(geometry->instData->platform == PLATFORM_PS2)
		return ps2::writeNativeData(stream, len, object, o, s);
	else if(geometry->instData->platform == PLATFORM_WDGL)
		return wdgl::writeNativeData(stream, len, object, o, s);
	else if(geometry->instData->platform == PLATFORM_XBOX)
		return xbox::writeNativeData(stream, len, object, o, s);
	else if(geometry->instData->platform == PLATFORM_D3D8)
		return d3d8::writeNativeData(stream, len, object, o, s);
	else if(geometry->instData->platform == PLATFORM_D3D9)
		return d3d9::writeNativeData(stream, len, object, o, s);
	return stream;
}

static int32
getSizeNativeData(void *object, int32 offset, int32 size)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil)
		return 0;
	if(geometry->instData->platform == PLATFORM_PS2)
		return ps2::getSizeNativeData(object, offset, size);
	else if(geometry->instData->platform == PLATFORM_WDGL)
		return wdgl::getSizeNativeData(object, offset, size);
	else if(geometry->instData->platform == PLATFORM_XBOX)
		return xbox::getSizeNativeData(object, offset, size);
	else if(geometry->instData->platform == PLATFORM_D3D8)
		return d3d8::getSizeNativeData(object, offset, size);
	else if(geometry->instData->platform == PLATFORM_D3D9)
		return d3d9::getSizeNativeData(object, offset, size);
	return 0;
}

void
registerNativeDataPlugin(void)
{
	Geometry::registerPlugin(0, ID_NATIVEDATA,
	                         nil, destroyNativeData, nil);
	Geometry::registerPluginStream(ID_NATIVEDATA,
	                               readNativeData,
	                               writeNativeData,
	                               getSizeNativeData);
}

}

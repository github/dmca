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
#include "../rwanim.h"
#include "../rwplugins.h"
#include "rwps2.h"
#include "rwps2plg.h"

#include "rwps2impl.h"

#define PLUGIN_ID 2

namespace rw {
namespace ps2 {

#define ALIGNPTR(p,a) ((uint8*)(((uintptr)(p)+a-1) & ~(uintptr)(a-1)))

static void*
driverOpen(void *o, int32, int32)
{
	engine->driver[PLATFORM_PS2]->defaultPipeline = makeDefaultPipeline();

	engine->driver[PLATFORM_PS2]->rasterNativeOffset = nativeRasterOffset;
	engine->driver[PLATFORM_PS2]->rasterCreate = rasterCreate;
	engine->driver[PLATFORM_PS2]->rasterLock = rasterLock;
	engine->driver[PLATFORM_PS2]->rasterUnlock = rasterUnlock;
	engine->driver[PLATFORM_PS2]->rasterLockPalette = rasterLockPalette;
	engine->driver[PLATFORM_PS2]->rasterUnlockPalette = rasterUnlockPalette;
	engine->driver[PLATFORM_PS2]->rasterNumLevels = rasterNumLevels;
	engine->driver[PLATFORM_PS2]->imageFindRasterFormat = imageFindRasterFormat;
	engine->driver[PLATFORM_PS2]->rasterFromImage = rasterFromImage;
	engine->driver[PLATFORM_PS2]->rasterToImage = rasterToImage;

	return o;
}

static void*
driverClose(void *o, int32, int32)
{
	return o;
}

void
registerPlatformPlugins(void)
{
	Driver::registerPlugin(PLATFORM_PS2, 0, PLATFORM_PS2,
	                       driverOpen, driverClose);

	registerNativeRaster();
}

ObjPipeline *defaultObjPipe;
MatPipeline *defaultMatPipe;

void*
destroyNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_PS2)
		return object;
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	for(uint32 i = 0; i < header->numMeshes; i++)
		rwFree(header->instanceMeshes[i].dataRaw);
	rwFree(header->instanceMeshes);
	rwFree(header);
	geometry->instData = nil;
	return object;
}

Stream*
readNativeData(Stream *stream, int32, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_PS2){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	geometry->instData = header;
	header->platform = PLATFORM_PS2;
	assert(geometry->meshHeader != nil);
	header->numMeshes = geometry->meshHeader->numMeshes;
	header->instanceMeshes = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);
	Mesh *m = geometry->meshHeader->getMeshes();
	for(uint32 i = 0; i < header->numMeshes; i++){
		InstanceData *instance = &header->instanceMeshes[i];
		uint32 buf[2];
		stream->read32(buf, 8);
		instance->dataSize = buf[0];
		instance->dataRaw = rwNewT(uint8, instance->dataSize+0x7F, MEMDUR_EVENT | ID_GEOMETRY);
		instance->data = ALIGNPTR(instance->dataRaw, 0x80);
#ifdef RW_PS2
		uint32 a = (uint32)instance->data;
		assert(a % 0x10 == 0);
#endif
		stream->read8(instance->data, instance->dataSize);
#ifdef RW_PS2
		if(!buf[1])
			fixDmaOffsets(instance);
#endif
		instance->material = m->material;
//		sizedebug(instance);
		m++;
	}
	return stream;
}

Stream*
writeNativeData(Stream *stream, int32 len, void *object, int32, int32)
{
	ASSERTLITTLE;
	Geometry *geometry = (Geometry*)object;
	writeChunkHeader(stream, ID_STRUCT, len-12);
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_PS2)
		return stream;
	stream->writeU32(PLATFORM_PS2);
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	for(uint32 i = 0; i < header->numMeshes; i++){
		InstanceData *instance = &header->instanceMeshes[i];
		uint32 buf[2];
		buf[0] = instance->dataSize;
		buf[1] = unfixDmaOffsets(instance);
		stream->write32(buf, 8);
		stream->write8(instance->data, instance->dataSize);
#ifdef RW_PS2
		if(!buf[1])
			fixDmaOffsets(instance);
#endif
	}
	return stream;
}

int32
getSizeNativeData(void *object, int32, int32)
{
	Geometry *geometry = (Geometry*)object;
	int32 size = 16;
	if(geometry->instData == nil ||
	   geometry->instData->platform != PLATFORM_PS2)
		return 0;
	InstanceDataHeader *header = (InstanceDataHeader*)geometry->instData;
	for(uint32 i = 0; i < header->numMeshes; i++){
		InstanceData *instance = &header->instanceMeshes[i];
		size += 8;
		size += instance->dataSize;
	}
	return size;
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

// Patch DMA ref ADDR fields to point to the actual data.
#ifdef RW_PS2
void
fixDmaOffsets(InstanceData *inst)
{
	uint32 base = (uint32)inst->data;
	uint32 *tag = (uint32*)inst->data;
	for(;;){
		switch(tag[0]&0x70000000){
		// DMAcnt
		case 0x10000000:
			// no need to fix
			tag += (1+(tag[0]&0xFFFF))*4;
			break;

		// DMAref
		case 0x30000000:
			// fix address and jump to next
			tag[1] = base + tag[1]<<4;
			tag += 4;
			break;

		// DMAret
		case 0x60000000:
			// we're done
			return;

		default:
			fprintf(stderr, "error: unknown DMAtag %X\n", tag[0]);
			return;
		}
	}
}
#endif

// Patch DMA ref ADDR fields to qword offsets and return whether
// no ref tags were found.
// Only under RW_PS2 are the addresses actually patched but we need
// the return value for streaming out.
bool32
unfixDmaOffsets(InstanceData *inst)
{
	bool32 norefs = 1;
#ifdef RW_PS2
	uint32 base = (uint32)inst->data;
#endif
	uint32 *tag = (uint32*)inst->data;
	for(;;){
		switch(tag[0]&0x70000000){
		// DMAcnt
		case 0x10000000:
			// no need to unfix
			tag += (1+(tag[0]&0xFFFF))*4;
			break;

		// DMAref
		case 0x30000000:
			norefs = 0;
			// unfix address and jump to next
#ifdef RW_PS2
			tag[1] = (tag[1] - base)>>4;
#endif
			tag += 4;
			break;

		// DMAret
		case 0x60000000:
			return norefs;

		default:
			fprintf(stderr, "error: unknown DMAtag %X\n", tag[0]);
			return norefs;
		}
	}
}

// Pipeline

PipeAttribute attribXYZ = {
	"XYZ",
	AT_V3_32
};

PipeAttribute attribXYZW = {
	"XYZW",
	AT_V4_32
};

PipeAttribute attribUV = {
	"UV",
	AT_V2_32
};

PipeAttribute attribUV2 = {
	"UV2",
	AT_V4_32
};

PipeAttribute attribRGBA = {
	"RGBA",
	AT_V4_8 | AT_UNSGN
};

PipeAttribute attribNormal = {
	"Normal",
	AT_V3_8		// RW has V4_8 but uses V3_8, wtf?
};

PipeAttribute attribWeights = {
	"Weights",
	AT_V4_32 | AT_RW
};

static uint32
attribSize(uint32 unpack)
{
	static uint32 size[] = { 32, 16, 8, 16 };
	return ((unpack>>26 & 3)+1)*size[unpack>>24 & 3]/8;
}

#define QWC(x) (((x)+0xF)>>4)

static uint32
getBatchSize(MatPipeline *pipe, uint32 vertCount)
{
	PipeAttribute *a;
	if(vertCount == 0)
		return 0;
	uint32 size = 1;	// ITOP &c. at the end
	for(uint i = 0; i < nelem(pipe->attribs); i++)
		if((a = pipe->attribs[i]) && (a->attrib & AT_RW) == 0){
			size++;	// UNPACK &c.
			size += QWC(vertCount*attribSize(a->attrib));
		}
	return size;
}

uint32*
instanceXYZ(uint32 *p, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	uint32 *d = (uint32*)g->morphTargets[0].vertices;
	for(uint32 i = idx; i < idx+n; i++){
		j = m->indices[i];
		*p++ = d[j*3+0];
		*p++ = d[j*3+1];
		*p++ = d[j*3+2];
	}
	while((uintptr)p % 0x10)
		*p++ = 0;
	return p;
}

uint32*
instanceXYZW(uint32 *p, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	uint32 *d = (uint32*)g->morphTargets[0].vertices;
	int8 *adcbits = getADCbitsForMesh(g, m);
	for(uint32 i = idx; i < idx+n; i++){
		j = m->indices[i];
		*p++ = d[j*3+0];
		*p++ = d[j*3+1];
		*p++ = d[j*3+2];
		*p++ = adcbits && adcbits[i] ? 0x8000 : 0;
	}
	// don't need to pad
	return p;
}

uint32*
instanceUV(uint32 *p, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	uint32 *d = (uint32*)g->texCoords[0];
	if((g->flags & Geometry::TEXTURED) ||
	   (g->flags & Geometry::TEXTURED2))
		for(uint32 i = idx; i < idx+n; i++){
			j = m->indices[i];
			*p++ = d[j*2+0];
			*p++ = d[j*2+1];
		}
	else
		for(uint32 i = idx; i < idx+n; i++){
			*p++ = 0;
			*p++ = 0;
		}
	while((uintptr)p % 0x10)
		*p++ = 0;
	return p;
}

uint32*
instanceUV2(uint32 *p, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	uint32 *d0 = (uint32*)g->texCoords[0];
	uint32 *d1 = (uint32*)g->texCoords[1];
	for(uint32 i = idx; i < idx+n; i++){
		j = m->indices[i];
		if(g->numTexCoordSets > 0){
			*p++ = d0[j*2+0];
			*p++ = d0[j*2+1];
		}else{
			*p++ = 0;
			*p++ = 0;
		}
		if(g->numTexCoordSets > 1){
			*p++ = d1[j*2+0];
			*p++ = d1[j*2+1];
		}else{
			*p++ = 0;
			*p++ = 0;
		}
	}
	while((uintptr)p % 0x10)
		*p++ = 0;
	return p;
}

uint32*
instanceRGBA(uint32 *p, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	uint32 *d = (uint32*)g->colors;
	if((g->flags & Geometry::PRELIT))
		for(uint32 i = idx; i < idx+n; i++){
			j = m->indices[i];
			*p++ = d[j];
		}
	else
		for(uint32 i = idx; i < idx+n; i++)
			*p++ = 0xFF000000;
	while((uintptr)p % 0x10)
		*p++ = 0;
	return p;
}

uint32*
instanceNormal(uint32 *wp, Geometry *g, Mesh *m, uint32 idx, uint32 n)
{
	uint16 j;
	V3d *d = g->morphTargets[0].normals;
	uint8 *p = (uint8*)wp;
	if((g->flags & Geometry::NORMALS))
		for(uint32 i = idx; i < idx+n; i++){
			j = m->indices[i];
			*p++ = d[j].x*127.0f;
			*p++ = d[j].y*127.0f;
			*p++ = d[j].z*127.0f;
		}
	else
		for(uint32 i = idx; i < idx+n; i++){
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
		}
	while((uintptr)p % 0x10)
		*p++ = 0;
	return (uint32*)p;
}

void
MatPipeline::init(void)
{
	this->rw::Pipeline::init(PLATFORM_PS2);
	for(int i = 0; i < 10; i++)
		this->attribs[i] = nil;
	this->instanceCB = nil;
	this->uninstanceCB = nil;
	this->preUninstCB = nil;
	this->postUninstCB = nil;
}

MatPipeline*
MatPipeline::create(void)
{
	MatPipeline *pipe = rwNewT(MatPipeline, 1, MEMDUR_GLOBAL);
	pipe->init();
	return pipe;
}

void
MatPipeline::destroy(void)
{
	rwFree(this);
}

void
MatPipeline::dump(void)
{
	if(this->platform != PLATFORM_PS2)
		return;
	PipeAttribute *a;
	printf("%x %x\n", this->pluginID, this->pluginData);
	for(uint i = 0; i < nelem(this->attribs); i++){
		a = this->attribs[i];
		if(a)
			printf("%d %s: %x\n", i, a->name, a->attrib);
	}
	printf("stride: %x\n", this->inputStride);
	printf("vertcount: %x\n", this->vifOffset/this->inputStride);
	printf("triSCount: %x\n", this->triStripCount);
	printf("triLCount: %x\n", this->triListCount);
	printf("vifOffset: %x\n", this->vifOffset);
	printf("\n");
}

void
MatPipeline::setTriBufferSizes(uint32 inputStride, uint32 bufferSize)
{
	PipeAttribute *a;

	this->inputStride = inputStride;
	uint32 numTLtris = bufferSize/3;
	this->triListCount = (numTLtris & ~3) * 3;
	this->triStripCount = bufferSize & ~3;
	for(uint i = 0; i < nelem(this->attribs); i++){
		a = this->attribs[i];
		if(a && a->attrib & AT_RW){
			// broken out attribs have different requirement
			// because we have to be able to restart a strip
			// at an aligned offset
			this->triStripCount = ((bufferSize-2) & ~3)+2;
			return;
		}
	}
}

// Instance format:
//  no broken out clusters
//  ======================
//  DMAret [FLUSH; MSKPATH3 || FLUSH; FLUSH] {
//  	foreach batch {
//  		foreach cluster {
//  			MARK/0; STMOD; STCYCL; UNPACK
//  			unpack-data
//  		}
//  		ITOP; MSCALF/MSCNT;                      // if first/not-first
//  		0/FLUSH; 0/MSKPATH3 || 0/FLUSH; 0/FLUSH  // if not-last/last
//  	}
//  }
//
//  broken out clusters
//  ===================
//  foreach batch {
//  	foreach broken out cluster {
//  		DMAref [STCYCL; UNPACK] -> pointer into unpack-data
//  		DMAcnt (empty)
//  	}
//  	DMAcnt/ret {
//  		foreach cluster {
//  			MARK/0; STMOD; STCYCL; UNPACK
//  			unpack-data
//  		}
//  		ITOP; MSCALF/MSCNT;                      // if first/not-first
//  		0/FLUSH; 0/MSKPATH3 || 0/FLUSH; 0/FLUSH  // if not-last/last
//  	}
//  }
//  unpack-data for broken out clusters

uint32 markcnt = 0;

enum {
	DMAcnt       = 0x10000000,
	DMAref       = 0x30000000,
	DMAret       = 0x60000000,

	VIF_NOP      = 0,
	VIF_STCYCL   = 0x01000000,
	VIF_STCYCL1  = 0x01000100,	// WL = 1
	VIF_OFFSET   = 0x02000000,
	VIF_BASE     = 0x03000000,
	VIF_ITOP     = 0x04000000,
	VIF_STMOD    = 0x05000000,
	VIF_MSKPATH3 = 0x06000000,
	VIF_MARK     = 0x07000000,
	VIF_FLUSHE   = 0x10000000,
	VIF_FLUSH    = 0x11000000,
	VIF_FLUSHA   = 0x13000000,
	VIF_MSCAL    = 0x14000000,
	VIF_MSCALF   = 0x15000000,
	VIF_MSCNT    = 0x17000000,
	VIF_STMASK   = 0x20000000,
	VIF_STROW    = 0x30000000,
	VIF_STCOL    = 0x31000000,
	VIF_MPG      = 0x4A000000,
	VIF_DIRECT   = 0x50000000,
	VIF_DIRECTHL = 0x51000000,
	VIF_UNPACK   = 0x60000000	// no mode encoded
};

struct InstMeshInfo
{
	uint32 numAttribs, numBrokenAttribs;
	uint32 batchVertCount, lastBatchVertCount;
	uint32 numBatches;
	uint32 batchSize, lastBatchSize;
	uint32 size;	// size of DMA chain without broken out data
	uint32 size2;	// size of broken out data
	uint32 vertexSize;
	uint32 attribPos[10];
};

InstMeshInfo
getInstMeshInfo(MatPipeline *pipe, Geometry *g, Mesh *m)
{
	PipeAttribute *a;
	InstMeshInfo im;
	im.numAttribs = 0;
	im.numBrokenAttribs = 0;
	im.vertexSize = 0;
	for(uint i = 0; i < nelem(pipe->attribs); i++)
		if((a = pipe->attribs[i])) {
			if(a->attrib & AT_RW)
				im.numBrokenAttribs++;
			else{
				im.vertexSize += attribSize(a->attrib);
				im.numAttribs++;
			}
		}
	if(g->meshHeader->flags == MeshHeader::TRISTRIP){
		im.numBatches = (m->numIndices-2) / (pipe->triStripCount-2);
		im.batchVertCount = pipe->triStripCount;
		im.lastBatchVertCount = (m->numIndices-2) % (pipe->triStripCount-2);
		if(im.lastBatchVertCount){
			im.numBatches++;
			im.lastBatchVertCount += 2;
		}
	}else{	// TRILIST; nothing else supported yet
		im.numBatches = (m->numIndices+pipe->triListCount-1) /
		                 pipe->triListCount;
		im.batchVertCount = pipe->triListCount;
		im.lastBatchVertCount = m->numIndices % pipe->triListCount;
	}
	if(im.lastBatchVertCount == 0)
		im.lastBatchVertCount = im.batchVertCount;

	im.batchSize = getBatchSize(pipe, im.batchVertCount);
	im.lastBatchSize = getBatchSize(pipe, im.lastBatchVertCount);
	if(im.numBrokenAttribs == 0)
		im.size = 1 + im.batchSize*(im.numBatches-1) + im.lastBatchSize;
	else
		im.size = 2*im.numBrokenAttribs*im.numBatches +
		          (1+im.batchSize)*(im.numBatches-1) + 1+im.lastBatchSize;

	/* figure out size and addresses of broken out sections */
	im.size2 = 0;
	for(uint i = 0; i < nelem(im.attribPos); i++)
		if((a = pipe->attribs[i]) && a->attrib & AT_RW){
			im.attribPos[i] = im.size2 + im.size;
			im.size2 += QWC(m->numIndices*attribSize(a->attrib));
		}

	return im;
}

void
MatPipeline::instance(Geometry *g, InstanceData *inst, Mesh *m)
{
	PipeAttribute *a;
	InstMeshInfo im = getInstMeshInfo(this, g, m);

	inst->dataSize = (im.size+im.size2)<<4;
	// TODO: do this properly, just a test right now
	inst->dataSize += 0x7F;
	inst->dataRaw = rwNewT(uint8, inst->dataSize, MEMDUR_EVENT | ID_GEOMETRY);
	inst->data = ALIGNPTR(inst->dataRaw, 0x80);

	/* make array of addresses of broken out sections */
	uint8 *datap[nelem(this->attribs)];
	uint8 **dp = datap;
	for(uint i = 0; i < nelem(this->attribs); i++)
		if((a = this->attribs[i]) && a->attrib & AT_RW)
			dp[i] = inst->data + im.attribPos[i]*0x10;

	// TODO: not sure if this is correct
	uint32 msk_flush = rw::version >= 0x35000 ? VIF_FLUSH : VIF_MSKPATH3;

	uint32 idx = 0;
	uint32 *p = (uint32*)inst->data;
	if(im.numBrokenAttribs == 0){
		*p++ = DMAret | im.size-1;
		*p++ = 0;
		*p++ = VIF_FLUSH;
		*p++ = msk_flush;
	}
	for(uint32 j = 0; j < im.numBatches; j++){
		uint32 nverts, bsize;
		if(j < im.numBatches-1){
			bsize = im.batchSize;
			nverts = im.batchVertCount;
		}else{
			bsize = im.lastBatchSize;
			nverts = im.lastBatchVertCount;
		}
		for(uint i = 0; i < nelem(this->attribs); i++)
			if((a = this->attribs[i]) && a->attrib & AT_RW){
				uint32 atsz = attribSize(a->attrib);
				*p++ = DMAref | QWC(nverts*atsz);
				*p++ = im.attribPos[i];
				*p++ = VIF_STCYCL1 | this->inputStride;
				// Round up nverts so UNPACK will fit exactly into the DMA packet
				//  (can't pad with zeroes in broken out sections).
				int num = (QWC(nverts*atsz)<<4)/atsz;
				*p++ = (a->attrib&0xFF004000)
					| 0x8000 | num << 16 | i; // UNPACK
				// This probably shouldn't happen.
				if(num*this->inputStride > this->vifOffset)
					fprintf(stderr, "WARNING: PS2 instance data over vifOffset %08X, %X-> %X %X\n",
						p[-1], num,
						num*this->inputStride, this->vifOffset);

				*p++ = DMAcnt;
				*p++ = 0x0;
				*p++ = VIF_NOP;
				*p++ = VIF_NOP;

				im.attribPos[i] += g->meshHeader->flags == 1 ?
					QWC((im.batchVertCount-2)*atsz) :
					QWC(im.batchVertCount*atsz);
			}
		if(im.numBrokenAttribs){
			*p++ = (j < im.numBatches-1 ? DMAcnt : DMAret) | bsize;
			*p++ = 0x0;
			*p++ = VIF_NOP;
			*p++ = VIF_NOP;
		}

		for(uint i = 0; i < nelem(this->attribs); i++)
			if((a = this->attribs[i]) && (a->attrib & AT_RW) == 0){
				if(rw::version >= 0x35000)
					*p++ = VIF_NOP;
				else
					*p++ = VIF_MARK | markcnt++;
				*p++ = VIF_STMOD;
				*p++ = VIF_STCYCL1 | this->inputStride;
				*p++ = (a->attrib&0xFF004000)
					| 0x8000 | nverts << 16 | i; // UNPACK

				if(a == &attribXYZ)
					p = instanceXYZ(p, g, m, idx, nverts);
				else if(a == &attribXYZW)
					p = instanceXYZW(p, g, m, idx, nverts);
				else if(a == &attribUV)
					p = instanceUV(p, g, m, idx, nverts);
				else if(a == &attribUV2)
					p = instanceUV2(p, g, m, idx, nverts);
				else if(a == &attribRGBA)
					p = instanceRGBA(p, g, m, idx, nverts);
				else if(a == &attribNormal)
					p = instanceNormal(p, g, m, idx, nverts);
			}
		idx += g->meshHeader->flags == 1
			? im.batchVertCount-2 : im.batchVertCount;

		*p++ = VIF_ITOP | nverts;
		*p++ = j == 0 ? VIF_MSCALF : VIF_MSCNT;
		if(j < im.numBatches-1){
			*p++ = VIF_NOP;
			*p++ = VIF_NOP;
		}else{
			*p++ = VIF_FLUSH;
			*p++ = msk_flush;
		}
	}

	if(this->instanceCB)
		this->instanceCB(this, g, m, datap);
#ifdef RW_PS2
	if(im.numBrokenAttribs)
		fixDmaOffsets(inst);
#endif
}

uint8*
MatPipeline::collectData(Geometry *g, InstanceData *inst, Mesh *m, uint8 *data[])
{
	PipeAttribute *a;
	InstMeshInfo im = getInstMeshInfo(this, g, m);

	uint8 *raw = rwNewT(uint8, im.vertexSize*m->numIndices, MEMDUR_EVENT | ID_GEOMETRY);
	uint8 *dp = raw;
	for(uint i = 0; i < nelem(this->attribs); i++)
		if((a = this->attribs[i])) {
			if(a->attrib & AT_RW){
				data[i] = inst->data + im.attribPos[i]*0x10;
			}else{
				data[i] = dp;
				dp += m->numIndices*attribSize(a->attrib);
			}
		}

	uint8 *datap[nelem(this->attribs)];
	memcpy(datap, data, sizeof(datap));

	uint32 overlap = g->meshHeader->flags == 1 ? 2 : 0;
	uint32 *p = (uint32*)inst->data;
	if(im.numBrokenAttribs == 0)
		p += 4;
	for(uint32 j = 0; j < im.numBatches; j++){
		uint32 nverts = j < im.numBatches-1 ? im.batchVertCount :
		                                      im.lastBatchVertCount;
		for(uint i = 0; i < nelem(this->attribs); i++)
			if((a = this->attribs[i]) && a->attrib & AT_RW)
				p += 8;
		if(im.numBrokenAttribs)
			p += 4;
		for(uint i = 0; i < nelem(this->attribs); i++)
			if((a = this->attribs[i]) && (a->attrib & AT_RW) == 0){
				uint32 asz = attribSize(a->attrib);
				p += 4;
				if((p[-1] & 0xff004000) != a->attrib){
					fprintf(stderr, "unexpected unpack: %08x %08x\n", p[-1], a->attrib);
					assert(0 && "unexpected unpack\n");
				}
				memcpy(datap[i], p, asz*nverts);
				datap[i] += asz*(nverts-overlap);
				p += QWC(asz*nverts)*4;
			}
		p += 4;
	}
	return raw;
}

static void
objInstance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	// TODO: allow for REINSTANCE
	if(geo->instData)
		return;
	InstanceDataHeader *header = rwNewT(InstanceDataHeader, 1, MEMDUR_EVENT | ID_GEOMETRY);
	geo->instData = header;
	header->platform = PLATFORM_PS2;
	assert(geo->meshHeader != nil);
	header->numMeshes = geo->meshHeader->numMeshes;
	header->instanceMeshes = rwNewT(InstanceData, header->numMeshes, MEMDUR_EVENT | ID_GEOMETRY);
	for(uint32 i = 0; i < header->numMeshes; i++){
		Mesh *mesh = &geo->meshHeader->getMeshes()[i];
		InstanceData *instance = &header->instanceMeshes[i];

		MatPipeline *m;
		m = pipe->groupPipeline ?
		    pipe->groupPipeline :
		    (MatPipeline*)mesh->material->pipeline;
		if(m == nil)
			m = defaultMatPipe;
		m->instance(geo, instance, mesh);
		instance->material = mesh->material;
	}
}

/*
static void
printVertCounts(InstanceData *inst, int flag)
{
	uint32 *d = (uint32*)inst->data;
	uint32 id = 0;
	if(inst->material->pipeline)
		id = inst->material->pipeline->pluginData;
	int stride;
	if(inst->arePointersFixed){
		d += 4;
		while(d[3]&0x60000000){	// skip UNPACKs
			stride = d[2]&0xFF;
			d += 4 + 4*QWC(attribSize(d[3])*((d[3]>>16)&0xFF));
		}
		if(d[2] == 0)
			printf("ITOP %x %d (%d) %x\n", *d, stride, flag, id);
	}else{
		while((*d&0x70000000) == 0x30000000){
			stride = d[2]&0xFF;
			printf("UNPACK %x %d (%d) %x\n", d[3], stride, flag, id);
			d += 8;
		}
		if((*d&0x70000000) == 0x10000000){
			d += (*d&0xFFFF)*4;
			printf("ITOP %x %d (%d) %x\n", *d, stride, flag, id);
		}
	}
}
*/

static void
objUninstance(rw::ObjPipeline *rwpipe, Atomic *atomic)
{
	ObjPipeline *pipe = (ObjPipeline*)rwpipe;
	Geometry *geo = atomic->geometry;
	if((geo->flags & Geometry::NATIVE) == 0)
		return;
	assert(geo->instData != nil);
	assert(geo->instData->platform == PLATFORM_PS2);
	InstanceDataHeader *header = (InstanceDataHeader*)geo->instData;
	// highest possible number of vertices
	geo->numVertices = geo->meshHeader->totalIndices;
	geo->numTriangles = geo->meshHeader->guessNumTriangles();
	geo->allocateData();
	geo->allocateMeshes(geo->meshHeader->numMeshes, geo->meshHeader->totalIndices, 0);
	uint32 *flags = rwNewT(uint32, geo->numVertices,
		MEMDUR_FUNCTION | ID_GEOMETRY);
	memset(flags, 0, 4*geo->numVertices);
	memset(geo->meshHeader->getMeshes()->indices, 0, 2*geo->meshHeader->totalIndices);
	for(uint32 i = 0; i < header->numMeshes; i++){
		Mesh *mesh = &geo->meshHeader->getMeshes()[i];
		MatPipeline *m;
		m = pipe->groupPipeline ?
		    pipe->groupPipeline :
		    (MatPipeline*)mesh->material->pipeline;
		if(m == nil) m = defaultMatPipe;
		if(m->preUninstCB) m->preUninstCB(m, geo);
	}
	geo->numVertices = 0;
	for(uint32 i = 0; i < header->numMeshes; i++){
		Mesh *mesh = &geo->meshHeader->getMeshes()[i];
		InstanceData *instance = &header->instanceMeshes[i];
		MatPipeline *m;
		m = pipe->groupPipeline ?
		    pipe->groupPipeline :
		    (MatPipeline*)mesh->material->pipeline;
		if(m == nil) m = defaultMatPipe;

		//printDMAVIF(instance);
		uint8 *data[nelem(m->attribs)] = { nil };
		uint8 *raw = m->collectData(geo, instance, mesh, data);
		assert(m->uninstanceCB);
		m->uninstanceCB(m, geo, flags, mesh, data);
		rwFree(raw);
	}
	for(uint32 i = 0; i < header->numMeshes; i++){
		Mesh *mesh = &geo->meshHeader->getMeshes()[i];
		MatPipeline *m;
		m = pipe->groupPipeline ?
		    pipe->groupPipeline :
		    (MatPipeline*)mesh->material->pipeline;
		if(m == nil) m = defaultMatPipe;
		if(m->postUninstCB) m->postUninstCB(m, geo);
	}

	int8 *bits = getADCbits(geo);
	geo->generateTriangles(bits);
	rwFree(flags);
	geo->flags &= ~Geometry::NATIVE;
	destroyNativeData(geo, 0, 0);
/*
	for(uint32 i = 0; i < header->numMeshes; i++){
		Mesh *mesh = &geo->meshHeader->mesh[i];
		InstanceData *instance = &header->instanceMeshes[i];
//		printf("numIndices: %d\n", mesh->numIndices);
//		printDMA(instance);
		printVertCounts(instance, geo->meshHeader->flags);
	}
*/
}

void
ObjPipeline::init(void)
{
	this->rw::ObjPipeline::init(PLATFORM_PS2);
	this->groupPipeline = nil;
	this->impl.instance = objInstance;
	this->impl.uninstance = objUninstance;
}

ObjPipeline*
ObjPipeline::create(void)
{
	ObjPipeline *pipe = rwNewT(ObjPipeline, 1, MEMDUR_GLOBAL);
	pipe->init();
	return pipe;
}

void
insertVertex(Geometry *geo, int32 i, uint32 mask, Vertex *v)
{
	if(mask & 0x1)
		geo->morphTargets[0].vertices[i] = v->p;
	if(mask & 0x10)
		geo->morphTargets[0].normals[i] = v->n;
	if(mask & 0x100)
		geo->colors[i] = v->c;
	if(mask & 0x1000)
		geo->texCoords[0][i] = v->t;
	if(mask & 0x2000)
		geo->texCoords[1][i] = v->t1;
}

void
genericPreCB(MatPipeline *pipe, Geometry *geo)
{
	PipeAttribute *a;
	for(int32 i = 0; i < (int)nelem(pipe->attribs); i++)
		if((a = pipe->attribs[i]))
			if(a == &attribXYZW){
				allocateADC(geo);
				break;
			}
	skinPreCB(pipe, geo);
}

void
genericUninstanceCB(MatPipeline *pipe, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[])
{
	float32 *xyz = nil, *xyzw = nil;
	float32 *uv = nil, *uv2 = nil;
	uint8 *rgba = nil;
	int8 *normals = nil;
	uint32 *weights = nil;
	int8 *adc = nil;
	Skin *skin = nil;
	if(skinGlobals.geoOffset)
		skin = Skin::get(geo);

	PipeAttribute *a;
	for(int32 i = 0; i < (int)nelem(pipe->attribs); i++)
		if((a = pipe->attribs[i])){
			if(a == &attribXYZ) xyz = (float32*)data[i];
			else if(a == &attribXYZW) xyzw = (float32*)data[i];
			else if(a == &attribUV) uv = (float32*)data[i];
			else if(a == &attribUV2) uv2 = (float32*)data[i];
			else if(a == &attribRGBA) rgba = data[i];
			else if(a == &attribNormal) normals = (int8*)data[i];
			else if(a == &attribWeights) weights = (uint32*)data[i];
		}

	uint32 mask = 0x1;	// vertices
	if(normals && geo->flags & Geometry::NORMALS)
		mask |= 0x10;
	if(rgba && geo->flags & Geometry::PRELIT)
		mask |= 0x100;
	if((uv || uv2) && geo->numTexCoordSets > 0)
		mask |= 0x1000;
	if(uv2 && geo->numTexCoordSets > 1)
		mask |= 0x2000;
	if(weights && skin)
		mask |= 0x10000;
	if(xyzw)
		adc = getADCbitsForMesh(geo, mesh);

	Vertex v;
	for(uint32 i = 0; i < mesh->numIndices; i++){
		if(mask & 0x1)
			memcpy(&v.p, xyz ? xyz : xyzw, 12);
		if(mask & 0x10){
			// TODO: figure out scaling :/
			v.n.x = normals[0]/128.0f;
			v.n.y = normals[1]/128.0f;
			v.n.z = normals[2]/128.0f;
		}
		if(mask & 0x100)
			memcpy(&v.c, rgba, 4);
		if(mask & 0x1000)
			memcpy(&v.t, uv ? uv : uv2, 8);
		if(mask & 0x2000)
			memcpy(&v.t1, uv2 + 2, 8);
		if(mask & 0x10000)
			for(int j = 0; j < 4; j++){
				((uint32*)v.w)[j] = weights[j] & ~0x3FF;
				v.i[j] = (weights[j] & 0x3FF) >> 2;
				if(v.i[j]) v.i[j]--;
				if(v.w[j] == 0.0f) v.i[j] = 0;
			}
		int32 idx = findVertexSkin(geo, flags, mask, &v);
		if(idx < 0)
			idx = geo->numVertices++;
		mesh->indices[i] = idx;
		if(adc)
			adc[i] = xyzw[3] != 0.0f;
		flags[idx] = mask;
		insertVertexSkin(geo, idx, mask, &v);
		if(xyz) xyz += 3;
		if(xyzw) xyzw += 4;
		if(uv) uv += 2;
		if(uv2) uv2 += 4;
		rgba += 4;
		normals += 3;
		weights += 4;
	}
}

/*
void
defaultUninstanceCB(MatPipeline *pipe, Geometry *geo, uint32 flags[], Mesh *mesh, uint8 *data[])
{
	float32 *verts     = (float32*)data[AT_XYZ];
	float32 *texcoords = (float32*)data[AT_UV];
	uint8 *colors      = (uint8*)data[AT_RGBA];
	int8 *norms        = (int8*)data[AT_NORMAL];
	uint32 mask = 0x1;	// vertices
	if(geo->flags & Geometry::NORMALS)
		mask |= 0x10;
	if(geo->flags & Geometry::PRELIT)
		mask |= 0x100;
	for(int32 i = 0; i < geo->numTexCoordSets; i++)
		mask |= 0x1000 << i;
	int numUV = pipe->attribs[AT_UV] == &attribUV2 ? 2 : 1;

	Vertex v;
	for(uint32 i = 0; i < mesh->numIndices; i++){
		if(mask & 0x1)
			memcpy(&v.p, verts, 12);
		if(mask & 0x10){
			v.n[0] = norms[0]/127.0f;
			v.n[1] = norms[1]/127.0f;
			v.n[2] = norms[2]/127.0f;
		}
		if(mask & 0x100){
			memcpy(&v.c, colors, 4);
			//v.c[3] = 0xFF;
		}
		if(mask & 0x1000)
			memcpy(&v.t, texcoords, 8);
		if(mask & 0x2000)
			memcpy(&v.t1, texcoords+2, 8);

		int32 idx = findVertex(geo, flags, mask, &v);
		if(idx < 0)
			idx = geo->numVertices++;
		mesh->indices[i] = idx;
		flags[idx] = mask;
		insertVertex(geo, idx, mask, &v);
		verts += 3;
		texcoords += 2*numUV;
		colors += 4;
		norms += 3;
	}
}
*/

#undef QWC

ObjPipeline*
makeDefaultPipeline(void)
{
	if(defaultMatPipe == nil){
		MatPipeline *pipe = MatPipeline::create();
		pipe->attribs[AT_XYZ] = &attribXYZ;
		pipe->attribs[AT_UV] = &attribUV;
		pipe->attribs[AT_RGBA] = &attribRGBA;
		pipe->attribs[AT_NORMAL] = &attribNormal;
		uint32 vertCount = MatPipeline::getVertCount(VU_Lights,4,3,2);
		pipe->setTriBufferSizes(4, vertCount);
		pipe->vifOffset = pipe->inputStride*vertCount;
		pipe->uninstanceCB = genericUninstanceCB;
		defaultMatPipe = pipe;
	}

	if(defaultObjPipe == nil){
		ObjPipeline *opipe = ObjPipeline::create();
		defaultObjPipe = opipe;
	}
	return defaultObjPipe;
}

// ADC

int32 adcOffset;

int8*
getADCbits(Geometry *geo)
{
	int8 *bits = nil;
	if(adcOffset){
		ADCData *adc = PLUGINOFFSET(ADCData, geo, adcOffset);
		if(adc->adcFormatted)
			bits = adc->adcBits;
	}
	return bits;
}

int8*
getADCbitsForMesh(Geometry *geo, Mesh *mesh)
{
	int8 *bits = getADCbits(geo);
	if(bits == nil)
		return nil;
	int32 n = mesh - geo->meshHeader->getMeshes();
	for(int32 i = 0; i < n; i++)
		bits += geo->meshHeader->getMeshes()[i].numIndices;
	return bits;
}

// TODO
void
convertADC(Geometry*)
{
}

// Not optimal but works
void
unconvertADC(Geometry *g)
{
	ADCData *adc = PLUGINOFFSET(ADCData, g, adcOffset);
	if(!adc->adcFormatted)
		return;
	int8 *b = adc->adcBits;

	MeshHeader *oldmh = g->meshHeader;
	g->meshHeader = nil;
	// Don't allocate indices for now
	MeshHeader *newmh = g->allocateMeshes(oldmh->numMeshes, 0, 1);
	newmh->flags = oldmh->flags;	// should be tristrip
	Mesh *oldm = oldmh->getMeshes();
	Mesh *newm = newmh->getMeshes();
	for(int32 i = 0; i < newmh->numMeshes; i++){
		newm->material = oldm->material;
		newm->numIndices = oldm->numIndices;
		for(uint32 j = 0; j < oldm->numIndices; j++)
			if(*b++)
				newm->numIndices += 2;
		newmh->totalIndices += newm->numIndices;
		newm++;
		oldm++;
	}
	// Now re-allocate with indices
	newmh = g->allocateMeshes(newmh->numMeshes, newmh->totalIndices, 0);
	b = adc->adcBits;
	oldm = oldmh->getMeshes();
	newm = newmh->getMeshes();
	for(int32 i = 0; i < newmh->numMeshes; i++){
		int32 n = 0;
		for(uint32 j = 0; j < oldm->numIndices; j++){
			if(*b++){
				newm->indices[n++] = oldm->indices[j-1];
				newm->indices[n++] = oldm->indices[j-1];
			}
			newm->indices[n++] = oldm->indices[j];
		}
		newm++;
		oldm++;
	}
	rwFree(oldmh);
	adc->adcFormatted = 0;
	rwFree(adc->adcBits);
	adc->adcBits = nil;
	adc->numBits = 0;
}

void
allocateADC(Geometry *geo)
{
	ADCData *adc = PLUGINOFFSET(ADCData, geo, adcOffset);
	adc->adcFormatted = 1;
	adc->numBits = geo->meshHeader->totalIndices;
	int32 size = adc->numBits+3 & ~3;
	adc->adcBits = rwNewT(int8, size, MEMDUR_EVENT | ID_ADC);
	memset(adc->adcBits, 0, size);
}

static void*
createADC(void *object, int32 offset, int32)
{
	ADCData *adc = PLUGINOFFSET(ADCData, object, offset);
	adc->adcFormatted = 0;
	return object;
}

static void*
copyADC(void *dst, void *src, int32 offset, int32)
{
	ADCData *dstadc = PLUGINOFFSET(ADCData, dst, offset);
	ADCData *srcadc = PLUGINOFFSET(ADCData, src, offset);
	dstadc->adcFormatted = srcadc->adcFormatted;
	if(!dstadc->adcFormatted)
		return dst;
	dstadc->numBits = srcadc->numBits;
	int32 size = dstadc->numBits+3 & ~3;
	dstadc->adcBits = rwNewT(int8, size, MEMDUR_EVENT | ID_ADC);
	memcpy(dstadc->adcBits, srcadc->adcBits, size);
	return dst;
}

static void*
destroyADC(void *object, int32 offset, int32)
{
	ADCData *adc = PLUGINOFFSET(ADCData, object, offset);
	if(adc->adcFormatted)
		rwFree(adc->adcBits);
	return object;
}

static Stream*
readADC(Stream *stream, int32, void *object, int32 offset, int32)
{
	ADCData *adc = PLUGINOFFSET(ADCData, object, offset);
	if(!findChunk(stream, ID_ADC, nil, nil)){
		RWERROR((ERR_CHUNK, "ADC"));
		return nil;
	}
	adc->numBits = stream->readI32();
	adc->adcFormatted = 1;
	if(adc->numBits == 0){
		adc->adcBits = nil;
		adc->numBits = 0;
		return stream;
	}
	int32 size = adc->numBits+3 & ~3;
	adc->adcBits = rwNewT(int8, size, MEMDUR_EVENT | ID_ADC);
	stream->read8(adc->adcBits, size);
	return stream;
}

static Stream*
writeADC(Stream *stream, int32 len, void *object, int32 offset, int32)
{
	ADCData *adc = PLUGINOFFSET(ADCData, object, offset);
	Geometry *geometry = (Geometry*)object;
	writeChunkHeader(stream, ID_ADC, len-12);
	if(geometry->flags & Geometry::NATIVE){
		stream->writeI32(0);
		return stream;
	}
	stream->writeI32(adc->numBits);
	int32 size = adc->numBits+3 & ~3;
	stream->write8(adc->adcBits, size);
	return stream;
}

static int32
getSizeADC(void *object, int32 offset, int32)
{
	Geometry *geometry = (Geometry*)object;
	ADCData *adc = PLUGINOFFSET(ADCData, object, offset);
	if(!adc->adcFormatted)
		return 0;
	if(geometry->flags & Geometry::NATIVE)
		return 16;
	return 16 + (adc->numBits+3 & ~3);
}

void
registerADCPlugin(void)
{
	adcOffset = Geometry::registerPlugin(sizeof(ADCData), ID_ADC,
	                                     createADC, destroyADC, copyADC);
	Geometry::registerPluginStream(ID_ADC,
	                               readADC,
	                               writeADC,
	                               getSizeADC);
}

// misc stuff

static uint32
unpackSize(uint32 unpack)
{
	static uint32 size[] = { 32, 16, 8, 4 };
	return ((unpack>>26 & 3)+1)*size[unpack>>24 & 3]/8;
}

/* A little dumb VIF interpreter */
static void
sendVIF(uint32 w)
{
	enum VIFstate {
		VST_cmd,
		VST_stmask,
		VST_strow,
		VST_stcol,
		VST_mpg,
		VST_direct,
		VST_unpack
	};
//	static uint32 buf[256 * 16];	// maximum unpack size
	static VIFstate state = VST_cmd;
	static uint32 n;
	static uint32 code;
	uint32 imm, num;

	imm = w & 0xFFFF;
	num = (w>>16) & 0xFF;
	switch(state){
	case VST_cmd:
		code = w;
		if((code & 0x60000000) == VIF_UNPACK){
			printf("\t%08X VIF_UNPACK\n", code);
			printf("\t...skipping...\n");
			state = VST_unpack;
			n = (unpackSize(code)*num + 3) >> 2;
		}else switch(code & 0x7F000000){
		case VIF_NOP:
			printf("\t%08X VIF_NOP\n", code);
			break;
		case VIF_STCYCL:
			printf("\t%08X VIF_STCYCL\n", code);
			break;
		case VIF_OFFSET:
			printf("\t%08X VIF_OFFSET\n", code);
			break;
		case VIF_BASE:
			printf("\t%08X VIF_BASE\n", code);
			break;
		case VIF_ITOP:
			printf("\t%08X VIF_ITOP\n", code);
			break;
		case VIF_STMOD:
			printf("\t%08X VIF_STMOD\n", code);
			break;
		case VIF_MSKPATH3:
			printf("\t%08X VIF_MSKPATH3\n", code);
			break;
		case VIF_MARK:
			printf("\t%08X VIF_MARK\n", code);
			break;
		case VIF_FLUSHE:
			printf("\t%08X VIF_FLUSHE\n", code);
			break;
		case VIF_FLUSH:
			printf("\t%08X VIF_FLUSH\n", code);
			break;
		case VIF_FLUSHA:
			printf("\t%08X VIF_FLUSHA\n", code);
			break;
		case VIF_MSCAL:
			printf("\t%08X VIF_MSCAL\n", code);
			break;
		case VIF_MSCALF:
			printf("\t%08X VIF_MSCALF\n", code);
			break;
		case VIF_MSCNT:
			printf("\t%08X VIF_MSCNT\n", code);
			break;
		case VIF_STMASK:
			printf("\t%08X VIF_STMASK\n", code);
			printf("\t...skipping...\n");
			state = VST_stmask;
			n = 1;
			break;
		case VIF_STROW:
			printf("\t%08X VIF_STROW\n", code);
			printf("\t...skipping...\n");
			state = VST_strow;
			n = 4;
			break;
		case VIF_STCOL:
			printf("\t%08X VIF_STCOL\n", code);
			printf("\t...skipping...\n");
			state = VST_stcol;
			n = 4;
			break;
		case VIF_MPG:
			printf("\t%08X VIF_MPG\n", code);
			state = VST_mpg;
			n = num*2;
			break;
		case VIF_DIRECT:
			printf("\t%08X VIF_DIRECT\n", code);
			printf("\t...skipping...\n");
			state = VST_direct;
			n = imm*4;
			break;
		case VIF_DIRECTHL:
			printf("\t%08X VIF_DIRECTHL\n", code);
			printf("\t...skipping...\n");
			state = VST_direct;
			n = imm*4;
			break;
		default:
			printf("\tUnknown VIFcode %08X\n", code);
		}
		break;
	/* TODO: actually do something here */
	case VST_stmask:
		n--;
		break;
	case VST_strow:
		n--;
		break;
	case VST_stcol:
		n--;
		break;
	case VST_mpg:
		n--;
		break;
	case VST_direct:
		n--;
		break;
	case VST_unpack:
		n--;
		break;
	}
	if(n == 0)
		state = VST_cmd;
}

static void
dmaVIF(int32 qwc, uint32 *data)
{
	qwc *= 4;
	while(qwc--)
		sendVIF(*data++);
}

void
printDMAVIF(InstanceData *inst)
{
	uint32 *tag = (uint32*)inst->data;
	uint32 *base = (uint32*)inst->data;
	uint32 qwc;

	for(;;){
		qwc = tag[0]&0xFFFF;
		switch(tag[0]&0x70000000){
		case DMAcnt:
			printf("DMAcnt %04x %08x\n", qwc, tag[1]);
			sendVIF(tag[2]);
			sendVIF(tag[3]);
			dmaVIF(qwc, tag+4);
			tag += (1+qwc)*4;
			break;

		case DMAref:
			printf("DMAref %04x %08x\n", qwc, tag[1]);
			sendVIF(tag[2]);
			sendVIF(tag[3]);
			dmaVIF(qwc, base + tag[1]*4);
			tag += 4;
			break;

		case DMAret:
			printf("DMAret %04x %08x\n", qwc, tag[1]);
			sendVIF(tag[2]);
			sendVIF(tag[3]);
			dmaVIF(qwc, tag+4);
			printf("\n");
			return;
		}
	}
}

void
printDMA(InstanceData *inst)
{
	uint32 *tag = (uint32*)inst->data;
	uint32 qwc;
	for(;;){
		qwc = tag[0]&0xFFFF;
		switch(tag[0]&0x70000000){
		case DMAcnt:
			printf("CNT %04x %08x\n", qwc, tag[1]);
			tag += (1+qwc)*4;
			break;

		case DMAref:
			printf("REF %04x %08x\n", qwc, tag[1]);
			tag += 4;
			break;

		case DMAret:
			printf("RET %04x %08x\n\n", qwc, tag[1]);
			return;
		}
	}
}

/*
void
sizedebug(InstanceData *inst)
{
	if(inst->arePointersFixed == 2)
		return;
	uint32 *base = (uint32*)inst->data;
	uint32 *tag = (uint32*)inst->data;
	uint32 *last = nil;
	for(;;){
		switch(tag[0]&0x70000000){
		case DMAcnt:
			tag += (1+(tag[0]&0xFFFF))*4;
			break;

		case DMAref:
			last = base + tag[1]*4 + (tag[0]&0xFFFF)*4;
			tag += 4;
			break;

		case DMAret:
			tag += (1+(tag[0]&0xFFFF))*4;
			uint32 diff;
			if(!last)
				diff = (uint8*)tag - (uint8*)base;
			else
				diff = (uint8*)last - (uint8*)base;
			printf("%x %x %x\n", inst->dataSize-diff, diff, inst->dataSize);
			return;

		default:
			printf("unkown DMAtag: %X %X\n", tag[0], tag[1]);
			break;
		}
	}
}
*/

}
}

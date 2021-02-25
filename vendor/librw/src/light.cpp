#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

#define PLUGIN_ID ID_LIGHT

namespace rw {

int32 Light::numAllocated;

PluginList Light::s_plglist(sizeof(Light));

static void
lightSync(ObjectWithFrame*)
{
}

static void
worldLightSync(ObjectWithFrame *obj)
{
	Light *light = (Light*)obj;
	light->originalSync(obj);
}

Light*
Light::create(int32 type)
{
	Light *light = (Light*)rwMalloc(s_plglist.size, MEMDUR_EVENT | ID_LIGHT);
	if(light == nil){
		RWERROR((ERR_ALLOC, s_plglist.size));
		return nil;
	}
	numAllocated++;
	light->object.object.init(Light::ID, type);
	light->object.syncCB = lightSync;
	light->radius = 0.0f;
	light->color.red = 1.0f;
	light->color.green = 1.0f;
	light->color.blue = 1.0f;
	light->color.alpha = 1.0f;
	light->minusCosAngle = 1.0f;
	light->object.object.privateFlags = 1;
	light->object.object.flags = LIGHTATOMICS | LIGHTWORLD;
	light->inWorld.init();

	// clump extension
	light->clump = nil;
	light->inClump.init();

	// world extension
	light->world = nil;
	light->originalSync = light->object.syncCB;
	light->object.syncCB = worldLightSync;

	s_plglist.construct(light);
	return light;
}

void
Light::destroy(void)
{
	s_plglist.destruct(this);
	assert(this->clump == nil);
	assert(this->world == nil);
	this->setFrame(nil);
	rwFree(this);
	numAllocated--;
}

void
Light::setAngle(float32 angle)
{
	this->minusCosAngle = -cosf(angle);
}

float32
Light::getAngle(void)
{
	return acosf(-this->minusCosAngle);
}

void
Light::setColor(float32 r, float32 g, float32 b)
{
	this->color.red = r;
	this->color.green = g;
	this->color.blue = b;
	this->object.object.privateFlags = r == g && r == b;
}

struct LightChunkData
{
	float32 radius;
	float32 red, green, blue;
	float32 minusCosAngle;
	uint32 type_flags;
};

Light*
Light::streamRead(Stream *stream)
{
	uint32 version;
	LightChunkData buf;

	if(!findChunk(stream, ID_STRUCT, nil, &version)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	stream->read32(&buf, sizeof(LightChunkData));
	Light *light = Light::create(buf.type_flags>>16);
	if(light == nil)
		return nil;
	light->radius = buf.radius;
	light->setColor(buf.red, buf.green, buf.blue);
	float32 a = buf.minusCosAngle;
	if(version >= 0x30300)
		light->minusCosAngle = a;
	else
		// tan -> -cos
		light->minusCosAngle = -1.0f/sqrtf(a*a+1.0f);
	light->object.object.flags = (uint8)buf.type_flags;
	if(s_plglist.streamRead(stream, light))
		return light;
	light->destroy();
	return nil;
}

bool
Light::streamWrite(Stream *stream)
{
	LightChunkData buf;
	writeChunkHeader(stream, ID_LIGHT, this->streamGetSize());
	writeChunkHeader(stream, ID_STRUCT, sizeof(LightChunkData));
	buf.radius = this->radius;
	buf.red   = this->color.red;
	buf.green = this->color.green;
	buf.blue  = this->color.blue;
	if(version >= 0x30300)
		buf.minusCosAngle = this->minusCosAngle;
	else
		buf.minusCosAngle = tanf(acosf(-this->minusCosAngle));
	buf.type_flags = (uint32)this->object.object.flags |
		(uint32)this->object.object.subType << 16;
	stream->write32(&buf, sizeof(LightChunkData));

	s_plglist.streamWrite(stream, this);
	return true;
}

uint32
Light::streamGetSize(void)
{
	return 12 + sizeof(LightChunkData) + 12 + s_plglist.streamGetSize(this);
}

}

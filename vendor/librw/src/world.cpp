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

#define PLUGIN_ID ID_WORLD

namespace rw {

int32 World::numAllocated = 0;

PluginList World::s_plglist(sizeof(World));

World*
World::create(void)
{
	World *world = (World*)rwMalloc(s_plglist.size, MEMDUR_EVENT | ID_WORLD);
	if(world == nil){
		RWERROR((ERR_ALLOC, s_plglist.size));
		return nil;
	}
	numAllocated++;
	world->object.init(World::ID, 0);
	world->localLights.init();
	world->globalLights.init();
	world->clumps.init();
	s_plglist.construct(world);
	return world;
}

void
World::destroy(void)
{
	s_plglist.destruct(this);
	rwFree(this);
	numAllocated--;
}

void
World::addLight(Light *light)
{
	assert(light->world == nil);
	light->world = this;
	if(light->getType() < Light::POINT){
		this->globalLights.append(&light->inWorld);
	}else{
		this->localLights.append(&light->inWorld);
		if(light->getFrame())
			light->getFrame()->updateObjects();
	}
}

void
World::removeLight(Light *light)
{
	assert(light->world == this);
	light->inWorld.remove();
	light->world = nil;
}

void
World::addCamera(Camera *cam)
{
	assert(cam->world == nil);
	cam->world = this;
	if(cam->getFrame())
		cam->getFrame()->updateObjects();
}

void
World::removeCamera(Camera *cam)
{
	assert(cam->world == this);
	cam->world = nil;
}

void
World::addAtomic(Atomic *atomic)
{
	assert(atomic->world == nil);
	atomic->world = this;
	if(atomic->getFrame())
		atomic->getFrame()->updateObjects();
}

void
World::removeAtomic(Atomic *atomic)
{
	assert(atomic->world == this);
	atomic->world = nil;
}

void
World::addClump(Clump *clump)
{
	assert(clump->world == nil);
	clump->world = this;
	this->clumps.add(&clump->inWorld);
	FORLIST(lnk, clump->atomics)
		this->addAtomic(Atomic::fromClump(lnk));
	FORLIST(lnk, clump->lights)
		this->addLight(Light::fromClump(lnk));
	FORLIST(lnk, clump->cameras)
		this->addCamera(Camera::fromClump(lnk));

	if(clump->getFrame()){
		clump->getFrame()->matrix.optimize();
		clump->getFrame()->updateObjects();
	}
}

void
World::removeClump(Clump *clump)
{
	assert(clump->world == this);
	clump->inWorld.remove();
	FORLIST(lnk, clump->atomics)
		this->removeAtomic(Atomic::fromClump(lnk));
	FORLIST(lnk, clump->lights)
		this->removeLight(Light::fromClump(lnk));
	FORLIST(lnk, clump->cameras)
		this->removeCamera(Camera::fromClump(lnk));
	clump->world = nil;
}

void
World::render(void)
{
	// this is very wrong, we really want world sectors
	FORLIST(lnk, this->clumps)
		Clump::fromWorld(lnk)->render();
}

// Find lights that illuminate an atomic
void
World::enumerateLights(Atomic *atomic, WorldLights *lightData)
{
	int32 maxDirectionals, maxLocals;

	maxDirectionals = lightData->numDirectionals;
	maxLocals = lightData->numLocals;

	lightData->numDirectionals = 0;
	lightData->numLocals = 0;
	lightData->numAmbients = 0;
	lightData->ambient.red = 0.0f;
	lightData->ambient.green = 0.0f;
	lightData->ambient.blue = 0.0f;
	lightData->ambient.alpha = 1.0f;

	bool32 normals = atomic->geometry->flags & Geometry::NORMALS;

	FORLIST(lnk, this->globalLights){
		Light *l = Light::fromWorld(lnk);
		if((l->getFlags() & Light::LIGHTATOMICS) == 0)
			continue;
		if(l->getType() == Light::AMBIENT){
			lightData->ambient.red   += l->color.red;
			lightData->ambient.green += l->color.green;
			lightData->ambient.blue  += l->color.blue;
			lightData->numAmbients++;
		}else if(normals && l->getType() == Light::DIRECTIONAL){
			if(lightData->numDirectionals < maxDirectionals)
				lightData->directionals[lightData->numDirectionals++] = l;
		}
	}

	if(atomic->world != this)
		return;

	if(!normals)
		return;

	// TODO: for this we would use an atomic's world sectors, but we don't have those yet
	FORLIST(lnk, this->localLights){
		if(lightData->numLocals >= maxLocals)
			return;

		Light *l = Light::fromWorld(lnk);
		if((l->getFlags() & Light::LIGHTATOMICS) == 0)
			continue;

		// check if spheres are intersecting
		Sphere *atomsphere = atomic->getWorldBoundingSphere();
		V3d dist = sub(l->getFrame()->getLTM()->pos, atomsphere->center);
		if(length(dist) < atomsphere->radius + l->radius)
			lightData->locals[lightData->numLocals++] = l;
	}
}

}

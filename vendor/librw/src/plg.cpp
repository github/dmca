#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

namespace rw {

static void *defCtor(void *object, int32, int32) { return object; }
static void *defDtor(void *object, int32, int32) { return object; }
static void *defCopy(void *dst, void*, int32, int32) { return dst; }

static LinkList allPlugins;

#define PLG(lnk) LLLinkGetData(lnk, Plugin, inParentList)

void
PluginList::open(void)
{
	allPlugins.init();
}

void
PluginList::close(void)
{
	PluginList *l;
	Plugin *p;
	FORLIST(lnk, allPlugins){
		p = LLLinkGetData(lnk, Plugin, inGlobalList);
		l = p->parentList;
		p->inParentList.remove();
		p->inGlobalList.remove();
		rwFree(p);
		if(l->plugins.isEmpty())
			l->size = l->defaultSize;
	}
	assert(allPlugins.isEmpty());
}

void
PluginList::construct(void *object)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		p->constructor(object, p->offset, p->size);
	}
}

void
PluginList::destruct(void *object)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		p->destructor(object, p->offset, p->size);
	}
}

void
PluginList::copy(void *dst, void *src)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		p->copy(dst, src, p->offset, p->size);
	}
}

bool
PluginList::streamRead(Stream *stream, void *object)
{
	int32 length;
	ChunkHeaderInfo header;
	if(!findChunk(stream, ID_EXTENSION, (uint32*)&length, nil))
		return false;
	while(length > 0){
		if(!readChunkHeaderInfo(stream, &header))
			return false;
		length -= 12;
		FORLIST(lnk, this->plugins){
			Plugin *p = PLG(lnk);
			if(p->id == header.type && p->read){
				p->read(stream, header.length,
				        object, p->offset, p->size);
				goto cont;
			}
		}
		stream->seek(header.length);
cont:
		length -= header.length;
	}

	// now the always callbacks
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->alwaysCallback)
			p->alwaysCallback(object, p->offset, p->size);
	}
	return true;
}

void
PluginList::streamWrite(Stream *stream, void *object)
{
	int size = this->streamGetSize(object);
	writeChunkHeader(stream, ID_EXTENSION, size);
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->getSize == nil ||
		   (size = p->getSize(object, p->offset, p->size)) <= 0)
			continue;
		writeChunkHeader(stream, p->id, size);
		p->write(stream, size, object, p->offset, p->size);
	}
}

int
PluginList::streamGetSize(void *object)
{
	int32 size = 0;
	int32 plgsize;
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->getSize &&
		   (plgsize = p->getSize(object, p->offset, p->size)) > 0)
			size += 12 + plgsize;
	}
	return size;
}

void
PluginList::streamSkip(Stream *stream)
{
	int32 length;
	ChunkHeaderInfo header;
	if(!findChunk(stream, ID_EXTENSION, (uint32*)&length, nil))
		return;
	while(length > 0){
		if(!readChunkHeaderInfo(stream, &header))
			return;
		stream->seek(header.length);
		length -= 12 + header.length;
	}
}

void
PluginList::assertRights(void *object, uint32 pluginID, uint32 data)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->id == pluginID){
			if(p->rightsCallback)
				p->rightsCallback(object,
				                  p->offset, p->size, data);
			return;
		}
	}
}


int32
PluginList::registerPlugin(int32 size, uint32 id,
	Constructor ctor, Destructor dtor, CopyConstructor copy)
{
	Plugin *p = (Plugin*)rwMalloc(sizeof(Plugin), MEMDUR_GLOBAL);
	p->offset = this->size;
	this->size += size;
	int32 round = sizeof(void*)-1;
	this->size = (this->size + round)&~round;

	p->size = size;
	p->id = id;
	p->constructor = ctor ? ctor : defCtor;
	p->destructor = dtor ? dtor : defDtor;
	p->copy = copy ? copy : defCopy;
	p->read = nil;
	p->write = nil;
	p->getSize = nil;
	p->rightsCallback = nil;
	p->alwaysCallback = nil;
	p->parentList = this;
	this->plugins.add(&p->inParentList);
	allPlugins.add(&p->inGlobalList);
	return p->offset;
}

int32
PluginList::registerStream(uint32 id,
	StreamRead read, StreamWrite write, StreamGetSize getSize)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->id == id){
			p->read = read;
			p->write = write;
			p->getSize = getSize;
			return p->offset;
		}
	}
	return -1;
}

int32
PluginList::setStreamRightsCallback(uint32 id, RightsCallback cb)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->id == id){
			p->rightsCallback = cb;
			return p->offset;
		}
	}
	return -1;
}

int32
PluginList::setStreamAlwaysCallback(uint32 id, AlwaysCallback cb)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->id == id){
			p->alwaysCallback = cb;
			return p->offset;
		}
	}
	return -1;
}

int32
PluginList::getPluginOffset(uint32 id)
{
	FORLIST(lnk, this->plugins){
		Plugin *p = PLG(lnk);
		if(p->id == id)
			return p->offset;
	}
	return -1;
}

}

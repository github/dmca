#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WITH_D3D
#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"
#include "ps2/rwps2.h"
#include "d3d/rwd3d.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"
#include "d3d/rwd3dimpl.h"
#include "gl/rwgl3.h"

#define PLUGIN_ID 0

namespace rw {

int32 Texture::numAllocated;
int32 TexDictionary::numAllocated;

PluginList TexDictionary::s_plglist(sizeof(TexDictionary));
PluginList Texture::s_plglist(sizeof(Texture));
PluginList Raster::s_plglist(sizeof(Raster));

struct TextureGlobals
{
	TexDictionary *initialTexDict;
	TexDictionary *currentTexDict;
	// load textures from files
	bool32 loadTextures;
	// create dummy textures to store just names
	bool32 makeDummies;
	bool32 mipmapping;
	bool32 autoMipmapping;
	LinkList texDicts;

	LinkList textures;
};
int32 textureModuleOffset;

#define TEXTUREGLOBAL(v) (PLUGINOFFSET(TextureGlobals, engine, textureModuleOffset)->v)

static void*
textureOpen(void *object, int32 offset, int32 size)
{
	TexDictionary *texdict;
	textureModuleOffset = offset;
	TEXTUREGLOBAL(texDicts).init();
	TEXTUREGLOBAL(textures).init();
	texdict = TexDictionary::create();
	TEXTUREGLOBAL(initialTexDict) = texdict;
	TexDictionary::setCurrent(texdict);
	TEXTUREGLOBAL(loadTextures) = 1;
	TEXTUREGLOBAL(makeDummies) = 0;
	TEXTUREGLOBAL(mipmapping) = 0;
	TEXTUREGLOBAL(autoMipmapping) = 0;
	return object;
}
static void*
textureClose(void *object, int32 offset, int32 size)
{
	FORLIST(lnk, TEXTUREGLOBAL(texDicts))
		TexDictionary::fromLink(lnk)->destroy();
	TEXTUREGLOBAL(initialTexDict) = nil;
	TEXTUREGLOBAL(currentTexDict) = nil;

	FORLIST(lnk, TEXTUREGLOBAL(textures)){
		Texture *tex = LLLinkGetData(lnk, Texture, inGlobalList);
		printf("Tex still allocated: %d %s %s\n", tex->refCount, tex->name, tex->mask);
		assert(tex->dict == nil);
		tex->destroy();
	}
	return object;
}

void
Texture::registerModule(void)
{
	Engine::registerPlugin(sizeof(TextureGlobals), ID_TEXTUREMODULE, textureOpen, textureClose);
}

void
Texture::setLoadTextures(bool32 b)
{
	TEXTUREGLOBAL(loadTextures) = b;
}

void
Texture::setCreateDummies(bool32 b)
{
	TEXTUREGLOBAL(makeDummies) = b;
}

void Texture::setMipmapping(bool32 b) { TEXTUREGLOBAL(mipmapping) = b; }
void Texture::setAutoMipmapping(bool32 b) { TEXTUREGLOBAL(autoMipmapping) = b; }
bool32 Texture::getMipmapping(void) { return TEXTUREGLOBAL(mipmapping); }
bool32 Texture::getAutoMipmapping(void) { return TEXTUREGLOBAL(autoMipmapping); }

//
// TexDictionary
//

TexDictionary*
TexDictionary::create(void)
{
	TexDictionary *dict = (TexDictionary*)rwMalloc(s_plglist.size, MEMDUR_EVENT | ID_TEXDICTIONARY);
	if(dict == nil){
		RWERROR((ERR_ALLOC, s_plglist.size));
		return nil;
	}
	numAllocated++;
	dict->object.init(TexDictionary::ID, 0);
	dict->textures.init();
	TEXTUREGLOBAL(texDicts).add(&dict->inGlobalList);
	s_plglist.construct(dict);
	return dict;
}

void
TexDictionary::destroy(void)
{
	if(TEXTUREGLOBAL(currentTexDict) == this)
		TEXTUREGLOBAL(currentTexDict) = nil;
	FORLIST(lnk, this->textures){
		Texture *tex = Texture::fromDict(lnk);
		this->remove(tex);
		tex->destroy();
	}
	s_plglist.destruct(this);
	this->inGlobalList.remove();
	rwFree(this);
	numAllocated--;
}

void
TexDictionary::add(Texture *t)
{
	if(t->dict)
		t->inDict.remove();
	t->dict = this;
	this->textures.append(&t->inDict);
}

void
TexDictionary::remove(Texture *t)
{
	assert(t->dict == this);
	t->inDict.remove();
	t->dict = nil;
}

void
TexDictionary::addFront(Texture *t)
{
	if(t->dict)
		t->inDict.remove();
	t->dict = this;
	this->textures.add(&t->inDict);
}

Texture*
TexDictionary::find(const char *name)
{
	FORLIST(lnk, this->textures){
		Texture *tex = Texture::fromDict(lnk);
		if(strncmp_ci(tex->name, name, 32) == 0)
			return tex;
	}
	return nil;
}

TexDictionary*
TexDictionary::streamRead(Stream *stream)
{
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	int32 numTex = stream->readI16();
	stream->readI16(); // device id (0 = unknown, 1 = d3d8, 2 = d3d9,
	                   // 3 = gcn, 4 = null, 5 = opengl,
	                   // 6 = ps2, 7 = softras, 8 = xbox, 9 = psp)
	TexDictionary *txd = TexDictionary::create();
	if(txd == nil)
		return nil;
	Texture *tex;
	for(int32 i = 0; i < numTex; i++){
		if(!findChunk(stream, ID_TEXTURENATIVE, nil, nil)){
			RWERROR((ERR_CHUNK, "TEXTURENATIVE"));
			goto fail;
		}
		tex = Texture::streamReadNative(stream);
		if(tex == nil)
			goto fail;
		Texture::s_plglist.streamRead(stream, tex);
		txd->add(tex);
	}
	if(s_plglist.streamRead(stream, txd))
		return txd;
fail:
	txd->destroy();
	return nil;
}

void
TexDictionary::streamWrite(Stream *stream)
{
	writeChunkHeader(stream, ID_TEXDICTIONARY, this->streamGetSize());
	writeChunkHeader(stream, ID_STRUCT, 4);
	int32 numTex = this->count();
	stream->writeI16(numTex);
	stream->writeI16(0);
	FORLIST(lnk, this->textures){
		Texture *tex = Texture::fromDict(lnk);
		uint32 sz = tex->streamGetSizeNative();
		sz += 12 + Texture::s_plglist.streamGetSize(tex);
		writeChunkHeader(stream, ID_TEXTURENATIVE, sz);
		tex->streamWriteNative(stream);
		Texture::s_plglist.streamWrite(stream, tex);
	}
	s_plglist.streamWrite(stream, this);
}

uint32
TexDictionary::streamGetSize(void)
{
	uint32 size = 12 + 4;
	FORLIST(lnk, this->textures){
		Texture *tex = Texture::fromDict(lnk);
		size += 12 + tex->streamGetSizeNative();
		size += 12 + Texture::s_plglist.streamGetSize(tex);
	}
	size += 12 + s_plglist.streamGetSize(this);
	return size;
}

void
TexDictionary::setCurrent(TexDictionary *txd)
{
	PLUGINOFFSET(TextureGlobals, engine, textureModuleOffset)->currentTexDict = txd;
}

TexDictionary*
TexDictionary::getCurrent(void)
{
	return PLUGINOFFSET(TextureGlobals, engine, textureModuleOffset)->currentTexDict;
}

//
// Texture
//

static Texture *defaultFindCB(const char *name);
static Texture *defaultReadCB(const char *name, const char *mask);

Texture *(*Texture::findCB)(const char *name) = defaultFindCB;
Texture *(*Texture::readCB)(const char *name, const char *mask) = defaultReadCB;

Texture*
Texture::create(Raster *raster)
{
	Texture *tex = (Texture*)rwMalloc(s_plglist.size, MEMDUR_EVENT | ID_TEXTURE);
	if(tex == nil){
		RWERROR((ERR_ALLOC, s_plglist.size));
		return nil;
	}
	numAllocated++;
	tex->dict = nil;
	tex->inDict.init();
	memset(tex->name, 0, 32);
	memset(tex->mask, 0, 32);
	tex->filterAddressing = (WRAP << 12) | (WRAP << 8) | NEAREST;
	tex->raster = raster;
	tex->refCount = 1;
	TEXTUREGLOBAL(textures).add(&tex->inGlobalList);
	s_plglist.construct(tex);
	return tex;
}

void
Texture::destroy(void)
{
	this->refCount--;
	if(this->refCount <= 0){
		s_plglist.destruct(this);
		if(this->dict)
			this->inDict.remove();
		if(this->raster)
			this->raster->destroy();
		this->inGlobalList.remove();
		rwFree(this);
		numAllocated--;
	}
}

static Texture*
defaultFindCB(const char *name)
{
	if(TEXTUREGLOBAL(currentTexDict))
		return TEXTUREGLOBAL(currentTexDict)->find(name);
	// TODO: RW searches *all* TXDs otherwise
	return nil;
}


static Texture*
defaultReadCB(const char *name, const char *mask)
{
	Texture *tex;
	Image *img;

	img = Image::readMasked(name, mask);
	if(img){
		tex = Texture::create(Raster::createFromImage(img));
		strncpy(tex->name, name, 32);
		if(mask)
			strncpy(tex->mask, mask, 32);
		img->destroy();
		return tex;
	}else
		return nil;
}

Texture*
Texture::read(const char *name, const char *mask)
{
	(void)mask;
	Raster *raster = nil;
	Texture *tex;

	if(tex = Texture::findCB(name), tex){
		tex->addRef();
		return tex;
	}
	if(TEXTUREGLOBAL(loadTextures)){
		tex = Texture::readCB(name, mask);
		if(tex == nil)
			goto dummytex;
	}else dummytex: if(TEXTUREGLOBAL(makeDummies)){
//printf("missing texture %s %s\n", name ? name : "", mask ? mask : "");
		tex = Texture::create(nil);
		if(tex == nil)
			return nil;
		strncpy(tex->name, name, 32);
		if(mask)
			strncpy(tex->mask, mask, 32);
		raster = Raster::create(0, 0, 0, Raster::DONTALLOCATE);
		tex->raster = raster;
	}
	if(tex && TEXTUREGLOBAL(currentTexDict)){
		if(tex->dict)
			tex->inDict.remove();
		TEXTUREGLOBAL(currentTexDict)->add(tex);
	}
	return tex;
}

Texture*
Texture::streamRead(Stream *stream)
{
	uint32 length;
	char name[128], mask[128];
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	uint32 filterAddressing = stream->readU32();
	// if V addressing is 0, copy U
	if((filterAddressing & 0xF000) == 0)
		filterAddressing |= (filterAddressing&0xF00) << 4;

	// if using mipmap filter mode, set automipmapping,
	// if 0x10000 is set, set mipmapping

	if(!findChunk(stream, ID_STRING, &length, nil)){
		RWERROR((ERR_CHUNK, "STRING"));
		return nil;
	}
	stream->read8(name, length);

	if(!findChunk(stream, ID_STRING, &length, nil)){
		RWERROR((ERR_CHUNK, "STRING"));
		return nil;
	}
	stream->read8(mask, length);

	bool32 mipState = getMipmapping();
	bool32 autoMipState = getAutoMipmapping();
	int32 filter = filterAddressing&0xFF;
	if(filter == MIPNEAREST || filter == MIPLINEAR ||
	   filter == LINEARMIPNEAREST || filter == LINEARMIPLINEAR){
		setMipmapping(1);
		setAutoMipmapping((filterAddressing&0x10000) == 0);
	}else{
		setMipmapping(0);
		setAutoMipmapping(0);
	}

	Texture *tex = Texture::read(name, mask);

	setMipmapping(mipState);
	setAutoMipmapping(autoMipState);

	if(tex == nil){
		s_plglist.streamSkip(stream);
		return nil;
	}
	if(tex->refCount == 1)
		tex->filterAddressing = filterAddressing&0xFFFF;

	if(s_plglist.streamRead(stream, tex))
		return tex;

	tex->destroy();
	return nil;
}

bool
Texture::streamWrite(Stream *stream)
{
	int size;
	char buf[36];
	writeChunkHeader(stream, ID_TEXTURE, this->streamGetSize());
	writeChunkHeader(stream, ID_STRUCT, 4);
	uint32 filterAddressing = this->filterAddressing;
	if(this->raster && (raster->format & Raster::AUTOMIPMAP) == 0)
		filterAddressing |= 0x10000;
	stream->writeU32(filterAddressing);

	memset(buf, 0, 36);
	strncpy(buf, this->name, 32);
	size = strlen(buf)+4 & ~3;
	writeChunkHeader(stream, ID_STRING, size);
	stream->write8(buf, size);

	memset(buf, 0, 36);
	strncpy(buf, this->mask, 32);
	size = strlen(buf)+4 & ~3;
	writeChunkHeader(stream, ID_STRING, size);
	stream->write8(buf, size);

	s_plglist.streamWrite(stream, this);
	return true;
}

uint32
Texture::streamGetSize(void)
{
	uint32 size = 0;
	size += 12 + 4;
	size += 12 + 12;
	size += strlen(this->name)+4 & ~3;
	size += strlen(this->mask)+4 & ~3;
	size += 12 + s_plglist.streamGetSize(this);
	return size;
}

Texture*
Texture::streamReadNative(Stream *stream)
{
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	uint32 platform = stream->readU32();
	stream->seek(-16);
	if(platform == FOURCC_PS2)
		return ps2::readNativeTexture(stream);
	if(platform == PLATFORM_D3D8)
		return d3d8::readNativeTexture(stream);
	if(platform == PLATFORM_D3D9)
		return d3d9::readNativeTexture(stream);
	if(platform == PLATFORM_XBOX)
		return xbox::readNativeTexture(stream);
	if(platform == PLATFORM_GL3)
		return gl3::readNativeTexture(stream);
	assert(0 && "unsupported platform");
	return nil;
}

void
Texture::streamWriteNative(Stream *stream)
{
	if(this->raster->platform == PLATFORM_PS2)
		ps2::writeNativeTexture(this, stream);
	else if(this->raster->platform == PLATFORM_D3D8)
		d3d8::writeNativeTexture(this, stream);
	else if(this->raster->platform == PLATFORM_D3D9)
		d3d9::writeNativeTexture(this, stream);
	else if(this->raster->platform == PLATFORM_XBOX)
		xbox::writeNativeTexture(this, stream);
	else if(this->raster->platform == PLATFORM_GL3)
		gl3::writeNativeTexture(this, stream);
	else
		assert(0 && "unsupported platform");
}

uint32
Texture::streamGetSizeNative(void)
{
	if(this->raster->platform == PLATFORM_PS2)
		return ps2::getSizeNativeTexture(this);
	if(this->raster->platform == PLATFORM_D3D8)
		return d3d8::getSizeNativeTexture(this);
	if(this->raster->platform == PLATFORM_D3D9)
		return d3d9::getSizeNativeTexture(this);
	if(this->raster->platform == PLATFORM_XBOX)
		return xbox::getSizeNativeTexture(this);
	if(this->raster->platform == PLATFORM_GL3)
		return gl3::getSizeNativeTexture(this);
	assert(0 && "unsupported platform");
	return 0;
}



int32 anisotOffset;

static void*
createAnisot(void *object, int32 offset, int32)
{
	*GETANISOTROPYEXT(object) = 1;
	return object;
}

static void*
copyAnisot(void *dst, void *src, int32 offset, int32)
{
	*GETANISOTROPYEXT(dst) = *GETANISOTROPYEXT(src);
	return dst;
}

static Stream*
readAnisot(Stream *stream, int32, void *object, int32 offset, int32)
{
	*GETANISOTROPYEXT(object) = stream->readI32();
	return stream;
}

static Stream*
writeAnisot(Stream *stream, int32, void *object, int32 offset, int32)
{
	stream->writeI32(*GETANISOTROPYEXT(object));
	return stream;
}

static int32
getSizeAnisot(void *object, int32 offset, int32)
{
	if(*GETANISOTROPYEXT(object) == 1)
		return 0;
	return sizeof(int32);
}

void
registerAnisotropyPlugin(void)
{
	anisotOffset = Texture::registerPlugin(sizeof(int32), ID_ANISOT, createAnisot, nil, copyAnisot);
	Texture::registerPluginStream(ID_ANISOT, readAnisot, writeAnisot, getSizeAnisot);
}

void
Texture::setMaxAnisotropy(int32 maxaniso)
{
	if(anisotOffset > 0)
		*GETANISOTROPYEXT(this) = maxaniso;
}

int32
Texture::getMaxAnisotropy(void)
{
	if(anisotOffset > 0)
		return *GETANISOTROPYEXT(this);
	return 1;
}

int32
getMaxSupportedMaxAnisotropy(void)
{
#ifdef RW_D3D9
	return d3d::d3d9Globals.caps.MaxAnisotropy;
#endif
#ifdef RW_GL3
	return (int32)gl3::gl3Caps.maxAnisotropy;
#endif
	return 1;
}

}

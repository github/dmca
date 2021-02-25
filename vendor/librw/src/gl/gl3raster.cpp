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
#include "rwgl3impl.h"

#define PLUGIN_ID ID_DRIVER

namespace rw {
namespace gl3 {

int32 nativeRasterOffset;

static uint32
getLevelSize(Raster *raster, int32 level)
{
	int i;
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	int w = raster->originalWidth;
	int h = raster->originalHeight;
	int s = raster->originalStride;
	int minDim = 1;

#ifdef RW_OPENGL
	switch(natras->internalFormat){
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		minDim = 4;
		break;
	}
#endif

	for(i = 0; i < level; i++){
		if(w > minDim){
			w /= 2;
			s /= 2;
		}
		if(h > minDim)
			h /= 2;
	}

	return s*h;
}

#ifdef RW_OPENGL

static Raster*
rasterCreateTexture(Raster *raster)
{
	if(raster->format & (Raster::PAL4 | Raster::PAL8)){
		RWERROR((ERR_NOTEXTURE));
		return nil;
	}

	Gl3Raster *natras = GETGL3RASTEREXT(raster);
	switch(raster->format & 0xF00){
	case Raster::C8888:
		natras->internalFormat = GL_RGBA8;
		natras->format = GL_RGBA;
		natras->type = GL_UNSIGNED_BYTE;
		natras->hasAlpha = 1;
		natras->bpp = 4;
		raster->depth = 32;
		break;
	case Raster::C888:
		natras->internalFormat = GL_RGB8;
		natras->format = GL_RGB;
		natras->type = GL_UNSIGNED_BYTE;
		natras->hasAlpha = 0;
		natras->bpp = 3;
		raster->depth = 24;
		break;
	case Raster::C1555:
		natras->internalFormat = GL_RGB5_A1;
		natras->format = GL_RGBA;
		natras->type = GL_UNSIGNED_SHORT_5_5_5_1;
		natras->hasAlpha = 1;
		natras->bpp = 2;
		raster->depth = 16;
		break;
	default:
		RWERROR((ERR_INVRASTER));
		return nil;
	}

	if(gl3Caps.gles){
		// glReadPixels only supports GL_RGBA
		natras->internalFormat = GL_RGBA8;
		natras->format = GL_RGBA;
		natras->type = GL_UNSIGNED_BYTE;
		natras->bpp = 4;
	}

	raster->stride = raster->width*natras->bpp;

	if(raster->format & Raster::MIPMAP){
		int w = raster->width;
		int h = raster->height;
		natras->numLevels = 1;
		while(w != 1 || h != 1){
			natras->numLevels++;
			if(w > 1) w /= 2;
			if(h > 1) h /= 2;
		}
	}
	natras->autogenMipmap = (raster->format & (Raster::MIPMAP|Raster::AUTOMIPMAP)) == (Raster::MIPMAP|Raster::AUTOMIPMAP);
	if(natras->autogenMipmap)
		natras->numLevels = 1;

	glGenTextures(1, &natras->texid);
	uint32 prev = bindTexture(natras->texid);
	glTexImage2D(GL_TEXTURE_2D, 0, natras->internalFormat,
	             raster->width, raster->height,
	             0, natras->format, natras->type, nil);
	// TODO: allocate other levels...probably
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, natras->numLevels-1);
	natras->filterMode = 0;
	natras->addressU = 0;
	natras->addressV = 0;
	natras->maxAnisotropy = 1;

	bindTexture(prev);
	return raster;
}

static Raster*
rasterCreateCameraTexture(Raster *raster)
{
	if(raster->format & (Raster::PAL4 | Raster::PAL8)){
		RWERROR((ERR_NOTEXTURE));
		return nil;
	}

	// TODO: figure out what the backbuffer is and use that as a default
	Gl3Raster *natras = GETGL3RASTEREXT(raster);
	switch(raster->format & 0xF00){
	case Raster::C8888:
		natras->internalFormat = GL_RGBA8;
		natras->format = GL_RGBA;
		natras->type = GL_UNSIGNED_BYTE;
		natras->hasAlpha = 1;
		natras->bpp = 4;
		break;
	case Raster::C888:
	default:
		natras->internalFormat = GL_RGB8;
		natras->format = GL_RGB;
		natras->type = GL_UNSIGNED_BYTE;
		natras->hasAlpha = 0;
		natras->bpp = 3;
		break;
	case Raster::C1555:
		natras->internalFormat = GL_RGB5_A1;
		natras->format = GL_RGBA;
		natras->type = GL_UNSIGNED_SHORT_5_5_5_1;
		natras->hasAlpha = 1;
		natras->bpp = 2;
		break;
	}

	// i don't remember why this was once here...
	if(gl3Caps.gles){
		// glReadPixels only supports GL_RGBA
//		natras->internalFormat = GL_RGBA8;
//		natras->format = GL_RGBA;
//		natras->type = GL_UNSIGNED_BYTE;
//		natras->bpp = 4;
	}

	raster->stride = raster->width*natras->bpp;

	natras->autogenMipmap = (raster->format & (Raster::MIPMAP|Raster::AUTOMIPMAP)) == (Raster::MIPMAP|Raster::AUTOMIPMAP);

	glGenTextures(1, &natras->texid);
	uint32 prev = bindTexture(natras->texid);
	glTexImage2D(GL_TEXTURE_2D, 0, natras->internalFormat,
	             raster->width, raster->height,
	             0, natras->format, natras->type, nil);
	natras->filterMode = 0;
	natras->addressU = 0;
	natras->addressV = 0;
	natras->maxAnisotropy = 1;

	bindTexture(prev);


	glGenFramebuffers(1, &natras->fbo);
	bindFramebuffer(natras->fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, natras->texid, 0);
	bindFramebuffer(0);
	natras->fboMate = nil;

	return raster;
}

static Raster*
rasterCreateCamera(Raster *raster)
{
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	// TODO: set/check width, height, depth, format?

	// used for locking right now
	raster->format = Raster::C888;
	natras->internalFormat = GL_RGB8;
	natras->format = GL_RGB;
	natras->type = GL_UNSIGNED_BYTE;
	natras->hasAlpha = 0;
	natras->bpp = 3;

	natras->autogenMipmap = 0;

	natras->texid = 0;
	natras->fbo = 0;
	natras->fboMate = nil;

	return raster;
}

static Raster*
rasterCreateZbuffer(Raster *raster)
{
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	if(gl3Caps.gles){
		// have to use RBO on GLES!!
		glGenRenderbuffers(1, &natras->texid);
		glBindRenderbuffer(GL_RENDERBUFFER, natras->texid);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, raster->width, raster->height);
	}else{
		// TODO: set/check width, height, depth, format?
		natras->internalFormat = GL_DEPTH_STENCIL;
		natras->format = GL_DEPTH_STENCIL;
		natras->type = GL_UNSIGNED_INT_24_8;

		natras->autogenMipmap = 0;

		glGenTextures(1, &natras->texid);
		uint32 prev = bindTexture(natras->texid);
		glTexImage2D(GL_TEXTURE_2D, 0, natras->internalFormat,
			     raster->width, raster->height,
			     0, natras->format, natras->type, nil);
		natras->filterMode = 0;
		natras->addressU = 0;
		natras->addressV = 0;
		natras->maxAnisotropy = 1;

		bindTexture(prev);
	}
	natras->fbo = 0;
	natras->fboMate = nil;

	return raster;
}


#endif


void
allocateDXT(Raster *raster, int32 dxt, int32 numLevels, bool32 hasAlpha)
{
#ifdef RW_OPENGL
	assert(raster->type == Raster::TEXTURE);

	Gl3Raster *natras = GETGL3RASTEREXT(raster);
	switch(dxt){
	case 1:
		if(hasAlpha){
			natras->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			natras->format = GL_RGBA;
		}else{
			natras->internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			natras->format = GL_RGB;
		}
		// bogus, but stride*height should be the size of the image
		// 4x4 in 8 bytes
		raster->stride = raster->width/2;
		break;
	case 3:
		natras->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		natras->format = GL_RGBA;
		// 4x4 in 16 bytes
		raster->stride = raster->width;
		break;
	case 5:
		natras->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		natras->format = GL_RGBA;
		// 4x4 in 16 bytes
		raster->stride = raster->width;
		break;
	default:
		assert(0 && "invalid DXT format");
	}
	natras->type = GL_UNSIGNED_BYTE;
	natras->hasAlpha = hasAlpha;
	natras->bpp = 2;
	raster->depth = 16;

	natras->isCompressed = 1;
	if(raster->format & Raster::MIPMAP)
		natras->numLevels = numLevels;
	natras->autogenMipmap = (raster->format & (Raster::MIPMAP|Raster::AUTOMIPMAP)) == (Raster::MIPMAP|Raster::AUTOMIPMAP);
	if(natras->autogenMipmap)
		natras->numLevels = 1;

	glGenTextures(1, &natras->texid);
	uint32 prev = bindTexture(natras->texid);
	glTexImage2D(GL_TEXTURE_2D, 0, natras->internalFormat,
	             raster->width, raster->height,
	             0, natras->format, natras->type, nil);
	// TODO: allocate other levels...probably
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, natras->numLevels-1);
	natras->filterMode = 0;
	natras->addressU = 0;
	natras->addressV = 0;
	natras->maxAnisotropy = 1;

	bindTexture(prev);

	raster->originalStride = raster->stride;

	if(gl3Caps.gles && needToReadBackTextures){
		// Uh oh, need to keep a copy in cpu memory
		int32 i;
		int32 size = 0;
		for(i = 0; i < natras->numLevels; i++)
			size += getLevelSize(raster, i);
		uint8 *data = (uint8*)rwNew(sizeof(RasterLevels)+sizeof(RasterLevels::Level)*(natras->numLevels-1)+size,
			MEMDUR_EVENT | ID_DRIVER);
		RasterLevels *levels = (RasterLevels*)data;
		data += sizeof(RasterLevels)+sizeof(RasterLevels::Level)*(natras->numLevels-1);
		levels->numlevels = natras->numLevels;
		levels->format = 0;	// not needed
		for(i = 0; i < natras->numLevels; i++){
			levels->levels[i].data = data;
			levels->levels[i].size = getLevelSize(raster, i);
			levels->levels[i].width = 0;	// we don't need those here, maybe later...
			levels->levels[i].height = 0;
			data += levels->levels[i].size;
		}
		natras->backingStore = levels;
	}

	raster->flags &= ~Raster::DONTALLOCATE;
#endif
}

/*
{ 0, 0, 0 },
{ 16, 4, GL_RGBA },	// 1555
{ 16, 3, GL_RGB },	// 565
{ 16, 4, GL_RGBA },	// 4444
{ 0, 0, 0 },	// LUM8
{ 32, 4, GL_RGBA },	// 8888
{ 24, 3, GL_RGB },	// 888
{ 16, 3, GL_RGB },	// D16
{ 24, 3, GL_RGB },	// D24
{ 32, 4, GL_RGBA },	// D32
{ 16, 3, GL_RGB },	// 555

0,
GL_RGB5_A1,
GL_RGB5,
GL_RGBA4,
0,
GL_RGBA8,
GL_RGB8,
GL_RGB5,
GL_RGB8,
GL_RGBA8,
GL_RGB5
*/

Raster*
rasterCreate(Raster *raster)
{
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	natras->isCompressed = 0;
	natras->hasAlpha = 0;
	natras->numLevels = 1;

	Raster *ret = raster;

	if(raster->width == 0 || raster->height == 0){
		raster->flags |= Raster::DONTALLOCATE;
		raster->stride = 0;
		goto ret;
	}
	if(raster->flags & Raster::DONTALLOCATE)
		goto ret;

	switch(raster->type){
#ifdef RW_OPENGL
	case Raster::NORMAL:
	case Raster::TEXTURE:
		ret = rasterCreateTexture(raster);
		break;
	case Raster::CAMERATEXTURE:
		ret = rasterCreateCameraTexture(raster);
		break;
	case Raster::ZBUFFER:
		ret = rasterCreateZbuffer(raster);
		break;
	case Raster::CAMERA:
		ret = rasterCreateCamera(raster);
		break;
#endif

	default:
		RWERROR((ERR_INVRASTER));
		return nil;
	}

ret:
	raster->originalWidth = raster->width;
	raster->originalHeight = raster->height;
	raster->originalStride = raster->stride;
	raster->originalPixels = raster->pixels;
	return ret;
}

uint8*
rasterLock(Raster *raster, int32 level, int32 lockMode)
{
#ifdef RW_OPENGL
	Gl3Raster *natras GETGL3RASTEREXT(raster);
	uint8 *px;
	uint32 allocSz;
	int i;

	assert(raster->privateFlags == 0);

	switch(raster->type){
	case Raster::NORMAL:
	case Raster::TEXTURE:
	case Raster::CAMERATEXTURE:
		for(i = 0; i < level; i++){
			if(raster->width > 1){
				raster->width /= 2;
				raster->stride /= 2;
			}
			if(raster->height > 1)
				raster->height /= 2;
		}

		allocSz = getLevelSize(raster, level);
		px = (uint8*)rwMalloc(allocSz, MEMDUR_EVENT | ID_DRIVER);
		assert(raster->pixels == nil);
		raster->pixels = px;

		if(lockMode & Raster::LOCKREAD || !(lockMode & Raster::LOCKNOFETCH)){
			if(natras->isCompressed){
				if(natras->backingStore){
					assert(level < natras->backingStore->numlevels);
					assert(allocSz >= natras->backingStore->levels[level].size);
					memcpy(px, natras->backingStore->levels[level].data, allocSz);
				}else{
					// GLES is losing here
					uint32 prev = bindTexture(natras->texid);
					glGetCompressedTexImage(GL_TEXTURE_2D, level, px);
					bindTexture(prev);
				}
			}else if(gl3Caps.gles){
				GLuint fbo;
				glGenFramebuffers(1, &fbo);
				bindFramebuffer(fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, natras->texid, 0);
				GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
assert(natras->format == GL_RGBA);
				glReadPixels(0, 0, raster->width, raster->height, natras->format, natras->type, px);
//e = glGetError(); printf("GL err4 %x (%x)\n", e, natras->format);
				bindFramebuffer(0);
				glDeleteFramebuffers(1, &fbo);
			}else{
				uint32 prev = bindTexture(natras->texid);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glGetTexImage(GL_TEXTURE_2D, level, natras->format, natras->type, px);
				bindTexture(prev);
			}
		}

		raster->privateFlags = lockMode;
		break;

	case Raster::CAMERA:
		if(lockMode & Raster::PRIVATELOCK_WRITE)
			assert(0 && "can't lock framebuffer for writing");
		raster->width = glGlobals.presentWidth;
		raster->height = glGlobals.presentHeight;
		raster->stride = raster->width*natras->bpp;
		assert(natras->bpp == 3);
		allocSz = raster->height*raster->stride;
		px = (uint8*)rwMalloc(allocSz, MEMDUR_EVENT | ID_DRIVER);
		assert(raster->pixels == nil);
		raster->pixels = px;
		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, raster->width, raster->height, GL_RGB, GL_UNSIGNED_BYTE, px);

		raster->privateFlags = lockMode;
		break;

	default:
		assert(0 && "cannot lock this type of raster yet");
		return nil;
	}

	return px;
#else
	return nil;
#endif
}

void
rasterUnlock(Raster *raster, int32 level)
{
#ifdef RW_OPENGL
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	assert(raster->pixels);

	switch(raster->type){
	case Raster::NORMAL:
	case Raster::TEXTURE:
	case Raster::CAMERATEXTURE:
		if(raster->privateFlags & Raster::LOCKWRITE){
			uint32 prev = bindTexture(natras->texid);
			if(natras->isCompressed){
				glCompressedTexImage2D(GL_TEXTURE_2D, level, natras->internalFormat,
					raster->width, raster->height, 0,
					getLevelSize(raster, level),
					raster->pixels);
				if(natras->backingStore){
					assert(level < natras->backingStore->numlevels);
					memcpy(natras->backingStore->levels[level].data, raster->pixels,
						natras->backingStore->levels[level].size);
				}
			}else{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, level, natras->internalFormat,
					     raster->width, raster->height,
					     0, natras->format, natras->type, raster->pixels);
			}
			if(level == 0 && natras->autogenMipmap)
				glGenerateMipmap(GL_TEXTURE_2D);
			bindTexture(prev);
		}
		break;

	case Raster::CAMERA:
		// TODO: write?
		break;
	}

	rwFree(raster->pixels);
	raster->pixels = nil;
#endif
	raster->width = raster->originalWidth;
	raster->height = raster->originalHeight;
	raster->stride = raster->originalStride;
	raster->pixels = raster->originalPixels;
	raster->privateFlags = 0;
}

int32
rasterNumLevels(Raster *raster)
{
	return GETGL3RASTEREXT(raster)->numLevels;
}

// Almost the same as d3d9 and ps2 function
bool32
imageFindRasterFormat(Image *img, int32 type,
	int32 *pWidth, int32 *pHeight, int32 *pDepth, int32 *pFormat)
{
	int32 width, height, depth, format;

	assert((type&0xF) == Raster::TEXTURE);

//	for(width = 1; width < img->width; width <<= 1);
//	for(height = 1; height < img->height; height <<= 1);
	// Perhaps non-power-of-2 textures are acceptable?
	width = img->width;
	height = img->height;

	depth = img->depth;

	if(depth <= 8)
		depth = 32;

	switch(depth){
	case 32:
		if(img->hasAlpha())
			format = Raster::C8888;
		else{
			format = Raster::C888;
			depth = 24;
		}
		break;
	case 24:
		format = Raster::C888;
		break;
	case 16:
		format = Raster::C1555;
		break;

	case 8:
	case 4:
	default:
		RWERROR((ERR_INVRASTER));
		return 0;
	}

	format |= type;

	*pWidth = width;
	*pHeight = height;
	*pDepth = depth;
	*pFormat = format;

	return 1;
}

bool32
rasterFromImage(Raster *raster, Image *image)
{
	if((raster->type&0xF) != Raster::TEXTURE)
		return 0;

	void (*conv)(uint8 *out, uint8 *in) = nil;

	// Unpalettize image if necessary but don't change original
	Image *truecolimg = nil;
	if(image->depth <= 8){
		truecolimg = Image::create(image->width, image->height, image->depth);
		truecolimg->pixels = image->pixels;
		truecolimg->stride = image->stride;
		truecolimg->palette = image->palette;
		truecolimg->unpalettize();
		image = truecolimg;
	}

	Gl3Raster *natras = GETGL3RASTEREXT(raster);
	int32 format = raster->format&0xF00;
	assert(!natras->isCompressed);
	switch(image->depth){
	case 32:
		if(gl3Caps.gles)
			conv = conv_RGBA8888_from_RGBA8888;
		else if(format == Raster::C8888)
			conv = conv_RGBA8888_from_RGBA8888;
		else if(format == Raster::C888)
			conv = conv_RGB888_from_RGB888;
		else
			goto err;
		break;
	case 24:
		if(gl3Caps.gles)
			conv = conv_RGBA8888_from_RGB888;
		else if(format == Raster::C8888)
			conv = conv_RGBA8888_from_RGB888;
		else if(format == Raster::C888)
			conv = conv_RGB888_from_RGB888;
		else
			goto err;
		break;
	case 16:
		if(gl3Caps.gles)
			conv = conv_RGBA8888_from_ARGB1555;
		else if(format == Raster::C1555)
			conv = conv_RGBA5551_from_ARGB1555;
		else
			goto err;
		break;

	case 8:
	case 4:
	default:
	err:
		RWERROR((ERR_INVRASTER));
		return 0;
	}

	natras->hasAlpha = image->hasAlpha();

	bool unlock = false;
	if(raster->pixels == nil){
		raster->lock(0, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
		unlock = true;
	}

	uint8 *pixels = raster->pixels;
	assert(pixels);
	uint8 *imgpixels = image->pixels + (image->height-1)*image->stride;

	int x, y;
	assert(image->width == raster->width);
	assert(image->height == raster->height);
	for(y = 0; y < image->height; y++){
		uint8 *imgrow = imgpixels;
		uint8 *rasrow = pixels;
		for(x = 0; x < image->width; x++){
			conv(rasrow, imgrow);
			imgrow += image->bpp;
			rasrow += natras->bpp;
		}
		imgpixels -= image->stride;
		pixels += raster->stride;
	}
	if(unlock)
		raster->unlock(0);

	if(truecolimg)
		truecolimg->destroy();

	return 1;
}

Image*
rasterToImage(Raster *raster)
{
	int32 depth;
	Image *image;

	bool unlock = false;
	if(raster->pixels == nil){
		raster->lock(0, Raster::LOCKREAD);
		unlock = true;
	}

	Gl3Raster *natras = GETGL3RASTEREXT(raster);
	if(natras->isCompressed){
		// TODO
		RWERROR((ERR_INVRASTER));
		return nil;
	}

	void (*conv)(uint8 *out, uint8 *in) = nil;
	switch(raster->format & 0xF00){
	case Raster::C1555:
		depth = 16;
		conv = conv_ARGB1555_from_RGBA5551;
		break;
	case Raster::C8888:
		depth = 32;
		conv = conv_RGBA8888_from_RGBA8888;
		break;
	case Raster::C888:
		depth = 24;
		conv = conv_RGB888_from_RGB888;
		break;

	default:
	case Raster::C555:
	case Raster::C565:
	case Raster::C4444:
	case Raster::LUM8:
		RWERROR((ERR_INVRASTER));
		return nil;
	}

	if(raster->format & Raster::PAL4 ||
	   raster->format & Raster::PAL8){
		RWERROR((ERR_INVRASTER));
		return nil;
	}
		
	uint8 *in, *out;
	image = Image::create(raster->width, raster->height, depth);
	image->allocate();

	uint8 *imgpixels = image->pixels + (image->height-1)*image->stride;
	uint8 *pixels = raster->pixels;

	int x, y;
	assert(image->width == raster->width);
	assert(image->height == raster->height);
	for(y = 0; y < image->height; y++){
		uint8 *imgrow = imgpixels;
		uint8 *rasrow = pixels;
		for(x = 0; x < image->width; x++){
			conv(imgrow, rasrow);
			imgrow += image->bpp;
			rasrow += natras->bpp;
		}
		imgpixels -= image->stride;
		pixels += raster->stride;
	}

	if(unlock)
		raster->unlock(0);

	return image;
}

static void*
createNativeRaster(void *object, int32 offset, int32)
{
	Gl3Raster *ras = PLUGINOFFSET(Gl3Raster, object, offset);
	ras->texid = 0;
	ras->fbo = 0;
	ras->fboMate = nil;
	ras->backingStore = nil;
	return object;
}

static void*
destroyNativeRaster(void *object, int32 offset, int32)
{
	Raster *raster = (Raster*)object;
	Gl3Raster *natras = PLUGINOFFSET(Gl3Raster, object, offset);
#ifdef RW_OPENGL
	switch(raster->type){
	case Raster::NORMAL:
	case Raster::TEXTURE:
		glDeleteTextures(1, &natras->texid);
		break;

	case Raster::CAMERATEXTURE:
		if(natras->fboMate){
			// Break apart from currently associated zbuffer
			Gl3Raster *zras = GETGL3RASTEREXT(natras->fboMate);
			zras->fboMate = nil;
			natras->fboMate = nil;
		}
		glDeleteFramebuffers(1, &natras->fbo);
		glDeleteTextures(1, &natras->texid);
		break;

	case Raster::ZBUFFER:
		if(natras->fboMate){
			// Detatch from FBO we may be attached to
			Gl3Raster *oldfb = GETGL3RASTEREXT(natras->fboMate);
			if(oldfb->fbo){
				bindFramebuffer(oldfb->fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			}
			oldfb->fboMate = nil;
		}
		if(gl3Caps.gles)
			glDeleteRenderbuffers(1, &natras->texid);
		else
			glDeleteTextures(1, &natras->texid);
		break;

	case Raster::CAMERA:
		if(natras->fboMate){
			// Break apart from currently associated zbuffer
			Gl3Raster *zras = GETGL3RASTEREXT(natras->fboMate);
			zras->fboMate = nil;
			natras->fboMate = nil;
		}
		break;
	}
	natras->texid = 0;
	natras->fbo = 0;

	free(natras->backingStore);

#endif
	return object;
}

static void*
copyNativeRaster(void *dst, void *, int32 offset, int32)
{
	Gl3Raster *d = PLUGINOFFSET(Gl3Raster, dst, offset);
	d->texid = 0;
	d->fbo = 0;
	d->fboMate = nil;
	d->backingStore = nil;
	return dst;
}

Texture*
readNativeTexture(Stream *stream)
{
	uint32 platform;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	platform = stream->readU32();
	if(platform != PLATFORM_GL3){
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}
	Texture *tex = Texture::create(nil);
	if(tex == nil)
		return nil;

	// Texture
	tex->filterAddressing = stream->readU32();
	stream->read8(tex->name, 32);
	stream->read8(tex->mask, 32);

	// Raster
	uint32 format = stream->readU32();
	int32 width = stream->readI32();
	int32 height = stream->readI32();
	int32 depth = stream->readI32();
	int32 numLevels = stream->readI32();

	// Native raster
	int32 subplatform = stream->readI32();
	int32 flags = stream->readI32();
	int32 compression = stream->readI32();

	if(subplatform != gl3Caps.gles){
		tex->destroy();
		RWERROR((ERR_PLATFORM, platform));
		return nil;
	}

	Raster *raster;
	Gl3Raster *natras;
	if(flags & 2){
		if(!gl3Caps.dxtSupported){
			tex->destroy();
			RWERROR((ERR_FORMAT_UNSUPPORTED));
			return nil;
		}
		raster = Raster::create(width, height, depth, format | Raster::TEXTURE | Raster::DONTALLOCATE, PLATFORM_GL3);
		allocateDXT(raster, compression, numLevels, flags & 1);
	}else{
		raster = Raster::create(width, height, depth, format | Raster::TEXTURE, PLATFORM_GL3);
	}
	assert(raster);
	natras = GETGL3RASTEREXT(raster);
	tex->raster = raster;

	uint32 size;
	uint8 *data;
	for(int32 i = 0; i < numLevels; i++){
		size = stream->readU32();
		data = raster->lock(i, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
		stream->read8(data, size);
		raster->unlock(i);
	}
	return tex;
}

void
writeNativeTexture(Texture *tex, Stream *stream)
{
	Raster *raster = tex->raster;
	Gl3Raster *natras = GETGL3RASTEREXT(raster);

	int32 chunksize = getSizeNativeTexture(tex);
	writeChunkHeader(stream, ID_STRUCT, chunksize-12);
	stream->writeU32(PLATFORM_GL3);

	// Texture
	stream->writeU32(tex->filterAddressing);
	stream->write8(tex->name, 32);
	stream->write8(tex->mask, 32);

	// Raster
	int32 numLevels = natras->numLevels;
	stream->writeI32(raster->format);
	stream->writeI32(raster->width);
	stream->writeI32(raster->height);
	stream->writeI32(raster->depth);
	stream->writeI32(numLevels);

	// Native raster
	int32 flags = 0;
	int32 compression = 0;
	if(natras->hasAlpha)
		flags |= 1;
	if(natras->isCompressed){
		flags |= 2;
		switch(natras->internalFormat){
#ifdef RW_OPENGL
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			compression = 1;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			compression = 3;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			compression = 5;
			break;
#endif
		default:
			assert(0 && "unknown compression");
		}
	}
	stream->writeI32(gl3Caps.gles);
	stream->writeI32(flags);
	stream->writeI32(compression);
	// TODO: auto mipmaps?

	uint32 size;
	uint8 *data;
	for(int32 i = 0; i < numLevels; i++){
		size = getLevelSize(raster, i);
		stream->writeU32(size);
		data = raster->lock(i, Raster::LOCKREAD);
		stream->write8(data, size);
		raster->unlock(i);
	}
}

uint32
getSizeNativeTexture(Texture *tex)
{
	uint32 size = 12 + 72 + 32;
	int32 levels = tex->raster->getNumLevels();
	for(int32 i = 0; i < levels; i++)
		size += 4 + getLevelSize(tex->raster, i);
	return size;
}



void registerNativeRaster(void)
{
	nativeRasterOffset = Raster::registerPlugin(sizeof(Gl3Raster),
	                                            ID_RASTERGL3,
	                                            createNativeRaster,
	                                            destroyNativeRaster,
	                                            copyNativeRaster);
}

}
}

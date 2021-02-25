#include <stdio.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwrender.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"
#include "rwcharset.h"

#include "ps2/rwps2.h"
#include "d3d/rwd3d.h"
#include "gl/rwgl3.h"


#define PLUGIN_ID 1000	// TODO: find a better ID

#ifndef RW_NULL

#ifdef RWHALFPIXEL
#define HALFPX (0.5f)
#else
#define HALFPX (0.0f)
#endif

namespace rw {

const uint8 fontbits[256*256] = {
#include "vgafont.inc"
};

#define NUMCHARS 100
static uint16 *indices;
static RWDEVICE::Im2DVertex *vertices;
static int32 numChars;
static Raster *lastRaster;

bool32
Charset::open(void)
{
	if(indices || vertices)
		return 0;
	numChars = 0;
	lastRaster = nil;
	indices = rwNewT(uint16, NUMCHARS*6, MEMDUR_EVENT);
	vertices = rwNewT(RWDEVICE::Im2DVertex, NUMCHARS*4, MEMDUR_EVENT);
	if(indices == nil || vertices == nil){
		close();
		return 0;
	}
	return 1;
}

void
Charset::close(void)
{
	rwFree(indices);
	indices = nil;
	rwFree(vertices);
	vertices = nil;
}

Charset*
Charset::create(const RGBA *foreground, const RGBA *background)
{
	Charset *charset = (Charset*)rwMalloc(sizeof(Charset), MEMDUR_EVENT);
	if(charset == nil){
		RWERROR((ERR_ALLOC, sizeof(Charset)));
		return nil;
	}
	charset->raster = nil;
	if(charset->setColors(foreground, background) == nil){
		charset->destroy();
		return nil;
	}
	charset->desc.count = 256;
	charset->desc.tileWidth = 28;
	charset->desc.tileHeight = 10;
	charset->desc.width = 9;
	charset->desc.height = 16;
	charset->desc.width_internal = 9;
	charset->desc.height_internal = 16;
	return charset;
}

void
Charset::destroy(void)
{
	if(raster)
		raster->destroy();
	rwFree(this);
}

Charset*
Charset::setColors(const RGBA *foreground, const RGBA *background)
{
	Image *img = Image::create(256, 256, 8);
	if(img == nil)
		return nil;
	img->pixels = (uint8*)fontbits;
	img->stride = 256;
	img->allocate();
	img->palette[0] = background->red;
	img->palette[1] = background->green;
	img->palette[2] = background->blue;
	img->palette[3] = background->alpha;
	img->palette[4] = foreground->red;
	img->palette[5] = foreground->green;
	img->palette[6] = foreground->blue;
	img->palette[7] = foreground->alpha;

	Raster *newRaster = Raster::createFromImage(img);
	img->destroy();
	if(newRaster == nil)
		return nil;
	if(this->raster)
		this->raster->destroy();
	this->raster = newRaster;
	return this;
}

void
Charset::flushBuffer(void)
{
	if(numChars){
		rw::SetRenderStatePtr(rw::TEXTURERASTER, lastRaster);
		rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
		rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);

		im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST,
			vertices, numChars*4, indices, numChars*6);
	}

	numChars = 0;
	lastRaster = nil;
}

void
Charset::printChar(int32 c, int32 x, int32 y)
{
	Camera *cam;
	float recipZ;
	float u, v, du, dv;
	RWDEVICE::Im2DVertex *vert;
	uint16 *ix;

	if(c >= this->desc.count)
		return;

	if(this->raster != lastRaster || numChars >= NUMCHARS)
		flushBuffer();
	lastRaster = this->raster;

	cam = (Camera*)engine->currentCamera;
	vert = &vertices[numChars*4];
	ix = &indices[numChars*6];
	recipZ = 1.0f/cam->nearPlane;

	u = ((c % this->desc.tileWidth)*this->desc.width_internal + HALFPX) / (float32)this->raster->width;
	v = ((c / this->desc.tileWidth)*this->desc.height_internal + HALFPX) / (float32)this->raster->height;
	du = this->desc.width_internal/(float32)this->raster->width;
	dv = this->desc.height_internal/(float32)this->raster->height;

	vert->setScreenX((float)x);
	vert->setScreenY((float)y);
	vert->setScreenZ(rw::im2d::GetNearZ());
	vert->setCameraZ(cam->nearPlane);
	vert->setRecipCameraZ(recipZ);
	vert->setColor(255, 255, 255, 255);
	vert->setU(u, recipZ);
	vert->setV(v, recipZ);
	vert++;

	vert->setScreenX(float(x+this->desc.width_internal));
	vert->setScreenY((float)y);
	vert->setScreenZ(rw::im2d::GetNearZ());
	vert->setCameraZ(cam->nearPlane);
	vert->setRecipCameraZ(recipZ);
	vert->setColor(255, 255, 255, 255);
	vert->setU(u+du, recipZ);
	vert->setV(v, recipZ);
	vert++;
	
	vert->setScreenX((float)x);
	vert->setScreenY(float(y+this->desc.height_internal));
	vert->setScreenZ(rw::im2d::GetNearZ());
	vert->setCameraZ(cam->nearPlane);
	vert->setRecipCameraZ(recipZ);
	vert->setColor(255, 255, 255, 255);
	vert->setU(u, recipZ);
	vert->setV(v+dv, recipZ);
	vert++;

	vert->setScreenX(float(x+this->desc.width_internal));
	vert->setScreenY(float(y+this->desc.height_internal));
	vert->setScreenZ(rw::im2d::GetNearZ());
	vert->setCameraZ(cam->nearPlane);
	vert->setRecipCameraZ(recipZ);
	vert->setColor(255, 255, 255, 255);
	vert->setU(u+du, recipZ);
	vert->setV(v+dv, recipZ);
	vert++;

	*ix++ = numChars*4;
	*ix++ = numChars*4+1;
	*ix++ = numChars*4+2;
	*ix++ = numChars*4+2;
	*ix++ = numChars*4+1;
	*ix++ = numChars*4+3;

	numChars++;
}

void
Charset::print(const char *str, int32 x, int32 y, bool32 hideSpaces)
{
	this->printBuffered(str, x, y, hideSpaces);
	flushBuffer();
}

void
Charset::printBuffered(const char *str, int32 x, int32 y, bool32 hideSpaces)
{
	while(*str){
		if(!hideSpaces || *str != ' ')
			printChar((uint8)*str, x, y);
		x += this->desc.width;
		str++;
	}
}

}

#endif


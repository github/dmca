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
#include "ps2/rwps2.h"
#include "d3d/rwd3d.h"
#include "d3d/rwxbox.h"
#include "d3d/rwd3d8.h"
#include "d3d/rwd3d9.h"

#define PLUGIN_ID ID_IMAGE

namespace rw {

int32 Image::numAllocated;

struct FileAssociation
{
	char *extension;
	Image *(*read)(const char *afilename);
	void (*write)(Image *image, const char *filename);
};

struct ImageGlobals
{
	char *searchPaths;
	int numSearchPaths;
	FileAssociation fileFormats[10];
	int numFileFormats;
};
int32 imageModuleOffset;

#define IMAGEGLOBAL(v) (PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset)->v)

// Image formats are as follows:
//  32 bit has 4 bytes: 8888 RGBA
//  24 bit has 3 bytes: 888 RGB
//  16 bit has 2 bytes: 1555 ARGB stored in platform native order (TODO?)
// palettes always have 4 bytes: r, g, b, a
//   8 bit has 1 byte: x
//   4 bit has 1 byte per two pixels: 0xLR, where L and R are the left and right pixel resp.

Image*
Image::create(int32 width, int32 height, int32 depth)
{
	Image *img = (Image*)rwMalloc(sizeof(Image), MEMDUR_EVENT | ID_IMAGE);
	if(img == nil){
		RWERROR((ERR_ALLOC, sizeof(Image)));
		return nil;
	}
	numAllocated++;
	img->flags = 0;
	img->width = width;
	img->height = height;
	img->depth = depth;
	img->bpp = depth < 8 ? 1 : depth/8;
	img->stride = 0;
	img->pixels = nil;
	img->palette = nil;
	return img;
}

void
Image::destroy(void)
{
	this->free();
	rwFree(this);
	numAllocated--;
}

void
Image::allocate(void)
{
	if(this->pixels == nil){
		this->stride = this->width*this->bpp;
		this->pixels = rwNewT(uint8, this->stride*this->height, MEMDUR_EVENT | ID_IMAGE);
		this->flags |= 1;
	}
	if(this->palette == nil){
		if(this->depth == 4 || this->depth == 8)
			this->palette = rwNewT(uint8, (1 << this->depth)*4, MEMDUR_EVENT | ID_IMAGE);
		this->flags |= 2;
	}
}

void
Image::free(void)
{
	if(this->flags&1){
		rwFree(this->pixels);
		this->pixels = nil;
	}
	if(this->flags&2){
		rwFree(this->palette);
		this->palette = nil;
	}
	this->flags = 0;
}

void
Image::setPixels(uint8 *pixels)
{
	this->pixels = pixels;
	this->flags |= 1;
}

void
decompressDXT1(uint8 *adst, int32 w, int32 h, uint8 *src)
{
	/* j loops through old texels
	 * x and y loop through new texels */
	int32 x = 0, y = 0;
	uint32 c[4][4];
	uint8 idx[16];
	uint8 (*dst)[4] = (uint8(*)[4])adst;
	for(int32 j = 0; j < w*h/2; j += 8){
		/* calculate colors */
		uint32 col0 = *((uint16*)&src[j+0]);
		uint32 col1 = *((uint16*)&src[j+2]);
		c[0][0] = ((col0>>11) & 0x1F)*0xFF/0x1F;
		c[0][1] = ((col0>> 5) & 0x3F)*0xFF/0x3F;
		c[0][2] = ( col0      & 0x1F)*0xFF/0x1F;
		c[0][3] = 0xFF;

		c[1][0] = ((col1>>11) & 0x1F)*0xFF/0x1F;
		c[1][1] = ((col1>> 5) & 0x3F)*0xFF/0x3F;
		c[1][2] = ( col1      & 0x1F)*0xFF/0x1F;
		c[1][3] = 0xFF;
		if(col0 > col1){
			c[2][0] = (2*c[0][0] + 1*c[1][0])/3;
			c[2][1] = (2*c[0][1] + 1*c[1][1])/3;
			c[2][2] = (2*c[0][2] + 1*c[1][2])/3;
			c[2][3] = 0xFF;

			c[3][0] = (1*c[0][0] + 2*c[1][0])/3;
			c[3][1] = (1*c[0][1] + 2*c[1][1])/3;
			c[3][2] = (1*c[0][2] + 2*c[1][2])/3;
			c[3][3] = 0xFF;
		}else{
			c[2][0] = (c[0][0] + c[1][0])/2;
			c[2][1] = (c[0][1] + c[1][1])/2;
			c[2][2] = (c[0][2] + c[1][2])/2;
			c[2][3] = 0xFF;

			c[3][0] = 0x00;
			c[3][1] = 0x00;
			c[3][2] = 0x00;
			c[3][3] = 0x00;
		}

		/* make index list */
		uint32 indices = *((uint32*)&src[j+4]);
		for(int32 k = 0; k < 16; k++){
			idx[k] = indices & 0x3;
			indices >>= 2;
		}

		/* write bytes */
		for(uint32 l = 0; l < 4; l++)
			for(uint32 k = 0; k < 4; k++){
				dst[(y+l)*w + x+k][0] = c[idx[l*4+k]][0];
				dst[(y+l)*w + x+k][1] = c[idx[l*4+k]][1];
				dst[(y+l)*w + x+k][2] = c[idx[l*4+k]][2];
				dst[(y+l)*w + x+k][3] = c[idx[l*4+k]][3];
			}
		x += 4;
		if(x >= w){
			y += 4;
			x = 0;
		}
	}
}

void
decompressDXT3(uint8 *adst, int32 w, int32 h, uint8 *src)
{
	/* j loops through old texels
	 * x and y loop through new texels */
	int32 x = 0, y = 0;
	uint32 c[4][4];
	uint8 idx[16];
	uint8 a[16];
	uint8 (*dst)[4] = (uint8(*)[4])adst;
	for(int32 j = 0; j < w*h; j += 16){
		/* calculate colors */
		uint32 col0 = *((uint16*)&src[j+8]);
		uint32 col1 = *((uint16*)&src[j+10]);
		c[0][0] = ((col0>>11) & 0x1F)*0xFF/0x1F;
		c[0][1] = ((col0>> 5) & 0x3F)*0xFF/0x3F;
		c[0][2] = ( col0      & 0x1F)*0xFF/0x1F;

		c[1][0] = ((col1>>11) & 0x1F)*0xFF/0x1F;
		c[1][1] = ((col1>> 5) & 0x3F)*0xFF/0x3F;
		c[1][2] = ( col1      & 0x1F)*0xFF/0x1F;

		c[2][0] = (2*c[0][0] + 1*c[1][0])/3;
		c[2][1] = (2*c[0][1] + 1*c[1][1])/3;
		c[2][2] = (2*c[0][2] + 1*c[1][2])/3;

		c[3][0] = (1*c[0][0] + 2*c[1][0])/3;
		c[3][1] = (1*c[0][1] + 2*c[1][1])/3;
		c[3][2] = (1*c[0][2] + 2*c[1][2])/3;

		/* make index list */
		uint32 indices = *((uint32*)&src[j+12]);
		for(int32 k = 0; k < 16; k++){
			idx[k] = indices & 0x3;
			indices >>= 2;
		}
		uint64 alphas = *((uint64*)&src[j+0]);
		for(int32 k = 0; k < 16; k++){
			a[k] = (alphas & 0xF)*17;
			alphas >>= 4;
		}

		/* write bytes */
		for(uint32 l = 0; l < 4; l++)
			for(uint32 k = 0; k < 4; k++){
				dst[(y+l)*w + x+k][0] = c[idx[l*4+k]][0];
				dst[(y+l)*w + x+k][1] = c[idx[l*4+k]][1];
				dst[(y+l)*w + x+k][2] = c[idx[l*4+k]][2];
				dst[(y+l)*w + x+k][3] = a[l*4+k];
			}
		x += 4;
		if(x >= w){
			y += 4;
			x = 0;
		}
	}
}

void
decompressDXT5(uint8 *adst, int32 w, int32 h, uint8 *src)
{
	/* j loops through old texels
	 * x and y loop through new texels */
	int32 x = 0, y = 0;
	uint32 c[4][4];
	uint32 a[8];
	uint8 idx[16];
	uint8 aidx[16];
	uint8 (*dst)[4] = (uint8(*)[4])adst;
	for(int32 j = 0; j < w*h; j += 16){
		/* calculate colors */
		uint32 col0 = *((uint16*)&src[j+8]);
		uint32 col1 = *((uint16*)&src[j+10]);
		c[0][0] = ((col0>>11) & 0x1F)*0xFF/0x1F;
		c[0][1] = ((col0>> 5) & 0x3F)*0xFF/0x3F;
		c[0][2] = ( col0      & 0x1F)*0xFF/0x1F;

		c[1][0] = ((col1>>11) & 0x1F)*0xFF/0x1F;
		c[1][1] = ((col1>> 5) & 0x3F)*0xFF/0x3F;
		c[1][2] = ( col1      & 0x1F)*0xFF/0x1F;
		if(col0 > col1){
			c[2][0] = (2*c[0][0] + 1*c[1][0])/3;
			c[2][1] = (2*c[0][1] + 1*c[1][1])/3;
			c[2][2] = (2*c[0][2] + 1*c[1][2])/3;

			c[3][0] = (1*c[0][0] + 2*c[1][0])/3;
			c[3][1] = (1*c[0][1] + 2*c[1][1])/3;
			c[3][2] = (1*c[0][2] + 2*c[1][2])/3;
		}else{
			c[2][0] = (c[0][0] + c[1][0])/2;
			c[2][1] = (c[0][1] + c[1][1])/2;
			c[2][2] = (c[0][2] + c[1][2])/2;

			c[3][0] = 0x00;
			c[3][1] = 0x00;
			c[3][2] = 0x00;
		}

		a[0] = src[j+0];
		a[1] = src[j+1];
		if(a[0] > a[1]){
			a[2] = (6*a[0] + 1*a[1])/7;
			a[3] = (5*a[0] + 2*a[1])/7;
			a[4] = (4*a[0] + 3*a[1])/7;
			a[5] = (3*a[0] + 4*a[1])/7;
			a[6] = (2*a[0] + 5*a[1])/7;
			a[7] = (1*a[0] + 6*a[1])/7;
		}else{
			a[2] = (4*a[0] + 1*a[1])/5;
			a[3] = (3*a[0] + 2*a[1])/5;
			a[4] = (2*a[0] + 3*a[1])/5;
			a[5] = (1*a[0] + 4*a[1])/5;
			a[6] = 0;
			a[7] = 0xFF;
		}

		/* make index list */
		uint32 indices = *((uint32*)&src[j+12]);
		for(int32 k = 0; k < 16; k++){
			idx[k] = indices & 0x3;
			indices >>= 2;
		}
		// only 6 indices
		uint64 alphas = *((uint64*)&src[j+2]);
		for(int32 k = 0; k < 16; k++){
			aidx[k] = alphas & 0x7;
			alphas >>= 3;
		}

		/* write bytes */
		for(uint32 l = 0; l < 4; l++)
			for(uint32 k = 0; k < 4; k++){
				dst[(y+l)*w + x+k][0] = c[idx[l*4+k]][0];
				dst[(y+l)*w + x+k][1] = c[idx[l*4+k]][1];
				dst[(y+l)*w + x+k][2] = c[idx[l*4+k]][2];
				dst[(y+l)*w + x+k][3] = a[aidx[l*4+k]];
			}
		x += 4;
		if(x >= w){
			y += 4;
			x = 0;
		}
	}
}

// not strictly image but related

// flip a DXT 2-bit block
static void
flipBlock(uint8 *dst, uint8 *src)
{
	// color
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
	// bits
	dst[4] = src[7];
	dst[5] = src[6];
	dst[6] = src[5];
	dst[7] = src[4];
}

// flip top 2 rows of a DXT 2-bit block
static void
flipBlock_half(uint8 *dst, uint8 *src)
{
	// color
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
	// bits
	dst[4] = src[5];
	dst[5] = src[4];
	dst[6] = src[6];
	dst[7] = src[7];
}

// flip a DXT3 4-bit alpha block
static void
flipAlphaBlock3(uint8 *dst, uint8 *src)
{
	dst[6] = src[0];
	dst[7] = src[1];
	dst[4] = src[2];
	dst[5] = src[3];
	dst[2] = src[4];
	dst[3] = src[5];
	dst[0] = src[6];
	dst[1] = src[7];
}

// flip top 2 rows of a DXT3 4-bit alpha block
static void
flipAlphaBlock3_half(uint8 *dst, uint8 *src)
{
	dst[0] = src[2];
	dst[1] = src[3];
	dst[2] = src[0];
	dst[3] = src[1];
	dst[4] = src[4];
	dst[5] = src[5];
	dst[6] = src[6];
	dst[7] = src[7];
}

// flip a DXT5 3-bit alpha block
static void
flipAlphaBlock5(uint8 *dst, uint8 *src)
{
	// color
	dst[0] = src[0];
	dst[1] = src[1];
	// bits
	uint64 bits = *(uint64*)&src[2];
	uint64 flipbits = 0;
	for(int i = 0; i < 4; i++){
		flipbits <<= 12;
		flipbits |= bits & 0xFFF;
		bits >>= 12;
	}
	memcpy(dst+2, &flipbits, 6);
}

// flip top 2 rows of a DXT5 3-bit alpha block
static void
flipAlphaBlock5_half(uint8 *dst, uint8 *src)
{
	// color
	dst[0] = src[0];
	dst[1] = src[1];
	// bits
	uint64 bits = *(uint64*)&src[2];
	uint64 flipbits = bits & 0xFFFFFF000000;
	flipbits |= (bits>>12) & 0xFFF;
	flipbits |= (bits<<12) & 0xFFF000;
	memcpy(dst+2, &flipbits, 6);
}

void
flipDXT1(uint8 *dst, uint8 *src, uint32 width, uint32 height)
{
	int x, y;
	int bw = (width+3)/4;
	int bh = (height+3)/4;
	if(height < 4){
		// used pixels are always at the top
		// so don't swap the full 4 rows
		if(height == 2){
			uint8 *s = src;
			uint8 *d = dst;
			for(x = 0; x < bw; x++){
				flipBlock_half(dst, src);
				s += 8;
				d += 8;
			}
		}else
			memcpy(dst, src, 8*bw);
		return;
	}
	dst += 8*bw*bh;
	for(y = 0; y < bh; y++){
		dst -= 8*bw;
		uint8 *s = src;
		uint8 *d = dst;
		for(x = 0; x < bw; x++){
			flipBlock(d, s);
			s += 8;
			d += 8;
		}
		src += 8*bw;
	}
}

void
flipDXT3(uint8 *dst, uint8 *src, uint32 width, uint32 height)
{
	int x, y;
	int bw = (width+3)/4;
	int bh = (height+3)/4;
	if(height < 4){
		// used pixels are always at the top
		// so don't swap the full 4 rows
		if(height == 2){
			uint8 *s = src;
			uint8 *d = dst;
			for(x = 0; x < bw; x++){
				flipAlphaBlock3_half(d, s);
				flipBlock_half(d+8, s+8);
				s += 16;
				d += 16;
			}
		}else
			memcpy(dst, src, 16*bw);
		return;
	}
	dst += 16*bw*bh;
	for(y = 0; y < bh; y++){
		dst -= 16*bw;
		uint8 *s = src;
		uint8 *d = dst;
		for(x = 0; x < bw; x++){
			flipAlphaBlock3(d, s);
			flipBlock(d+8, s+8);
			s += 16;
			d += 16;
		}
		src += 16*bw;
	}
}

void
flipDXT5(uint8 *dst, uint8 *src, uint32 width, uint32 height)
{
	int x, y;
	int bw = (width+3)/4;
	int bh = (height+3)/4;
	if(height < 4){
		// used pixels are always at the top
		// so don't swap the full 4 rows
		if(height == 2){
			uint8 *s = src;
			uint8 *d = dst;
			for(x = 0; x < bw; x++){
				flipAlphaBlock5_half(d, s);
				flipBlock_half(d+8, s+8);
				s += 16;
				d += 16;
			}
		}else
			memcpy(dst, src, 16*bw);
		return;
	}
	dst += 16*bw*bh;
	for(y = 0; y < bh; y++){
		dst -= 16*bw;
		uint8 *s = src;
		uint8 *d = dst;
		for(x = 0; x < bw; x++){
			flipAlphaBlock5(d, s);
			flipBlock(d+8, s+8);
			s += 16;
			d += 16;
		}
		src += 16*bw;
	}
}

void
flipDXT(int32 type, uint8 *dst, uint8 *src, uint32 width, uint32 height)
{
	switch(type){
	case 1:
		flipDXT1(dst, src, width, height);
		break;
	case 3:
		flipDXT3(dst, src, width, height);
		break;
	case 5:
		flipDXT5(dst, src, width, height);
		break;
	}
}

void
Image::setPixelsDXT(int32 type, uint8 *pixels)
{
	switch(type){
	case 1:
		decompressDXT1(this->pixels, this->width, this->height, pixels);
		break;
	case 3:
		decompressDXT3(this->pixels, this->width, this->height, pixels);
		break;
	case 5:
		decompressDXT5(this->pixels, this->width, this->height, pixels);
		break;
	}
}

void
Image::setPalette(uint8 *palette)
{
	this->palette = palette;
	this->flags |= 2;
}

void
Image::compressPalette(void)
{
	if(this->depth != 8)
		return;
	uint8 *pixels = this->pixels;
	for(int y = 0; y < this->height; y++){
		uint8 *line = pixels;
		for(int x = 0; x < this->width; x++){
			if(*line > 0xF) return;
			line += this->bpp;
		}
		pixels += this->stride;
	}
	this->depth = 4;
}

bool32
Image::hasAlpha(void)
{
	uint8 ret = 0xFF;
	uint8 *pixels = this->pixels;
	if(this->depth == 32){
		for(int y = 0; y < this->height; y++){
			uint8 *line = pixels;
			for(int x = 0; x < this->width; x++){
				ret &= line[3];
				line += this->bpp;
			}
			pixels += this->stride;
		}
	}else if(this->depth == 24){
		return 0;
	}else if(this->depth == 16){
		for(int y = 0; y < this->height; y++){
			uint8 *line = pixels;
			for(int x = 0; x < this->width; x++){
				ret &= line[1] & 0x80;
				line += this->bpp;
			}
			pixels += this->stride;
		}
		return ret != 0x80;
	}else if(this->depth <= 8){
		for(int y = 0; y < this->height; y++){
			uint8 *line = pixels;
			for(int x = 0; x < this->width; x++){
				ret &= this->palette[*line*4+3];
				line += this->bpp;
			}
			pixels += this->stride;
		}
	}
	return ret != 0xFF;
}

void
Image::convertTo32(void)
{
	assert(this->pixels);
	uint8 *pixels = this->pixels;
	int32 newstride = this->width*4;
	uint8 *newpixels;

	void (*fun)(uint8 *out, uint8 *in) = nil;
	switch(this->depth){
	case 4:
	case 8:
		assert(this->palette);
		this->unpalettize(true);
		return;
	case 16:
		fun = conv_RGBA8888_from_ARGB1555;
		break;
	case 24:
		fun = conv_RGBA8888_from_RGB888;
		break;
	default:
		return;
	}

	newpixels = rwNewT(uint8, newstride*this->height, MEMDUR_EVENT | ID_IMAGE);
	uint8 *pixels32 = newpixels;
	for(int y = 0; y < this->height; y++){
		uint8 *line = pixels;
		uint8 *newline = newpixels;
		for(int x = 0; x < this->width; x++){
			fun(newline, line);
			line += this->bpp;
			newline += 4;
		}
		pixels += this->stride;
		newpixels += newstride;
	}

	this->free();
	this->depth = 32;
	this->bpp = 4;
	this->stride = newstride;
	this->pixels = nil;
	this->palette = nil;
	this->setPixels(pixels32);
}

void
Image::palettize(int32 depth)
{
	RGBA colors[256];
	ColorQuant quant;
	uint8 *newpixels;
	uint32 newstride;

	quant.init();
	quant.addImage(this);
	assert(depth <= 8);
	quant.makePalette(1<<depth, colors);

	newstride = this->width;
	newpixels = rwNewT(uint8, newstride*this->height, MEMDUR_EVENT | ID_IMAGE);
	// TODO: maybe do floyd-steinberg dithering?
	quant.matchImage(newpixels, newstride, this);

	this->free();
	this->depth = depth;
	this->bpp = depth < 8 ? 1 : depth/8;
	this->stride = newstride;
	this->pixels = nil;
	this->palette = nil;
	this->setPixels(newpixels);
	this->allocate();
	memcpy(this->palette, colors, 4*(1<<depth));

	quant.destroy();
}

void
Image::unpalettize(bool forceAlpha)
{
	if(this->depth > 8)
		return;
	assert(this->pixels);
	assert(this->palette);

	int32 ndepth = (forceAlpha || this->hasAlpha()) ? 32 : 24;
	int32 nstride = this->width*ndepth/8;
	uint8 *npixels = rwNewT(uint8, nstride*this->height, MEMDUR_EVENT | ID_IMAGE);

	uint8 *line = this->pixels;
	uint8 *nline = npixels;
	uint8 *p, *np;
	for(int32 y = 0; y < this->height; y++){
		p = line;
		np = nline;
		for(int32 x = 0; x < this->width; x++){
			np[0] = this->palette[*p*4+0];
			np[1] = this->palette[*p*4+1];
			np[2] = this->palette[*p*4+2];
			np += 3;
			if(ndepth == 32)
				*np++ = this->palette[*p*4+3];
			p++;
		}
		line += this->stride;
		nline += nstride;
	}
	this->free();
	this->depth = ndepth;
	this->bpp = ndepth < 8 ? 1 : ndepth/8;
	this->stride = nstride;
	this->setPixels(npixels);
}

// Copy the biggest channel value to alpha
void
Image::makeMask(void)
{
	int32 maxcol;
	switch(this->depth){
	case 4:
	case 8: {
		assert(this->palette);
		int32 pallen = 1 << this->depth;
		for(int32 i = 0; i < pallen; i++){
			maxcol = this->palette[i*4+0];
			if(this->palette[i*4+1] > maxcol) maxcol = this->palette[i*4+1];
			if(this->palette[i*4+2] > maxcol) maxcol = this->palette[i*4+2];
			this->palette[i*4+3] = maxcol;
		}
		break;
	}

	case 16:
	case 24:
		this->convertTo32();
		// fallthrough

	case 32: {
		assert(this->pixels);
		uint8 *line = this->pixels;
		uint8 *p;
		for(int32 y = 0; y < this->height; y++){
			p = line;
			for(int32 x = 0; x < this->width; x++){
				maxcol = p[0];
				if(p[1] > maxcol) maxcol = p[1];
				if(p[2] > maxcol) maxcol = p[2];
				p[3] = maxcol;
				p += this->bpp;
			}
			line += this->stride;
		}
		break;
	}
	}
}

void
Image::applyMask(Image *mask)
{
	if(this->width != mask->width || this->height != mask->height)
		return;	// TODO: set an error
	// we could use alpha with 16 bits but what's the point?
	if(mask->depth == 16 || mask->depth == 24)
		return;

	this->convertTo32();
	assert(this->depth == 32);

	uint8 *line = this->pixels;
	uint8 *mline = mask->pixels;
	uint8 *p, *m;
	for(int32 y = 0; y < this->height; y++){
		p = line;
		m = mline;
		for(int32 x = 0; x < this->width; x++){
			if(mask->depth == 32)
				p[3] = m[3];
			else if(mask->depth <= 8)
				p[3] = mask->palette[m[0]*4+3];
			p += this->bpp;
			m += mask->bpp;
		}
		line += this->stride;
		mline += mask->stride;
	}
}

void
Image::removeMask(void)
{
	if(this->depth <= 8){
		assert(this->palette);
		int32 pallen = 4*(1 << this->depth);
		for(int32 i = 0; i < pallen; i += 4)
			this->palette[i+3] = 0xFF;
		return;
	}
	if(this->depth == 24)
		return;
	assert(this->pixels);
	uint8 *line = this->pixels;
	uint8 *p;
	for(int32 y = 0; y < this->height; y++){
		p = line;
		for(int32 x = 0; x < this->width; x++){
			switch(this->depth){
			case 16:
				p[1] |= 0x80;
				p += 2;
				break;
			case 32:
				p[3] = 0xFF;
				p += 4;
				break;
			}
		}
		line += this->stride;
	}
}

Image*
Image::extractMask(void)
{
	Image *img = Image::create(this->width, this->height, 8);
	img->allocate();

	// use an 8bit palette to store all shades of grey
	for(int32 i = 0; i < 256; i++){
		img->palette[i*4+0] = i;
		img->palette[i*4+1] = i;
		img->palette[i*4+2] = i;
		img->palette[i*4+3] = 0xFF;
	}

	// Then use the alpha value as palette index
	uint8 *line = this->pixels;
	uint8 *nline = img->pixels;
	uint8 *p, *np;
	for(int32 y = 0; y < this->height; y++){
		p = line;
		np = nline;
		for(int32 x = 0; x < this->width; x++){
			switch(this->depth){
			case 4:
			case 8:
				*np++ = this->palette[*p*4+3];
				p++;
				break;
			case 16:
				*np++ = 0xFF*!!(p[1]&0x80);
				p += 2;
				break;
			case 24:
				*np++ = 0xFF;
				p += 3;
				break;
			case 32:
				*np++ = p[3];
				p += 4;
				break;
			}
		}
		line += this->stride;
		nline += img->stride;
	}
	return img;
}

void
Image::setSearchPath(const char *path)
{
	char *p, *end;
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	rwFree(g->searchPaths);
	g->numSearchPaths = 0;
	if(path)
		g->searchPaths = p = rwStrdup(path, MEMDUR_EVENT);
	else{
		g->searchPaths = nil;
		return;
	}
	while(p && *p){
		end = strchr(p, ';');
		if(end)
			*end++ = '\0';
		g->numSearchPaths++;
		p = end;
	}
}

void
Image::printSearchPath(void)
{
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	char *p = g->searchPaths;
	for(int i = 0; i < g->numSearchPaths; i++){
		printf("%s\n", p);
		p += strlen(p) + 1;
	}
}

char*
Image::getFilename(const char *name)
{
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	FILE *f;
	char *s, *p = g->searchPaths;
	size_t len = strlen(name)+1;
	if(g->numSearchPaths == 0){
		s = rwStrdup(name, MEMDUR_EVENT);
		makePath(s);
		f = fopen(s, "rb");
		if(f){
			fclose(f);
			printf("found %s\n", s);
			return s;
		}
		rwFree(s);
		return nil;
	}else
		for(int i = 0; i < g->numSearchPaths; i++){
			s = (char*)rwMalloc(strlen(p)+len, MEMDUR_EVENT | ID_IMAGE);
			if(s == nil){
				RWERROR((ERR_ALLOC, strlen(p)+len));
				return nil;
			}
			strcpy(s, p);
			strcat(s, name);
			makePath(s);
			f = fopen(s, "r");
			if(f){
				fclose(f);
				printf("found %s\n", name);
				return s;
			}
			rwFree(s);
			p += strlen(p) + 1;
		}
	return nil;
}

Image*
Image::readMasked(const char *imageName, const char *maskName)
{
	Image *img, *mask;

	img = read(imageName);
	if(img == nil)
		return nil;
	if(maskName && maskName[0]){
		mask = read(maskName);
		if(mask == nil)
			return img;
		mask->makeMask();
		int32 origDepth = img->depth;
		img->applyMask(mask);
		mask->destroy();
		if(origDepth <= 8 && img->depth != origDepth)
			img->palettize(origDepth);
	}
	return img;
}

Image*
Image::read(const char *imageName)
{
	int i;
	char *filename, *ext, *found;
	Image *img;

	filename = rwNewT(char, strlen(imageName) + 20, MEMDUR_FUNCTION | ID_IMAGE);
	strcpy(filename, imageName);
	ext = filename + strlen(filename);
	*ext++ = '.';
	// Try all supported extensions
	for(i = 0; i < IMAGEGLOBAL(numFileFormats); i++){
		if(IMAGEGLOBAL(fileFormats)[i].read == nil)
			continue;
		strncpy(ext, IMAGEGLOBAL(fileFormats)[i].extension, 19);
		found = getFilename(filename);
		// Found a file
		if(found){
			img = IMAGEGLOBAL(fileFormats)[i].read(found);
			rwFree(found);
			// It was a valid image of that format
			if(img){
				rwFree(filename);
				return img;
			}
		}
	}
	rwFree(filename);
	return nil;
}

bool32
Image::registerFileFormat(const char *ext, fileRead read, fileWrite write)
{
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	if(g->numFileFormats >= (int)nelem(g->fileFormats))
		return 0;
	g->fileFormats[g->numFileFormats].extension = rwStrdup(ext, MEMDUR_EVENT);
	g->fileFormats[g->numFileFormats].read = read;
	g->fileFormats[g->numFileFormats].write = write;
	g->numFileFormats++;
	return 1;
}

static void*
imageOpen(void *object, int32 offset, int32 size)
{
	imageModuleOffset = offset;
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	g->searchPaths = nil;
	g->numSearchPaths = 0;
	g->numFileFormats = 0;
	return object;
}

static void*
imageClose(void *object, int32 offset, int32 size)
{
	ImageGlobals *g = PLUGINOFFSET(ImageGlobals, engine, imageModuleOffset);
	int i;
	rwFree(g->searchPaths);
	g->searchPaths = nil;
	g->numSearchPaths = 0;
	for(i = 0; i < g->numFileFormats; i++)
		rwFree(g->fileFormats[i].extension);
	g->numFileFormats = 0;
	return object;
}

void
Image::registerModule(void)
{
	Engine::registerPlugin(sizeof(ImageGlobals), ID_IMAGEMODULE, imageOpen, imageClose);
}



/*
 * Color Quantization
 */

// An address for a single level is 4 bits.
// Since we have 8 bpp that is 32 bits to address any tree node.
// The lower bits address the higher level tree nodes.
// This is essentially a bit reverse and swizzle.
static uint32
makeTreeAddr(RGBA color)
{
	int32 i;
	uint32 addr = 0;
	uint32 r = 1;
	uint32 g = 2;
	uint32 b = 4;
	uint32 a = 8;
	for(i = 0; i < 8; i++){
		uint32 mask = 0x80>>i;
		if(color.red & mask) addr |= r;
		if(color.green & mask) addr |= g;
		if(color.blue & mask) addr |= b;
		if(color.alpha & mask) addr |= a;
		r <<= 4;
		g <<= 4;
		b <<= 4;
		a <<= 4;
	}
	return addr;
}

void
ColorQuant::Node::destroy(void)
{
	int i;
	for(i = 0; i < 16; i++)
		if(this->children[i])
			this->children[i]->destroy();
	if(this->link.next)
		this->link.remove();
	rwFree(this);
}

ColorQuant::Node*
ColorQuant::createNode(int32 level)
{
	int i;
	ColorQuant::Node *node = rwNewT(ColorQuant::Node, 1, MEMDUR_EVENT | ID_IMAGE);
	node->parent = nil;
	for(i = 0; i < 16; i++)
		node->children[i] = nil;
	node->r = 0;
	node->g = 0;
	node->b = 0;
	node->a = 0;
	node->numPixels = 0;
	node->link.init();

	if(level == 0)
		this->leaves.append(&node->link);

	return node;
}

ColorQuant::Node*
ColorQuant::getNode(ColorQuant::Node *root, uint32 addr, int32 level)
{
	if(level == 0)
		return root;

	uint32 a = addr & 0xF;
	if(root->children[a] == nil){
		root->children[a] = this->createNode(level-1);
		root->children[a]->parent = root;
	}

	return this->getNode(root->children[a], addr>>4, level-1);
}

ColorQuant::Node*
ColorQuant::findNode(ColorQuant::Node *root, uint32 addr, int32 level)
{
	if(level == 0)
		return root;

	uint32 a = addr & 0xF;
	if(root->children[a] == nil)
		return root;

	return this->findNode(root->children[a], addr>>4, level-1);
}

void
ColorQuant::reduceNode(Node *node)
{
	int i;
	assert(node->numPixels == 0);
	for(i = 0; i < 16; i++)
		if(node->children[i]){
			node->r += node->children[i]->r;
			node->g += node->children[i]->g;
			node->b += node->children[i]->b;
			node->a += node->children[i]->a;
			node->numPixels += node->children[i]->numPixels;
			node->children[i]->destroy();
			node->children[i] = nil;
		}
	assert(node->link.next == nil);
	assert(node->link.prev == nil);
	this->leaves.append(&node->link);
}

void
ColorQuant::Node::addColor(RGBA color)
{
	this->r += color.red;
	this->g += color.green;
	this->b += color.blue;
	this->a += color.alpha;
	this->numPixels++;
}

void
ColorQuant::init(void)
{
	this->leaves.init();
	this->root = this->createNode(QUANTDEPTH);
}

void
ColorQuant::destroy(void)
{
	this->root->destroy();
}

void
ColorQuant::addColor(RGBA color)
{
	uint32 addr = makeTreeAddr(color);
	ColorQuant::Node *node = this->getNode(root, addr, QUANTDEPTH);
	node->addColor(color);
}

uint8
ColorQuant::findColor(RGBA color)
{
	uint32 addr = makeTreeAddr(color);
	ColorQuant::Node *node = this->findNode(root, addr, QUANTDEPTH);
	return node->numPixels;
}

void
ColorQuant::addImage(Image *img)
{
	RGBA col;
	uint8 rgba[4];
	uint8 *pixels = img->pixels;
	for(int y = 0; y < img->height; y++){
		uint8 *line = pixels;
		for(int x = 0; x < img->width; x++){
			uint8 *p = line;
			switch(img->depth){
			case 4: case 8:
				conv_RGBA8888_from_RGBA8888(rgba, &img->palette[p[0]*4]);
				break;
			case 32:
				conv_RGBA8888_from_RGBA8888(rgba, p);
				break;
			case 24:
				conv_RGBA8888_from_RGB888(rgba, p);
				break;
			case 16:
				conv_RGBA8888_from_ARGB1555(rgba, p);
				break;
			default: assert(0 && "invalid depth");
			}
			col.red = rgba[0];
			col.green = rgba[1];
			col.blue = rgba[2];
			col.alpha = rgba[3];
			this->addColor(col);
			line += img->bpp;
		}
		pixels += img->stride;
	}
}

void
ColorQuant::makePalette(int32 numColors, RGBA *colors)
{
	while(this->leaves.count() > numColors){
		Node *n = LLLinkGetData(this->leaves.link.next, Node, link);
		this->reduceNode(n->parent);
	}

	int i = 0;
	FORLIST(lnk, this->leaves){
		Node *n = LLLinkGetData(lnk, Node, link);
		n->r /= n->numPixels;
		n->g /= n->numPixels;
		n->b /= n->numPixels;
		n->a /= n->numPixels;
		colors[i].red = n->r;
		colors[i].green = n->g;
		colors[i].blue = n->b;
		colors[i].alpha = n->a;
		n->numPixels = i++;
	}
}

void
ColorQuant::matchImage(uint8 *dstPixels, uint32 dstStride, Image *img)
{
	RGBA col;
	uint8 rgba[4];
	uint8 *pixels = img->pixels;
	for(int y = 0; y < img->height; y++){
		uint8 *line = pixels;
		uint8 *dline = dstPixels;
		for(int x = 0; x < img->width; x++){
			uint8 *p = line;
			uint8 *d = dline;
			switch(img->depth){
			case 4: case 8:
				conv_RGBA8888_from_RGBA8888(rgba, &img->palette[p[0]*4]);
				break;
			case 32:
				conv_RGBA8888_from_RGBA8888(rgba, p);
				break;
			case 24:
				conv_RGBA8888_from_RGB888(rgba, p);
				break;
			case 16:
				conv_RGBA8888_from_ARGB1555(rgba, p);
				break;
			default: assert(0 && "invalid depth");
			}
			
			col.red = rgba[0];
			col.green = rgba[1];
			col.blue = rgba[2];
			col.alpha = rgba[3];
			*d = this->findColor(col);

			line += img->bpp;
			dline++;
		}
		pixels += img->stride;
		dstPixels += dstStride;
	}
}



}

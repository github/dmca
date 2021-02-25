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

#define PLUGIN_ID 0

namespace rw {

// NB: this has padding and cannot be streamed directly!
struct BMPheader
{
	uint16 magic;
	uint32 size;
	uint16 reserved[2];
	uint32 offset;

	bool32 read(Stream *stream);
	void write(Stream *stream);
};

// This one is aligned and can be streamed directly
struct DIBheader
{
	uint32 headerSize;
	int32 width;
	int32 height;
	int16 numPlanes;
	int16 depth;
	uint32 compression;
	uint32 imgSize;
	int32 hres;
	int32 vres;
	int32 paletteLen;
	int32 numImportant;
	// end of 40 btyes

	uint32 rmask, gmask, bmask, amask;
};

bool32
BMPheader::read(Stream *stream)
{
	magic = stream->readU16();
	size = stream->readU32();
	reserved[0] = stream->readU16();
	reserved[1] = stream->readU16();
	offset = stream->readU32();
	return magic == 0x4D42;
}

void
BMPheader::write(Stream *stream)
{

	stream->writeU16(magic);
	stream->writeU32(size);
	stream->writeU16(reserved[0]);
	stream->writeU16(reserved[1]);
	stream->writeU32(offset);
}

Image*
readBMP(const char *filename)
{
	ASSERTLITTLE;
	Image *image;
	uint32 length;
	uint8 *data;
	StreamMemory file;
	int i, x, y;

	bool32 noalpha;
	int pad;

	data = getFileContents(filename, &length);
	if(data == nil)
		return nil;
	file.open(data, length);

	/* read headers */
	BMPheader bmp;
	DIBheader dib;
	if(!bmp.read(&file))
		goto lose;
	file.read8(&dib, sizeof(dib));
	file.seek(dib.headerSize-sizeof(dib));	// skip the part of the header we're ignoring
	if(dib.headerSize <= 16){
		dib.compression = 0;
		dib.paletteLen = 0;
	}

	noalpha = true;

	// Recognize 32 bit formats
	if(dib.compression == 3){
		if(dib.rmask != 0xFF0000 ||
		   dib.gmask != 0x00FF00 ||
		   dib.bmask != 0x0000FF)
			goto lose;
		dib.compression = 0;
		if(dib.headerSize > 52 && dib.amask == 0xFF000000)
			noalpha = false;
	}

	if(dib.compression != 0)
		goto lose;

	image = Image::create(dib.width, dib.height, dib.depth);
	image->allocate();


	if(image->palette){
		int len = 1<<dib.depth;
		uint8 (*color)[4] = (uint8 (*)[4])image->palette;
		for(i = 0; i < len; i++){
			color[i][2] = file.readU8();	// blue
			color[i][1] = file.readU8();	// green
			color[i][0] = file.readU8();	// red
			color[i][3] = file.readU8();	// alpha
			if(noalpha)
				color[i][3] = 0xFF;
		}
	}

	file.seek(bmp.offset, 0);

	pad = image->width*image->bpp % 4;

	uint8 *px, *line;
	line = image->pixels + (image->height-1)*image->stride;
	for(y = 0; y < image->height; y++){
		px = line;
		for(x = 0; x < image->width; x++){
			switch(image->depth){
			case 4:
				i = file.readU8();;
				px[x+0] = (i>>4)&0xF;
				px[x+1] = i&0xF;
				x++;
				break;

			case 8:
				px[x] = file.readU8();
				break;

			case 16:
				// TODO: what format is this even? and what does Image expect?
				px[x*2 + 0] = file.readU8();
				px[x*2 + 1] = file.readU8();
				break;

			case 24:
				px[x*3 + 2] = file.readU8();
				px[x*3 + 1] = file.readU8();
				px[x*3 + 0] = file.readU8();
				break;

			case 32:
				px[x*4 + 2] = file.readU8();
				px[x*4 + 1] = file.readU8();
				px[x*4 + 0] = file.readU8();
				px[x*4 + 3] = file.readU8();
				if(noalpha)
					px[x*4 + 3] = 0xFF;
				break;

			default:
				goto lose;
			}
		}

		line -= image->stride;
		file.seek(pad);
	}

	
	file.close();
	rwFree(data);
	return image;

lose:
	file.close();
	rwFree(data);
	return nil;
}

/* can't write alpha */
void
writeBMP(Image *image, const char *filename)
{
	ASSERTLITTLE;
	uint8 *p;
	StreamFile file;
	if(!file.open(filename, "wb")){
		RWERROR((ERR_FILE, filename));
		return;
	}

	int32 pallen = image->depth > 8  ? 0 :
	               image->depth == 4 ? 16 : 256;
	int32 stride = image->width*image->depth/8;
	int32 depth = image->depth == 32 ? 24 : image->depth;
	stride = stride+3 & ~3;

	// File headers
	BMPheader bmp;
	bmp.magic = 0x4D42;	// BM
	bmp.size = 0x36 + 4*pallen + image->height*stride;
	bmp.reserved[0] = 0;
	bmp.reserved[1] = 0;
	bmp.offset = 0x36 + 4*pallen;
	bmp.write(&file);

	DIBheader dib;
	dib.headerSize = 0x28;
	dib.width = image->width;
	dib.height = image->height;
	dib.numPlanes = 1;
	dib.depth = depth;
	dib.compression = 0;
	dib.imgSize = 0;
	dib.hres = 2835;	// 72dpi
	dib.vres = 2835;	// 72dpi
	dib.paletteLen = 0;
	dib.numImportant = 0;
	file.write8(&dib, dib.headerSize);

	for(int i = 0; i < pallen; i++){
		file.writeU8(image->palette[i*4+2]);
		file.writeU8(image->palette[i*4+1]);
		file.writeU8(image->palette[i*4+0]);
		file.writeU8(0xFF);
	}

	uint8 *line = image->pixels + (image->height-1)*image->stride;
	int32 n;
	for(int y = 0; y < image->height; y++){
		p = line;
		for(int x = 0; x < image->width; x++){
			switch(image->depth){
			case 4:
				file.writeU8((p[0]&0xF)<<4 | (p[1]&0xF));
				p += 2;
				x++;
				break;
			case 8:
				file.writeU8(*p++);
				break;
			case 16:
				file.writeU8(p[0]);
				file.writeU8(p[1]);
				p += 2;
				break;
			case 24:
				file.writeU8(p[2]);
				file.writeU8(p[1]);
				file.writeU8(p[0]);
				p += 3;
				break;
			case 32:
				file.writeU8(p[2]);
				file.writeU8(p[1]);
				file.writeU8(p[0]);
				p += 4;
			}
		}
		n = (p-line) % 4;
		while(n--)
			file.writeU8(0);
		line -= image->stride;
	}

	file.close();
}

}

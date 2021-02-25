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
struct TGAHeader
{
	int8  IDlen;
	int8  colorMapType;
	int8  imageType;
	int16 colorMapOrigin;
	int16 colorMapLength;
	int8  colorMapDepth;
	int16 xOrigin, yOrigin;
	int16 width, height;
	uint8 depth;
	uint8 descriptor;

	void read(Stream *stream);
	void write(Stream *stream);
};

void
TGAHeader::read(Stream *stream)
{
	IDlen = stream->readI8();
	colorMapType = stream->readI8();
	imageType = stream->readI8();
	colorMapOrigin = stream->readI16();
	colorMapLength = stream->readI16();
	colorMapDepth = stream->readI8();
	xOrigin = stream->readI16();
	yOrigin = stream->readI16();
	width = stream->readI16();
	height = stream->readI16();
	depth = stream->readU8();
	descriptor = stream->readU8();
}

void
TGAHeader::write(Stream *stream)
{
	stream->writeI8(IDlen);
	stream->writeI8(colorMapType);
	stream->writeI8(imageType);
	stream->writeI16(colorMapOrigin);
	stream->writeI16(colorMapLength);
	stream->writeI8(colorMapDepth);
	stream->writeI16(xOrigin);
	stream->writeI16(yOrigin);
	stream->writeI16(width);
	stream->writeI16(height);
	stream->writeU8(depth);
	stream->writeU8(descriptor);
}

Image*
readTGA(const char *filename)
{
	TGAHeader header;
	Image *image;
	int depth = 0, palDepth = 0;
	uint32 length;
	uint8 *data = getFileContents(filename, &length);
	assert(data != nil);
	StreamMemory file;
	file.open(data, length);
	header.read(&file);

	assert(header.imageType == 1 || header.imageType == 2);
	file.seek(header.IDlen);
	if(header.colorMapType){
		assert(header.colorMapOrigin == 0);
		depth = (header.colorMapLength <= 16) ? 4 : 8;
		palDepth = header.colorMapDepth;
		assert(palDepth == 24 || palDepth == 32);
	}else{
		depth = header.depth;
		assert(depth == 16 || depth == 24 || depth == 32);
	}

	image = Image::create(header.width, header.height, depth);
	image->allocate();
	uint8 *palette = header.colorMapType ? image->palette : nil;
	uint8 (*color)[4] = nil;
	if(palette){
		int maxlen = depth == 4 ? 16 : 256;
		color = (uint8(*)[4])palette;
		int i;
		for(i = 0; i < header.colorMapLength; i++){
			color[i][2] = file.readU8();
			color[i][1] = file.readU8();
			color[i][0] = file.readU8();
			color[i][3] = 0xFF;
			if(palDepth == 32)
				color[i][3] = file.readU8();
		}
		for(; i < maxlen; i++){
			color[i][0] = color[i][1] = color[i][2] = 0;
			color[i][3] = 0xFF;
		}
	}

	uint8 *pixels = image->pixels;
	if(!(header.descriptor & 0x20))
		pixels += (image->height-1)*image->stride;
	for(int y = 0; y < image->height; y++){
		uint8 *line = pixels;
		for(int x = 0; x < image->width; x++){
			switch(image->depth){
			case 4:
			case 8:
				line[0] = file.readU8();
				break;
			case 16:
				line[0] = file.readU8();
				line[1] = file.readU8();
				break;
			case 24:
				line[2] = file.readU8();
				line[1] = file.readU8();
				line[0] = file.readU8();
				break;
			case 32:
				line[2] = file.readU8();
				line[1] = file.readU8();
				line[0] = file.readU8();
				line[3] = file.readU8();
				break;
			}
			line += image->bpp;
		}
		pixels += (header.descriptor&0x20) ?
		              image->stride : -image->stride;
	}

	file.close();
	rwFree(data);
	return image;
}

void
writeTGA(Image *image, const char *filename)
{
	TGAHeader header;
	StreamFile file;
	if(!file.open(filename, "wb")){
		RWERROR((ERR_FILE, filename));
		return;
	}
	header.IDlen = 0;
	header.imageType = image->palette != nil ? 1 : 2;
	header.colorMapType = image->palette != nil;
	header.colorMapOrigin = 0;
	header.colorMapLength = image->depth == 4 ? 16 :
	                        image->depth == 8 ? 256 : 0;
	header.colorMapDepth = image->palette ? 32 : 0;
	header.xOrigin = 0;
	header.yOrigin = 0;
	header.width = image->width;
	header.height = image->height;
	header.depth = image->depth == 4 ? 8 : image->depth;
	header.descriptor = 0x20 | (image->depth == 32 ? 8 : 0);
	header.write(&file);

	uint8 *palette = header.colorMapType ? image->palette : nil;
	uint8 (*color)[4] = (uint8(*)[4])palette;;
	if(palette)
		for(int i = 0; i < header.colorMapLength; i++){
			file.writeU8(color[i][2]);
			file.writeU8(color[i][1]);
			file.writeU8(color[i][0]);
			file.writeU8(color[i][3]);
		}

	uint8 *line = image->pixels;
	uint8 *p;
	for(int y = 0; y < image->height; y++){
		p = line;
		for(int x = 0; x < image->width; x++){
			switch(image->depth){
			case 4:
			case 8:
				file.writeU8(p[0]);
				break;
			case 16:
				file.writeU8(p[0]);
				file.writeU8(p[1]);
				break;
			case 24:
				file.writeU8(p[2]);
				file.writeU8(p[1]);
				file.writeU8(p[0]);
				break;
			case 32:
				file.writeU8(p[2]);
				file.writeU8(p[1]);
				file.writeU8(p[0]);
				file.writeU8(p[3]);
				break;
			}
			p += image->bpp;
		}
		line += image->stride;
	}
	file.close();
}

}

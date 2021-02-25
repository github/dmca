#include <rw.h>
#include <skeleton.h>

using namespace rw;

struct Font
{
	Texture *tex;
	int32 glyphwidth, glyphheight;
	int32 numglyphs;
};
Font vga = { nil, 8, 16, 256 };
Font bios = { nil, 8, 8, 256 };
Font *curfont = &bios;

#define NUMCHARS 100
uint16 indices[NUMCHARS*6];
RWDEVICE::Im2DVertex vertices[NUMCHARS*4];
int32 curVert;
int32 curIndex;

void
printScreen(const char *s, float32 x, float32 y)
{
	char c;
	Camera *cam;
	RWDEVICE::Im2DVertex *vert;
	uint16 *ix;
	curVert = 0;
	curIndex = 0;
	float32 u, v, du, dv;
	float recipZ;

	cam = (Camera*)engine->currentCamera;
	vert = &vertices[curVert];
	ix = &indices[curIndex];
	du = curfont->glyphwidth/(float32)curfont->tex->raster->width;
	dv = curfont->glyphheight/(float32)curfont->tex->raster->height;
	recipZ = 1.0f/cam->nearPlane;
	while(c = *s){
		if(c >= curfont->numglyphs)
			c = 0;
		u = (c % 16)*curfont->glyphwidth / (float32)curfont->tex->raster->width;
		v = (c / 16)*curfont->glyphheight / (float32)curfont->tex->raster->height;

		vert->setScreenX(x);
		vert->setScreenY(y);
		vert->setScreenZ(rw::im2d::GetNearZ());
		vert->setCameraZ(cam->nearPlane);
		vert->setRecipCameraZ(recipZ);
		vert->setColor(255, 255, 255, 255);
		vert->setU(u, recipZ);
		vert->setV(v, recipZ);
		vert++;

		vert->setScreenX(x+curfont->glyphwidth);
		vert->setScreenY(y);
		vert->setScreenZ(rw::im2d::GetNearZ());
		vert->setCameraZ(cam->nearPlane);
		vert->setRecipCameraZ(recipZ);
		vert->setColor(255, 255, 255, 255);
		vert->setU(u+du, recipZ);
		vert->setV(v, recipZ);
		vert++;
		
		vert->setScreenX(x);
		vert->setScreenY(y+curfont->glyphheight);
		vert->setScreenZ(rw::im2d::GetNearZ());
		vert->setCameraZ(cam->nearPlane);
		vert->setRecipCameraZ(recipZ);
		vert->setColor(255, 255, 255, 255);
		vert->setU(u, recipZ);
		vert->setV(v+dv, recipZ);
		vert++;

		vert->setScreenX(x+curfont->glyphwidth);
		vert->setScreenY(y+curfont->glyphheight);
		vert->setScreenZ(rw::im2d::GetNearZ());
		vert->setCameraZ(cam->nearPlane);
		vert->setRecipCameraZ(recipZ);
		vert->setColor(255, 255, 255, 255);
		vert->setU(u+du, recipZ);
		vert->setV(v+dv, recipZ);
		vert++;

		*ix++ = curVert;
		*ix++ = curVert+1;
		*ix++ = curVert+2;
		*ix++ = curVert+2;
		*ix++ = curVert+1;
		*ix++ = curVert+3;

		curVert += 4;
		curIndex += 6;
		x += curfont->glyphwidth+1;
		
		s++;
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, curfont->tex->raster);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);

	im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST,
		vertices, curVert, indices, curIndex);

}

void
initFont(void)
{
	vga.tex = Texture::read("Bm437_IBM_VGA8", "");
	bios.tex = Texture::read("Bm437_IBM_BIOS", "");

/*
	FILE *foo = fopen("font.c", "w");
	assert(foo);
	int x, y;
	rw::Image *img = rw::readTGA("vga_font.tga");
	assert(img);
	for(y = 0; y < img->height; y++){
		for(x = 0; x < img->width; x++)
			fprintf(foo, "%d, ", !!img->pixels[y*img->width + x]);
		fprintf(foo, "\n");
	}
*/
}

/*
#define NUMGLYPHS 256
#define GLYPHWIDTH 8
#define GLYPHHEIGHT 16


void
convertFont(void)
{
	FILE *f;
	Image *img;
	uint8 data[NUMGLYPHS*GLYPHHEIGHT];
	int32 i, x, y;
	uint8 *px, *line, *glyph;
//	f = fopen("font0.bin", "rb");
	f = fopen("Bm437_IBM_VGA8.FON", "rb");
//	f = fopen("Bm437_IBM_BIOS.FON", "rb");
	if(f == nil)
		return;
fseek(f, 0x65A, 0);
	fread(data, 1, NUMGLYPHS*GLYPHHEIGHT, f);
	fclose(f);

	img = Image::create(16*GLYPHWIDTH, NUMGLYPHS/16*GLYPHHEIGHT, 32);
	img->allocate();
	for(i = 0; i < NUMGLYPHS; i++){
		glyph = &data[i*GLYPHHEIGHT];
		x = (i % 16)*GLYPHWIDTH;
		y = (i / 16)*GLYPHHEIGHT;
		line = &img->pixels[x*4 + y*img->stride];
		for(y = 0; y < GLYPHHEIGHT; y++){
			px = line;
			for(x = 0; x < 8; x++){
				if(*glyph & 1<<(8-x)){
					*px++ = 255;
					*px++ = 255;
					*px++ = 255;
					*px++ = 255;
				}else{
					*px++ = 0;
					*px++ = 0;
					*px++ = 0;
					*px++ = 0;
				}
			}
			glyph++;
			line += img->stride;
		}
	}
//	writeTGA(img, "Bm437_IBM_BIOS.tga");
	writeTGA(img, "Bm437_IBM_VGA8.tga");
}
*/
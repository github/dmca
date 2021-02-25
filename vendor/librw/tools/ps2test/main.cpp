#include <cstdio>
#include <cassert>

#define PAL

#include <rw.h>
using rw::uint8;
using rw::uint16;
using rw::uint32;
using rw::uint64;
using rw::int8;
using rw::int16;
using rw::int32;
using rw::int64;
using rw::bool32;
using rw::float32;
typedef uint8 uchar;
typedef uint16 ushort;
typedef uint32 uint;

#define WIDTH 640
#ifdef PAL
#define HEIGHT 512
#define VMODE GS_PAL
#else
#define HEIGHT 448
#define VMODE GS_NTSC
#endif

#include "ps2.h"

// getting undefined references otherwise :/
int *__errno() { return &errno; }

// NONINTERLACED and FRAME have half of the FIELD vertical resolution!
// NONINTERLACED has half the vertical units

uint128 packetbuf[128];
uint128 vuXYZScale;
uint128 vuXYZOffset;
extern uint32 geometryCall[];
extern uint32 skinPipe[];

uint128 *curVifPtr;
uint128 lightpacket[128];
int32 numLightQ;



rw::World *world;
rw::Camera *camera;


int frames;

void
printquad(uint128 p)
{
	uint64 *lp;
	lp = (uint64*)&p;
	printf("%016lx %016lx\n", lp[1], lp[0]);
}

void
printquad4(uint128 p)
{
	uint32 *lp;
	lp = (uint32*)&p;
	printf("%08x %08x %08x %08x\n", lp[0], lp[1], lp[2], lp[3]);
}

void
dump4(uint128 *p, int n)
{
printf("data at %p\n", p);
	while(n--)
		printquad4(*p++);
}

struct DmaChannel {
	uint32 chcr; uint32 pad0[3];
	uint32 madr; uint32 pad1[3];
	uint32 qwc;  uint32 pad2[3];
	uint32 tadr; uint32 pad3[3];
	uint32 asr0; uint32 pad4[3];
	uint32 asr1; uint32 pad5[3];
	uint32 pad6[8];
	uint32 sadr;
};

static struct DmaChannel *dmaChannels[] = {
	(struct DmaChannel *) &D0_CHCR,
	(struct DmaChannel *) &D1_CHCR,
	(struct DmaChannel *) &D2_CHCR,
	(struct DmaChannel *) &D3_CHCR,
	(struct DmaChannel *) &D4_CHCR,
	(struct DmaChannel *) &D5_CHCR,
	(struct DmaChannel *) &D6_CHCR,
	(struct DmaChannel *) &D7_CHCR,
	(struct DmaChannel *) &D8_CHCR,
	(struct DmaChannel *) &D9_CHCR
};

void
dmaReset(void)
{
	/* don't clear the SIF channels */
	int doclear[] = { 1, 1, 1, 1, 1, 0, 0, 0, 1, 1 };
	int i;

	D_CTRL = 0;
	for(i = 0; i < 10; i++)
		if(doclear[i]){
			dmaChannels[i]->chcr = 0;
			dmaChannels[i]->madr = 0;
			dmaChannels[i]->qwc = 0;
			dmaChannels[i]->tadr = 0;
			dmaChannels[i]->asr0 = 0;
			dmaChannels[i]->asr1 = 0;
			dmaChannels[i]->sadr = 0;
		}
	D_CTRL = 1;
}

void
waitDMA(volatile uint32 *chcr)
{
	while(*chcr & (1<<8));
}

void
qwcpy(uint128 *dst, uint128 *src, int n)
{
	while(n--) *dst++ = *src++;
}

void
toGIF(void *src, int n)
{
	FlushCache(0);
	D2_QWC = n;
	D2_MADR = (uint32)src;
	D2_CHCR = 1<<8;
	waitDMA(&D2_CHCR);
}

void
toGIFchain(void *src)
{
	FlushCache(0);
	D2_QWC = 0;
	D2_TADR = (uint32)src & 0x0FFFFFFF;
	D2_CHCR = 1<<0 | 1<<2 | 1<<6 | 1<<8;
	waitDMA(&D2_CHCR);
}

void
toVIF1chain(void *src)
{
	FlushCache(0);
	D1_QWC = 0;
	D1_TADR = (uint32)src & 0x0FFFFFFF;
	D1_CHCR = 1<<0 | 1<<2 | 1<<6 | 1<<8;
	waitDMA(&D1_CHCR);
}


GsCrtState gsCrtState;

int psmsizemap[64] = {
	4,	// GS_PSMCT32
	4,	// GS_PSMCT24
	2,	// GS_PSMCT16
	0, 0, 0, 0, 0, 0, 0,
	2,	// GS_PSMCT16S
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	4,	// GS_PSMZ32
	4,	// GS_PSMZ24
	2,	// GS_PSMZ16
	2,	// GS_PSMZ16S
	0, 0, 0, 0, 0
};

void
GsResetCrt(uchar inter, uchar mode, uchar ff)
{
	gsCrtState.inter = inter;
	gsCrtState.mode = mode;
	gsCrtState.ff = ff;
	GS_CSR = 1 << GS_CSR_RESET_O;
	__asm__("sync.p; nop");
	GsPutIMR(0xff00);
	SetGsCrt(gsCrtState.inter, gsCrtState.mode, gsCrtState.ff);
}

uint gsAllocPtr = 0;

void
GsInitDispCtx(GsDispCtx *disp, int width, int height, int psm)
{
	int magh, magv;
	int dx, dy;
	int dw, dh;

	dx = gsCrtState.mode == GS_NTSC ? 636 : 656;
	dy = gsCrtState.mode == GS_NTSC ? 25 : 36;
	magh = 2560/width - 1;
	magv = 0;
	dw = 2560-1;
	dh = height-1;

	if(gsCrtState.inter == GS_INTERLACED){
		dy *= 2;
		if(gsCrtState.ff == GS_FRAME)
			dh = (dh+1)*2-1;
	}

	disp->pmode.d = GS_MAKE_PMODE(0, 1, 1, 1, 0, 0x00);
//	disp->bgcolor.d = 0x404040;
	disp->bgcolor.d = 0x000000;
	disp->dispfb1.d = 0;
	disp->dispfb2.d = GS_MAKE_DISPFB(0, width/64, psm, 0, 0);
	disp->display1.d = 0;
	disp->display2.d = GS_MAKE_DISPLAY(dx, dy, magh, magv, dw, dh);
}

void
GsPutDispCtx(GsDispCtx *disp)
{
	GS_PMODE = disp->pmode.d;
	GS_DISPFB1 = disp->dispfb1.d;
	GS_DISPLAY1 = disp->display1.d;
	GS_DISPFB2 = disp->dispfb2.d;
	GS_DISPLAY2 = disp->display2.d;
	GS_BGCOLOR = disp->bgcolor.d;
}

void
GsInitDrawCtx(GsDrawCtx *draw, int width, int height, int psm, int zpsm)
{
	MAKE128(draw->gifTag, 0xe,
		GIF_MAKE_TAG(8, 1, 0, 0, GIF_PACKED, 1));
	draw->frame1.d = GS_MAKE_FRAME(0, width/64, psm, 0);
	draw->ad_frame1 = GS_FRAME_1;
	draw->frame2.d = draw->frame1.d;
	draw->ad_frame2 = GS_FRAME_2;
	draw->zbuf1.d = GS_MAKE_ZBUF(0, zpsm, 0);
	draw->ad_zbuf1 = GS_ZBUF_1;
	draw->zbuf2.d = draw->zbuf1.d;
	draw->ad_zbuf2 = GS_ZBUF_2;
	draw->xyoffset1.d = GS_MAKE_XYOFFSET(2048<<4, 2048<<4);
	draw->ad_xyoffset1 = GS_XYOFFSET_1;
	draw->xyoffset2.d = draw->xyoffset1.d;
	draw->ad_xyoffset2 = GS_XYOFFSET_2;
	draw->scissor1.d = GS_MAKE_SCISSOR(0, width-1, 0, height-1);
	draw->ad_scissor1 = GS_SCISSOR_1;
	draw->scissor2.d = draw->scissor1.d;
	draw->ad_scissor2 = GS_SCISSOR_2;
}

void
GsPutDrawCtx(GsDrawCtx *draw)
{
//	printquad(*(uint128*)&draw->frame1);
	toGIF(draw, 9);
}

void
GsInitCtx(GsCtx *ctx, int width, int height, int psm, int zpsm)
{
	uint fbsz, zbsz;
	uint fbp, zbp;
	fbsz = (width*height*psmsizemap[psm] + 2047)/2048;
	zbsz = (width*height*psmsizemap[0x30|zpsm] + 2047)/2048;
	gsAllocPtr = 2*fbsz + zbsz;
	fbp = fbsz;
	zbp = fbsz*2;

	GsInitDispCtx(&ctx->disp[0], width, height, psm);
	GsInitDispCtx(&ctx->disp[1], width, height, psm);
	GsInitDrawCtx(&ctx->draw[0], width, height, psm, zpsm);
	GsInitDrawCtx(&ctx->draw[1], width, height, psm, zpsm);
	ctx->disp[1].dispfb2.f.FBP = fbp/4;
	ctx->draw[0].frame1.f.FBP = fbp/4;
	ctx->draw[0].frame2.f.FBP = fbp/4;
	ctx->draw[0].zbuf1.f.ZBP = zbp/4;
	ctx->draw[0].zbuf2.f.ZBP = zbp/4;
	ctx->draw[1].zbuf1.f.ZBP = zbp/4;
	ctx->draw[1].zbuf2.f.ZBP = zbp/4;
}

void
initrender(void)
{
	uint128 *p, tmp;
	p = packetbuf;
	MAKE128(tmp, 0xe, GIF_MAKE_TAG(2, 1, 0, 0, GIF_PACKED, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_PRMODECONT, 1);
	*p++ = tmp;
	MAKE128(tmp, GS_COLCLAMP, 1);
	*p++ = tmp;
	toGIF(packetbuf, 3);
}

void
clearscreen(int r, int g, int b)
{
	int x, y;
	uint128 *p, tmp;
	p = packetbuf;

	x = (2048 + WIDTH)<<4;
	y = (2048 + HEIGHT)<<4;

	MAKE128(tmp, 0xe, GIF_MAKE_TAG(5, 1, 0, 0, GIF_PACKED, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_TEST_1, GS_MAKE_TEST(0, 0, 0, 0, 0, 0, 1, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_PRIM, GS_MAKE_PRIM(GS_PRIM_SPRITE,0,0,0,0,0,0,0,0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(r, g, b, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(2048<<4, 2048<<4, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x, y, 0));
	*p++ = tmp;
	toGIF(packetbuf, 6);
}

void
drawtest(void)
{
	int x0, x1, x2, x3;
	int y0, y1, y2;
	uint128 *p, tmp;
	int n;

	x0 = 2048<<4;
	x1 = (2048 + 210)<<4;
	x2 = (2048 + 430)<<4;
	x3 = (2048 + 640)<<4;
	y0 = 2048<<4;
	y1 = (2048 + 224)<<4;
	y2 = (2048 + 448)<<4;

	n = 2 + 3*7;
	p = packetbuf;
	MAKEQ(tmp, 0x70000000 | n+1, 0, 0, 0);
	*p++ = tmp;
	MAKE128(tmp, 0xe, GIF_MAKE_TAG(n, 1, 0, 0, GIF_PACKED, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_TEST_1, GS_MAKE_TEST(0, 0, 0, 0, 0, 0, 1, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_PRIM, GS_MAKE_PRIM(GS_PRIM_SPRITE,0,0,0,0,0,0,0,0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(255, 0, 0, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x0, y0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x1, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(0, 255, 0, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x1, y0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x2, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(0, 0, 255, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x2, y0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x3, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(0, 255, 255, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x0, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x1, y2, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(255, 0, 255, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x1, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x2, y2, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(255, 255, 0, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x2, y1, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x3, y2, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_RGBAQ, GS_MAKE_RGBAQ(255, 255, 255, 0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ((2048+20)<<4, y0, 0));
	*p++ = tmp;
	MAKE128(tmp, GS_XYZ2, GS_MAKE_XYZ(x3, (2048+20)<<4, 0));
	*p++ = tmp;
	toGIFchain(packetbuf);
}

void
drawtri(void)
{
	uint128 *p, tmp;
	uint32 *ip;
	int nverts, n;

	nverts = 3;
	n = 2*nverts;
	p = packetbuf;
	MAKEQ(tmp, 0x70000000 | n+1, 0, 0, 0);
	*p++ = tmp;
	MAKE128(tmp, (0x5<<4) | 0x1,
	        GIF_MAKE_TAG(nverts, 1, 1, GS_MAKE_PRIM(GS_PRIM_TRI, 1, 0, 0, 0, 0, 0, 0, 0), GIF_PACKED, 2));
	*p++ = tmp;
	MAKEQ(tmp, 255, 0, 0, 0);
	*p++ = tmp;
	MAKEQ(tmp, (2048+85)<<4, (2048+70)<<4, 0, 0);
	*p++ = tmp;
	MAKEQ(tmp, 0, 255, 0, 0);
	*p++ = tmp;
	MAKEQ(tmp, (2048+260)<<4, (2048+200)<<4, 0, 0);
	*p++ = tmp;
	MAKEQ(tmp, 0, 0, 255, 0);
	*p++ = tmp;
	MAKEQ(tmp, (2048+180)<<4, (2048+350)<<4, 0, 0);
	*p++ = tmp;
	toGIFchain(packetbuf);
}

void
printMatrix(rw::Matrix *m)
{
	rw::V3d *x = &m->right;
	rw::V3d *y = &m->up;
	rw::V3d *z = &m->at;
	rw::V3d *w = &m->pos;
        printf(
                  "[ [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ] ]\n"
                  "  %08x == flags\n",
		  x->x, y->x, z->x, w->x,
		  x->y, y->y, z->y, w->y,
		  x->z, y->z, z->z, w->z,
		  0.0f, 0.0f, 0.0f, 1.0f,
                  m->flags);
}

// This is not proper data, just for testing
void
setupLight(rw::Atomic *atomic)
{
	using namespace rw;
	Matrix *lightmat;
	float32 *lp;

	numLightQ = 0;
	lp = (float32*)lightpacket;

	// TODO: this is the wrong matrix. we actually want to
	// transform the light, not all normals.
	lightmat = atomic->getFrame()->getLTM();
	*lp++ = lightmat->right.x;
	*lp++ = lightmat->right.y;
	*lp++ = lightmat->right.z;
	*lp++ = 0.0f;
	*lp++ = lightmat->up.x;
	*lp++ = lightmat->up.y;
	*lp++ = lightmat->up.z;
	*lp++ = 0.0f;
	*lp++ = lightmat->at.x;
	*lp++ = lightmat->at.y;
	*lp++ = lightmat->at.z;
	*lp++ = 0.0f;
	*lp++ = lightmat->pos.x;
	*lp++ = lightmat->pos.y;
	*lp++ = lightmat->pos.z;
	*lp++ = 1.0f;
	// TODO: make a proper light block
	// ambient
	*lp++ = 80.0f;
	*lp++ = 80.0f;
	*lp++ = 80.0f;
	*lp++ = 0.0f;
	// directional
	*lp++ = 0.5f;
	*lp++ = -0.5f;
	*lp++ = -0.7071f;
	*lp++ = 0.0f;
	numLightQ = 6;
}

void
setupTransform(rw::Atomic *atomic, rw::Matrix *trans)
{
	rw::Matrix::mult(trans, atomic->getFrame()->getLTM(), &camera->viewMatrix);
}

enum {
	DMAcnt       = 0x10000000,
	DMAref       = 0x30000000,
	DMAcall      = 0x50000000,
	DMAret       = 0x60000000,
	DMAend       = 0x70000000,

	V4_32 = 0x6C
};

#define UNPACK(type, nq, offset) ((type)<<24 | (nq)<<16 | (offset))
#define STCYCL(WL,CL) (0x01000000 | (WL)<<8 | (CL))

void
drawAtomic(rw::Atomic *atomic)
{
	using namespace rw;

	Matrix trans;
	Geometry *geo;
	ps2::ObjPipeline *pipe;
	ps2::MatPipeline *matpipe;
	Material *material;
	uint128 tmp, *lp;
	uint32 *vec;
	RGBAf color;
	int i;

	geo = atomic->geometry;
	pipe = (ps2::ObjPipeline*)atomic->getPipeline();
	if(pipe->platform != PLATFORM_PS2)
		return;

	setupLight(atomic);
	setupTransform(atomic, &trans);

	curVifPtr = packetbuf;
	// upload lights
	MAKEQ(tmp, DMAcnt | numLightQ+8, 0, STCYCL(4,4), UNPACK(V4_32, numLightQ, 0x3d0));
	*curVifPtr++ = tmp;
	for(lp = lightpacket; numLightQ--;)
		*curVifPtr++ = *lp++;

	// upload transformation matrix
	MAKEQ(tmp, 0, 0, STCYCL(4,4), UNPACK(V4_32, 4, 0x3f0));
	*curVifPtr++ = tmp;
	vec = (uint32*)&trans.right;
	MAKEQ(tmp, vec[0], vec[1], vec[2], vec[2]);
	*curVifPtr++ = tmp;
	vec = (uint32*)&trans.up;
	MAKEQ(tmp, vec[0], vec[1], vec[2], vec[2]);
	*curVifPtr++ = tmp;
	vec = (uint32*)&trans.at;
	MAKEQ(tmp, vec[0], vec[1], vec[2], vec[2]);
	*curVifPtr++ = tmp;
	vec = (uint32*)&trans.pos;
	MAKEQ(tmp, vec[0], vec[1], vec[2], vec[2]);
	*curVifPtr++ = tmp;

	// upload camera/screen info
	MAKEQ(tmp, 0, 0, STCYCL(4,4), UNPACK(V4_32, 2, 0x3f7));
	*curVifPtr++ = tmp;
	*curVifPtr++ = vuXYZScale;
	*curVifPtr++ = vuXYZOffset;

	assert(geo->instData != NULL);
	rw::ps2::InstanceDataHeader *instData =
	  (rw::ps2::InstanceDataHeader*)geo->instData;
	rw::MeshHeader *meshHeader = geo->meshHeader;
	rw::Mesh *mesh;
	for(i = 0; i < instData->numMeshes; i++){
		material = instData->instanceMeshes[i].material;
		matpipe = pipe->groupPipeline;
		if(matpipe == nil)
			matpipe = (ps2::MatPipeline*)material->pipeline;
		if(matpipe == nil)
			matpipe = ps2::defaultMatPipe;

		// call vu code
		MAKEQ(tmp, DMAcall, (uint32)skinPipe, 0, 0);
		*curVifPtr++ = tmp;
		// unpack GIF tag, material color, surface properties
		MAKEQ(tmp, DMAcnt | 3, 0, STCYCL(4,4), UNPACK(V4_32, 3, 0x3fa));
		*curVifPtr++ = tmp;
		MAKE128(tmp, 0x412,
			GIF_MAKE_TAG(0, 1, 1, GS_MAKE_PRIM(GS_PRIM_TRI_STRIP,1,0,0,0,0,0,0,0), GIF_PACKED, 3));
		*curVifPtr++ = tmp;
		convColor(&color, &material->color);
		color.alpha *= 128.0f/255.0f;
		MAKEQ(tmp, *(uint32*)&color.red, *(uint32*)&color.green,
			*(uint32*)&color.blue, *(uint32*)&color.alpha);
		*curVifPtr++ = tmp;
		MAKEQ(tmp, *(uint32*)&material->surfaceProps.ambient,
			*(uint32*)&material->surfaceProps.specular,
			*(uint32*)&material->surfaceProps.diffuse,
			0.0f);	// extra
		*curVifPtr++ = tmp;
		// call geometry
		MAKEQ(tmp, DMAcall, (uint32)instData->instanceMeshes[i].data, 0x03000000, 0x02000000 | matpipe->vifOffset);
		*curVifPtr++ = tmp;
	}
	MAKEQ(tmp, DMAend, 0, 0, 0);
	*curVifPtr++ = tmp;
//	for(lp = packetbuf; lp < curVifPtr; lp++)
//		printquad4(*lp);
	toVIF1chain(packetbuf);
}

void
beginCamera(void)
{
	uint128 *p, tmp;
	float32 *f;

	p = packetbuf;
	MAKE128(tmp, 0xe, GIF_MAKE_TAG(2, 1, 0, 0, GIF_PACKED, 1));
	*p++ = tmp;
	MAKE128(tmp, GS_XYOFFSET_1, GS_MAKE_XYOFFSET(2048-WIDTH/2 <<4, 2048-HEIGHT/2 <<4));
	*p++ = tmp;
	MAKE128(tmp, GS_TEST_1, GS_MAKE_TEST(0, 0, 0, 0, 0, 0, 1, 2));
	*p++ = tmp;
	toGIF(packetbuf, 3);
	f = (float32*)&vuXYZScale;
	f[0] = WIDTH;
	f[1] = HEIGHT;
	f[2] = camera->zScale;
	f[3] = 0.0f;
	f = (float32*)&vuXYZOffset;
	f[0] = 2048.0f;
	f[1] = 2048.0f;
	f[2] = camera->zShift;
	f[3] = 0.0f;
}

rw::EngineOpenParams engineOpenParams;

void
pluginattach(void)
{
	rw::ps2::registerPDSPlugin(40);
	rw::ps2::registerPluginPDSPipes();

	rw::registerMeshPlugin();
	rw::registerNativeDataPlugin();
	rw::registerAtomicRightsPlugin();
	rw::registerMaterialRightsPlugin();
	rw::xbox::registerVertexFormatPlugin();
	rw::registerSkinPlugin();
	rw::registerHAnimPlugin();
	rw::registerMatFXPlugin();
	rw::registerUVAnimPlugin();
	rw::ps2::registerADCPlugin();
}

bool32
initrw(void)
{
	rw::version = 0x34000;
	rw::platform = rw::PLATFORM_PS2;
	if(!rw::Engine::init())
		return 0;
	pluginattach();
	if(!rw::Engine::open(&engineOpenParams))
		return 0;
	if(!rw::Engine::start())
		return 0;
	rw::Texture::setLoadTextures(0);

	rw::TexDictionary::setCurrent(rw::TexDictionary::create());
	rw::Image::setSearchPath(".");

	world = rw::World::create();
	camera = rw::Camera::create();
	camera->frameBuffer = rw::Raster::create(WIDTH, HEIGHT, 0, rw::Raster::CAMERA);
	camera->zBuffer = rw::Raster::create(WIDTH, HEIGHT, 0, rw::Raster::ZBUFFER);
	camera->setFrame(rw::Frame::create());
	rw::V3d t = { 0.0f, 0.0f, -4.0f };
//	rw::V3d t = { 0.0f, 0.0f, -40.0f };
	camera->getFrame()->translate(&t, rw::COMBINEPOSTCONCAT);
	rw::V3d axis = { 0.0f, 1.0f, 0.0f };
	camera->getFrame()->rotate(&axis, 40.0f, rw::COMBINEPOSTCONCAT);
	camera->setNearPlane(0.1f);
	camera->setFarPlane(450.0f);
	camera->setFOV(60.0f, 4.0f/3.0f);
	world->addCamera(camera);
	return 1;
}

int vsynchInt = 0;

int
vsynch(int id)
{
	vsynchInt = 1;
	frames++;
	ExitHandler();
	return 0;
}

int
main()
{
	FlushCache(0);
	if(!initrw()){
		printf("init failed!\n");	
		for(;;);
	}

	rw::uint32 len;
	rw::uint8 *data = rw::getFileContents("host:player.DFF", &len);
//	rw::uint8 *data = rw::getFileContents("host:od_newscafe_dy.dff", &len);
	rw::StreamMemory in;
	in.open(data, len);
	rw::findChunk(&in, rw::ID_CLUMP, NULL, NULL);
	rw::Clump *clump = rw::Clump::streamRead(&in);
	in.close();
	rwFree(data);

	GsCtx gsCtx;

	dmaReset();

//	GsResetCrt(GS_NONINTERLACED, GS_NTSC, 0);
//	GsInitCtx(&gsCtx, 640, 224, GS_PSMCT32, GS_PSMZ32);

//	GsResetCrt(GS_INTERLACED, GS_NTSC, GS_FRAME);
//	GsInitCtx(&gsCtx, 640, 224, GS_PSMCT32, GS_PSMZ32);

	GsResetCrt(GS_INTERLACED, VMODE, GS_FIELD);
	GsInitCtx(&gsCtx, WIDTH, HEIGHT, GS_PSMCT32, GS_PSMZ32);

	initrender();

	AddIntcHandler(2, vsynch, 0);
	EnableIntc(2);

	GsPutDrawCtx(&gsCtx.draw[0]);
	GsPutDispCtx(&gsCtx.disp[1]);
	// PCSX2 needs a delay for some reason
	{ int i; for(i = 0; i < 1000000; i++); }
	clearscreen(0x80, 0x80, 0x80);
//	drawtest();
//	drawtri();

	float angle = 40.0f;
	int drawbuf = 0;
	int dispbuf = 1;
	for(;;){
		clearscreen(0x80, 0x80, 0x80);

		rw::V3d t = { 0.0f, 0.0f, -4.0f };
		camera->getFrame()->translate(&t, rw::COMBINEREPLACE);
		rw::V3d axis = { 0.0f, 1.0f, 0.0f };
		camera->getFrame()->rotate(&axis, angle, rw::COMBINEPOSTCONCAT);
		angle += 1.0f;

		camera->beginUpdate();
		beginCamera();
		FORLIST(lnk, clump->atomics)
			drawAtomic(rw::Atomic::fromClump(lnk));
		camera->endUpdate();
		printf("");

		while(vsynchInt == 0);
		GsPutDrawCtx(&gsCtx.draw[drawbuf]);
		GsPutDispCtx(&gsCtx.disp[dispbuf]);
		drawbuf = !drawbuf;
		dispbuf = !dispbuf;
		vsynchInt = 0;
	}
/*
	camera->beginUpdate();
	beginCamera();
	FORLIST(lnk, clump->atomics)
		drawAtomic(rw::Atomic::fromClump(lnk));
	camera->endUpdate();

	printf("hello %p\n", clump);
	for(;;)
		printf("");
*/

	return 0;
}

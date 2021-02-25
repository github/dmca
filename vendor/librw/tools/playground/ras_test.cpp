#include <rw.h>
#include <skeleton.h>

namespace rs {

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef struct Canvas Canvas;
struct Canvas
{
	u8 *fb;
	u32 *zbuf;
	int w, h;
};
extern Canvas *canvas;

typedef struct Texture Texture;
struct Texture          
{                       
	u8 *pixels;
	int w, h;
	int wrap;
};              

typedef struct Point3 Point3;
struct Point3
{
	int x, y, z;
};

typedef struct Color Color;
struct Color
{
	u8 r, g, b, a;
};

typedef struct Vertex Vertex;
struct Vertex
{
	i32 x, y, z;
	float q;	// 1/z
	u8 r, g, b, a;
	u8 f;		// fog
	float s, t;
};

Canvas *makecanvas(int w, int h);
Texture *maketexture(int w, int h);
void putpixel(Canvas *canvas, Point3 p, Color c);
void clearcanvas(Canvas *canvas);
void drawTriangle(Canvas *canvas, Vertex p1, Vertex p2, Vertex p3);

// not good
void drawRect(Canvas *canvas, Point3 p1, Point3 p2, Color c);
void drawLine(Canvas *canvas, Point3 p1, Point3 p2, Color c);

//#define trace(...) printf(__VA_ARGS__)
#define trace(...)


int clamp(int x);


/*
 * Render States
 */
enum TextureWrap {
	WRAP_REPEAT,
	WRAP_CLAMP,
	WRAP_BORDER,
};

enum TextureFunction {
	TFUNC_MODULATE,
	TFUNC_DECAL,
	TFUNC_HIGHLIGHT,
	TFUNC_HIGHLIGHT2,
};

enum AlphaTestFunc {
	ALPHATEST_NEVER,
	ALPHATEST_ALWAYS,
	ALPHATEST_LESS,
	ALPHATEST_LEQUAL,
	ALPHATEST_EQUAL,
	ALPHATEST_GEQUAL,
	ALPHATEST_GREATER,
	ALPHATEST_NOTEQUAL,
};

enum AlphaTestFail {
	ALPHAFAIL_KEEP,
	ALPHAFAIL_FB_ONLY,
	ALPHAFAIL_ZB_ONLY,
};

enum DepthTestFunc {
	DEPTHTEST_NEVER,
	DEPTHTEST_ALWAYS,
	DEPTHTEST_GEQUAL,
	DEPTHTEST_GREATER,
};

// The blend equation is
// out = ((A - B) * C >> 7) + D
// A, B and D select the color, C the alpha value
enum AlphaBlendOp {
	ALPHABLEND_SRC,
	ALPHABLEND_DST,
	ALPHABLEND_ZERO,
	ALPHABLEND_FIX = ALPHABLEND_ZERO,
};

extern int srScissorX0, srScissorX1;
extern int srScissorY0, srScissorY1;
extern int srDepthTestEnable;
extern int srDepthTestFunction;
extern int srWriteZ;
extern int srAlphaTestEnable;
extern int srAlphaTestFunction;
extern int srAlphaTestReference;
extern int srAlphaTestFail;
extern int srAlphaBlendEnable;
extern int srAlphaBlendA;
extern int srAlphaBlendB;
extern int srAlphaBlendC;
extern int srAlphaBlendD;
extern int srAlphaBlendFix;
extern int srTexEnable;
extern Texture *srTexture;
extern int srWrapU;
extern int srWrapV;
extern Color srBorder;
extern int srTexUseAlpha;
extern int srTexFunc;
extern int srFogEnable;
extern Color srFogCol;



// end header




#define CEIL(p) (((p)+15) >> 4)

// render states
int srScissorX0, srScissorX1;
int srScissorY0, srScissorY1;
int srDepthTestEnable = 1;
int srDepthTestFunction = DEPTHTEST_GEQUAL;
int srWriteZ = 1;
int srAlphaTestEnable = 1;
int srAlphaTestFunction = ALPHATEST_ALWAYS;
int srAlphaTestReference;
int srAlphaTestFail = ALPHAFAIL_FB_ONLY;
int srAlphaBlendEnable = 1;
int srAlphaBlendA = ALPHABLEND_SRC;
int srAlphaBlendB = ALPHABLEND_DST;
int srAlphaBlendC = ALPHABLEND_SRC;
int srAlphaBlendD = ALPHABLEND_DST;
int srAlphaBlendFix = 0x80;
int srTexEnable = 0;
Texture *srTexture;
int srWrapU = WRAP_REPEAT;
int srWrapV = WRAP_REPEAT;
Color srBorder = { 255, 0, 0, 255 };
int srTexUseAlpha = 1;
int srTexFunc = TFUNC_MODULATE;
int srFogEnable = 0;
Color srFogCol = { 0, 0, 0, 0 };

int clamp(int x) { if(x < 0) return 0; if(x > 255) return 255; return x; }

Canvas*
makecanvas(int w, int h)
{
	Canvas *canv;
	canv = (Canvas*)malloc(sizeof(*canv) + w*h*(4+4));
	canv->w = w;
	canv->h = h;
	canv->fb = ((u8*)canv + sizeof(*canv));
	canv->zbuf = (u32*)(canv->fb + w*h*4);
	return canv;
}

Texture*
maketexture(int w, int h)
{
	Texture *t;
	t = (Texture*)malloc(sizeof(*t) + w*h*4);
	t->w = w;
	t->h = h;
	t->pixels = (u8*)t + sizeof(*t);
	t->wrap = 0x11; // wrap u and v
	return t;
}

void
clearcanvas(Canvas *canvas)
{
	memset(canvas->fb, 0, canvas->w*canvas->h*4);
	memset(canvas->zbuf, 0, canvas->w*canvas->h*4);
}

void
writefb(Canvas *canvas, int x, int y, Color c)
{
	u8 *px = &canvas->fb[(y*canvas->w + x)*4];
	u32 *z = &canvas->zbuf[y*canvas->w + x];

	px[3] = c.r;
	px[2] = c.g;
	px[1] = c.b;
	px[0] = c.a;
}

void
putpixel(Canvas *canvas, Point3 p, Color c)
{
	// scissor test
	if(p.x < srScissorX0 || p.x > srScissorX1 ||
	   p.y < srScissorY0 || p.y > srScissorY1)
		return;

	u8 *px = &canvas->fb[(p.y*canvas->w + p.x)*4];
	u32 *z = &canvas->zbuf[p.y*canvas->w + p.x];

	int fbwrite = 1;
	int zbwrite = srWriteZ;

	// alpha test
	if(srAlphaTestEnable){
		int fail;
		switch(srAlphaTestFunction){
		case ALPHATEST_NEVER:
			fail = 1;
			break;
		case ALPHATEST_ALWAYS:
			fail = 0;
			break;
		case ALPHATEST_LESS:
			fail = c.a >= srAlphaTestReference;
			break;
		case ALPHATEST_LEQUAL:
			fail = c.a > srAlphaTestReference;
			break;
		case ALPHATEST_EQUAL:
			fail = c.a != srAlphaTestReference;
			break;
		case ALPHATEST_GEQUAL:
			fail = c.a < srAlphaTestReference;
			break;
		case ALPHATEST_GREATER:
			fail = c.a <= srAlphaTestReference;
			break;
		case ALPHATEST_NOTEQUAL:
			fail = c.a == srAlphaTestReference;
			break;
		}
		if(fail){
			switch(srAlphaTestFail){
			case ALPHAFAIL_KEEP:
				return;
			case ALPHAFAIL_FB_ONLY:
				zbwrite = 0;
				break;
			case ALPHAFAIL_ZB_ONLY:
				fbwrite = 0;
			}
		}
	}

	// ztest
	if(srDepthTestEnable){
		switch(srDepthTestFunction){
		case DEPTHTEST_NEVER:
			return;
		case DEPTHTEST_ALWAYS:
			break;
		case DEPTHTEST_GEQUAL:
			if((u32)p.z < *z)
				return;
			break;
		case DEPTHTEST_GREATER:
			if((u32)p.z <= *z)
				return;
			break;
		}
	}

	Color d = { px[3], px[2], px[1], px[0] };

	// blend
	if(srAlphaBlendEnable){
		int ar, ag, ab;
		int br, bg, bb;
		int dr, dg, db;
		int ca;
		switch(srAlphaBlendA){
		case ALPHABLEND_SRC:
			ar = c.r;
			ag = c.g;
			ab = c.b;
			break;
		case ALPHABLEND_DST:
			ar = d.r;
			ag = d.g;
			ab = d.b;
			break;
		case ALPHABLEND_ZERO:
			ar = 0;
			ag = 0;
			ab = 0;
			break;
		default: assert(0);
		}
		switch(srAlphaBlendB){
		case ALPHABLEND_SRC:
			br = c.r;
			bg = c.g;
			bb = c.b;
			break;
		case ALPHABLEND_DST:
			br = d.r;
			bg = d.g;
			bb = d.b;
			break;
		case ALPHABLEND_ZERO:
			br = 0;
			bg = 0;
			bb = 0;
			break;
		default: assert(0);
		}
		switch(srAlphaBlendC){
		case ALPHABLEND_SRC:
			ca = c.a;
			break;
		case ALPHABLEND_DST:
			ca = d.a;
			break;
		case ALPHABLEND_FIX:
			ca = srAlphaBlendFix;
			break;
		default: assert(0);
		}
		switch(srAlphaBlendD){
		case ALPHABLEND_SRC:
			dr = c.r;
			dg = c.g;
			db = c.b;
			break;
		case ALPHABLEND_DST:
			dr = d.r;
			dg = d.g;
			db = d.b;
			break;
		case ALPHABLEND_ZERO:
			dr = 0;
			dg = 0;
			db = 0;
			break;
		default: assert(0);
		}

		int r, g, b;
		r = ((ar - br) * ca >> 7) + dr;
		g = ((ag - bg) * ca >> 7) + dg;
		b = ((ab - bb) * ca >> 7) + db;

		c.r = clamp(r);
		c.g = clamp(g);
		c.b = clamp(b);
	}

	if(fbwrite)
		writefb(canvas, p.x, p.y, c);
	if(zbwrite)
		*z = p.z;
}

Color
sampletex_nearest(int u, int v)
{
	Texture *tex = srTexture;

	const int usize = tex->w;
	const int vsize = tex->h;

	int iu = u >> 4;
	int iv = v >> 4;

	switch(srWrapU){
	case WRAP_REPEAT:
		iu %= usize;
		break;
	case WRAP_CLAMP:
		if(iu < 0) iu = 0;
		if(iu >= usize) iu = usize-1;
		break;
	case WRAP_BORDER:
		if(iu < 0 || iu >= usize)
			return srBorder;
	}

	switch(srWrapV){
	case WRAP_REPEAT:
		iv %= vsize;
		break;
	case WRAP_CLAMP:
		if(iv < 0) iv = 0;
		if(iv >= vsize) iv = vsize-1;
		break;
	case WRAP_BORDER:
		if(iv < 0 || iv >= vsize)
			return srBorder;
	}

	u8 *cp = &tex->pixels[(iv*tex->w + iu)*4];
	Color c = { cp[0], cp[1], cp[2], cp[3] };
	return c;
}

// t is texture, f is fragment
Color
texfunc(Color t, Color f)
{
	int r, g, b, a;
	switch(srTexFunc){
	case TFUNC_MODULATE:
		r = t.r * f.r >> 7;
		g = t.g * f.g >> 7;
		b = t.b * f.b >> 7;
		a = srTexUseAlpha ?
			t.a * f.a >> 7 :
			f.a;
		break;
	case TFUNC_DECAL:
		r = t.r;
		g = t.g;
		b = t.b;
		a = srTexUseAlpha ? t.a : f.a;
		break;
	case TFUNC_HIGHLIGHT:
		r = (t.r * f.r >> 7) + f.a;
		g = (t.g * f.g >> 7) + f.a;
		b = (t.b * f.b >> 7) + f.a;
		a = srTexUseAlpha ?
			t.a + f.a :
			f.a;
		break;
	case TFUNC_HIGHLIGHT2:
		r = (t.r * f.r >> 7) + f.a;
		g = (t.g * f.g >> 7) + f.a;
		b = (t.b * f.b >> 7) + f.a;
		a = srTexUseAlpha ? t.a : f.a;
		break;
	}
	Color v;
	v.r = clamp(r);
	v.g = clamp(g);
	v.b = clamp(b);
	v.a = clamp(a);
	return v;
}

Point3 mkpnt(int x, int y, int z) { Point3 p = { x, y, z}; return p; }

void
drawRect(Canvas *canvas, Point3 p1, Point3 p2, Color c)
{
	int x, y;
	for(y = p1.y; y <= p2.y; y++)
		for(x = p1.x; x <= p2.x; x++)
			putpixel(canvas, mkpnt(x, y, 0), c);
}

void
drawLine(Canvas *canvas, Point3 p1, Point3 p2, Color c)
{
	int dx, dy;
	int incx, incy;
	int e;
	int x, y;

	dx = abs(p2.x-p1.x);
	incx = p2.x > p1.x ? 1 : -1;
	dy = abs(p2.y-p1.y);
	incy = p2.y > p1.y ? 1 : -1;
	e = 0;
	if(dx == 0){
		for(y = p1.y; y != p2.y; y += incy)
			putpixel(canvas, mkpnt(p1.x, y, 0), c);
	}else if(dx > dy){
		y = p1.y;
		for(x = p1.x; x != p2.x; x += incx){
			putpixel(canvas, mkpnt(x, y, 0), c);
			e += dy;
			if(2*e >= dx){
				e -= dx;
				y += incy;
			}
		}
	}else{
		x = p1.x;
		for(y = p1.y; y != p2.y; y += incy){
			putpixel(canvas, mkpnt(x, y, 0), c);
			e += dx;
			if(2*e >= dy){
				e -= dy;
				x += incx;
			}
		}
	}
}

/*
	attibutes we want to interpolate:
	R G B A
	U V / S T Q
	X Y Z F
*/

struct TriAttribs
{
	i64 z;
	i32 r, g, b, a;
	i32 f;
	float s, t;
	float q;
};

static void
add1(struct TriAttribs *a, struct TriAttribs *b)
{
	a->z += b->z;
	a->r += b->r;
	a->g += b->g;
	a->b += b->b;
	a->a += b->a;
	a->f += b->f;
	a->s += b->s;
	a->t += b->t;
	a->q += b->q;
}

static void
sub1(struct TriAttribs *a, struct TriAttribs *b)
{
	a->z -= b->z;
	a->r -= b->r;
	a->g -= b->g;
	a->b -= b->b;
	a->a -= b->a;
	a->f -= b->f;
	a->s -= b->s;
	a->t -= b->t;
	a->q -= b->q;
}

static void
guard(struct TriAttribs *a)
{
	if(a->z < 0) a->z = 0;
	else if(a->z > 0x3FFFFFFFC000LL) a->z = 0x3FFFFFFFC000LL;
	if(a->r < 0) a->r = 0;
	else if(a->r > 0xFF000) a->r = 0xFF000;
	if(a->g < 0) a->g = 0;
	else if(a->g > 0xFF000) a->g = 0xFF000;
	if(a->b < 0) a->b = 0;
	else if(a->b > 0xFF000) a->b = 0xFF000;
	if(a->a < 0) a->a = 0;
	else if(a->a > 0xFF000) a->a = 0xFF000;
	if(a->f < 0) a->f = 0;
	else if(a->f > 0xFF000) a->f = 0xFF000;
}

struct RasTri
{
	int x, y;
	int ymid, yend;
	int right;
	int e[2], dx[3], dy[3];
	struct TriAttribs gx, gy, v, s;
};

static int
triangleSetup(struct RasTri *tri, Vertex v1, Vertex v2, Vertex v3)
{
	int dx1, dx2, dx3;
	int dy1, dy2, dy3;

	dy1 = v3.y - v1.y;	// long edge
	if(dy1 == 0) return 1;
	dx1 = v3.x - v1.x;
	dx2 = v2.x - v1.x;	// first small edge
	dy2 = v2.y - v1.y;
	dx3 = v3.x - v2.x;	// second small edge
	dy3 = v3.y - v2.y;

	// this is twice the triangle area
	const int area = dx2*dy1 - dx1*dy2;
	if(area == 0) return 1;
	// figure out if 0 or 1 is the right edge
	tri->right = area < 0;

	/* The gradients are to step whole pixels,
	 * so they are pre-multiplied by 16. */

	float denom = 16.0f/area;
	// gradients x
#define GX(p) ((v2.p - v1.p)*dy1 - (v3.p - v1.p)*dy2)
	tri->gx.z = GX(z)*denom * 16384;
	tri->gx.r = GX(r)*denom * 4096;
	tri->gx.g = GX(g)*denom * 4096;
	tri->gx.b = GX(b)*denom * 4096;
	tri->gx.a = GX(a)*denom * 4096;
	tri->gx.f = GX(f)*denom * 4096;
	tri->gx.s = GX(s)*denom;
	tri->gx.t = GX(t)*denom;
	tri->gx.q = GX(q)*denom;

	// gradients y
	denom = -denom;
#define GY(p) ((v2.p - v1.p)*dx1 - (v3.p - v1.p)*dx2)
	tri->gy.z = GY(z)*denom * 16384;
	tri->gy.r = GY(r)*denom * 4096;
	tri->gy.g = GY(g)*denom * 4096;
	tri->gy.b = GY(b)*denom * 4096;
	tri->gy.a = GY(a)*denom * 4096;
	tri->gy.f = GY(f)*denom * 4096;
	tri->gy.s = GY(s)*denom;
	tri->gy.t = GY(t)*denom;
	tri->gy.q = GY(q)*denom;

	tri->ymid = CEIL(v2.y);
	tri->yend = CEIL(v3.y);

	tri->y = CEIL(v1.y);
	tri->x = CEIL(v1.x);

	tri->dy[0] = dy2<<4;	// upper edge
	tri->dy[1] = dy1<<4;	// lower edge
	tri->dy[2] = dy3<<4;	// long edge
	tri->dx[0] = dx2<<4;
	tri->dx[1] = dx1<<4;
	tri->dx[2] = dx3<<4;

	// prestep to land on pixel center

	int stepx = v1.x - (tri->x<<4);
	int stepy = v1.y - (tri->y<<4);
	tri->e[0] = (-stepy*tri->dx[0] + stepx*tri->dy[0]) >> 4;
	tri->e[1] = (-stepy*tri->dx[1] + stepx*tri->dy[1]) >> 4;

	// attributes along interpolated edge	
	// why is this cast needed? (mingw)
	tri->v.z = (i64)v1.z*16384 - (stepy*tri->gy.z + stepx*tri->gx.z)/16;
	tri->v.r = v1.r*4096 - (stepy*tri->gy.r + stepx*tri->gx.r)/16;
	tri->v.g = v1.g*4096 - (stepy*tri->gy.g + stepx*tri->gx.g)/16;
	tri->v.b = v1.b*4096 - (stepy*tri->gy.b + stepx*tri->gx.b)/16;
	tri->v.a = v1.a*4096 - (stepy*tri->gy.a + stepx*tri->gx.a)/16;
	tri->v.f = v1.f*4096 - (stepy*tri->gy.f + stepx*tri->gx.f)/16;
	tri->v.s = v1.s - (stepy*tri->gy.s + stepx*tri->gx.s)/16.0f;
	tri->v.t = v1.t - (stepy*tri->gy.t + stepx*tri->gx.t)/16.0f;
	tri->v.q = v1.q - (stepy*tri->gy.q + stepx*tri->gx.q)/16.0f;

	return 0;
}

void
drawTriangle(Canvas *canvas, Vertex v1, Vertex v2, Vertex v3)
{
	Color c;
	struct RasTri tri;
	int stepx, stepy;

	// Sort such that we have from top to bottom v1,v2,v3
	if(v2.y < v1.y){ Vertex tmp = v1; v1 = v2; v2 = tmp; }
	if(v3.y < v1.y){ Vertex tmp = v1; v1 = v3; v3 = tmp; }
	if(v3.y < v2.y){ Vertex tmp = v2; v2 = v3; v3 = tmp; }

	if(triangleSetup(&tri, v1, v2, v3))
		return;

	// Current scanline start and end
	int xn[2] = { tri.x, tri.x };
	int a = !tri.right;	// left edge
	int b = tri.right;	// right edge

	// If upper triangle has no height, only do the lower part
	if(tri.dy[0] == 0)
		goto secondtri;
	while(tri.y < tri.yend){
		/* TODO: is this the righ way to step the edges? */

		/* Step x and interpolated value down left edge */
		while(tri.e[a] <= -tri.dy[a]){
			xn[a]--;
			tri.e[a] += tri.dy[a];
			sub1(&tri.v, &tri.gx);
		}
		while(tri.e[a] > 0){
			xn[a]++;
			tri.e[a] -= tri.dy[a];
			add1(&tri.v, &tri.gx);
		}

		/* Step x down right edge */
		while(tri.e[b] <= -tri.dy[b]){
			xn[b]--;
			tri.e[b] += tri.dy[b];
		}
		while(tri.e[b] > 0){
			xn[b]++;
			tri.e[b] -= tri.dy[b];
		}

		// When we reach the mid vertex, change state and jump to start of loop again
		// TODO: this is a bit ugly in here...can we fix it?
		if(tri.y == tri.ymid){
		secondtri:
			tri.dx[0] = tri.dx[2];
			tri.dy[0] = tri.dy[2];
			// Either the while prevents this or we returned early because dy1 == 0
			assert(tri.dy[0] != 0);
			stepx = v2.x - (xn[0]<<4);
			stepy = v2.y - (tri.y<<4);
			tri.e[0] = (-stepy*tri.dx[0] + stepx*tri.dy[0]) >> 4;

			tri.ymid = -1;	// so we don't do this again
			continue;
		}

		/* Rasterize one line */
		tri.s = tri.v;
		for(tri.x = xn[a]; tri.x < xn[b]; tri.x++){
			guard(&tri.s);
			c.r = tri.s.r >> 12;
			c.g = tri.s.g >> 12;
			c.b = tri.s.b >> 12;
			c.a = tri.s.a >> 12;
			if(srTexEnable && srTexture){
				float w = 1.0f/tri.s.q;
				float s = tri.s.s * w;
				float t = tri.s.t * w;
				int u = s * srTexture->w * 16;
				int v = t * srTexture->h * 16;
				Color texc = sampletex_nearest(u, v);
				c = texfunc(texc, c);
			}
			if(srFogEnable){
				const int f = tri.s.f >> 12;
				c.r = (f*c.r >> 8) + ((255 - f)*srFogCol.r >> 8);
				c.g = (f*c.g >> 8) + ((255 - f)*srFogCol.g >> 8);
				c.b = (f*c.b >> 8) + ((255 - f)*srFogCol.b >> 8);
			}
			putpixel(canvas, mkpnt(tri.x, tri.y, tri.s.z>>14), c);
			add1(&tri.s, &tri.gx);
		}

		/* Step in y */
		tri.y++;
		tri.e[a] += tri.dx[a];
		tri.e[b] += tri.dx[b];
		add1(&tri.v, &tri.gy);
	}
}

Canvas *canvas;

}

using namespace rw;

void
rastest_renderTriangles(RWDEVICE::Im2DVertex *scrverts, int32 numVerts, uint16 *indices, int32 numTris)
{
	int i;
	RGBA col;
	rs::Vertex v[3];
	RWDEVICE::Im2DVertex *iv;

	rs::srDepthTestEnable = 1;
	rs::srAlphaTestEnable = 0;
	rs::srTexEnable = 0;
	rs::srAlphaBlendEnable = 0;

	while(numTris--){
		for(i = 0; i < 3; i++){
			iv = &scrverts[indices[i]];
			v[i].x = iv->getScreenX() * 16.0f;
			v[i].y = iv->getScreenY() * 16.0f;
			v[i].z = 16777216*(1.0f-iv->getScreenZ());
			v[i].q = iv->getRecipCameraZ();
			col = iv->getColor();
			v[i].r = col.red;
			v[i].g = col.green;
			v[i].b = col.blue;
			v[i].a = col.alpha;
			v[i].f = 0;
			v[i].s = iv->u*iv->getRecipCameraZ();
			v[i].t = iv->v*iv->getRecipCameraZ();
		}
		drawTriangle(rs::canvas, v[0], v[1], v[2]);

		indices += 3;
	}
}

extern rw::Raster *testras;

void
beginSoftras(void)
{
	Camera *cam = (Camera*)engine->currentCamera;

	if(rs::canvas == nil ||
	   cam->frameBuffer->width != rs::canvas->w ||
	   cam->frameBuffer->height != rs::canvas->h){
		rs::canvas = rs::makecanvas(cam->frameBuffer->width, cam->frameBuffer->height);
		testras = rw::Raster::create(rs::canvas->w, rs::canvas->h, 32, rw::Raster::C8888);
	}

	clearcanvas(rs::canvas);
	rs::srScissorX0 = 0;
	rs::srScissorX1 = rs::canvas->w-1;
	rs::srScissorY0 = 0;
	rs::srScissorY1 = rs::canvas->h-1;
}

void
endSoftras(void)
{
	int i;
	uint8 *dst = testras->lock(0, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
	if(dst == nil)
		return;
	uint8 *src = rs::canvas->fb;
	for(i = 0; i < rs::canvas->w*rs::canvas->h; i++){
		dst[0] = src[1];
		dst[1] = src[2];
		dst[2] = src[3];
		dst[3] = src[0];
		dst += 4;
		src += 4;
	}
	// abgr in canvas
	// bgra in raster
	testras->unlock(0);
}


/*
typedef struct PixVert PixVert;
struct PixVert
{
	float x, y, z, q;
	int r, g, b, a;
	float u, v;
};
#include "test.inc"

void
drawtest(void)
{
	int i, j;
	rs::Vertex v[3];

	rs::srDepthTestEnable = 1;
	rs::srAlphaTestEnable = 0;
	rs::srTexEnable = 0;
	rs::srAlphaBlendEnable = 0;

	for(i = 0; i < nelem(verts); i += 3){
		for(j = 0; j < 3; j++){
			v[j].x = verts[i+j].x * 16.0f;
			v[j].y = verts[i+j].y * 16.0f;
			v[j].z = 16777216*(1.0f - verts[i+j].z);
			v[j].q = verts[i+j].q;
			v[j].r = verts[i+j].r;
			v[j].g = verts[i+j].g;
			v[j].b = verts[i+j].b;
			v[j].a = verts[i+j].a;
			v[j].f = 0;
			v[j].s = verts[i+j].u*v[j].q;
			v[j].t = verts[i+j].v*v[j].q;
		}
		drawTriangle(rs::canvas, v[0], v[1], v[2]);
	}
//exit(0);
}
*/

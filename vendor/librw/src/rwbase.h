#ifndef RW_PS2
#include <stdint.h>
#endif
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TODO: clean up the opengl defines
//       and figure out what we even want here...
#ifdef RW_GL3
#define RW_OPENGL
#define RWDEVICE gl3
// doesn't help
//#define RW_GL_USE_VAOS
#endif

#ifdef RW_GLES2
#define RW_GLES
#endif
#ifdef RW_GLES3
#define RW_GLES
#endif

#ifdef RW_D3D9
#define RWDEVICE d3d
#define RWHALFPIXEL
#endif

#ifdef RW_D3D8
#define RWDEVICE d3d
#endif

#ifdef RW_PS2
#define RWHALFPIXEL
#define RWDEVICE ps2
#endif

#ifdef RW_WDGL
#define RW_OPENGL
#endif

namespace rw {

#ifdef RW_PS2
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;
	typedef unsigned int uintptr;
#else
	/* get rid of the stupid _t */
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;
	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
	typedef uintptr_t uintptr;
#endif

typedef float float32;
typedef int32 bool32;
typedef uint8 byte;
typedef uint32 uint;

#define nil NULL

#define nelem(A) (sizeof(A) / sizeof A[0])

#ifdef __GNUC__
#define RWALIGN(n) __attribute__ ((aligned (n)))
#else
#ifdef _MSC_VER
#define RWALIGN(n) __declspec(align(n))
#else
#define RWALIGN(n)	// unknown compiler...ignore
#endif
#endif

// Lists

struct LLLink
{
	LLLink *next;
	LLLink *prev;
	void init(void){
		this->next = nil;
		this->prev = nil;
	}
	void remove(void){
		this->prev->next = this->next;
		this->next->prev = this->prev;
	}
};

#define LLLinkGetData(linkvar,type,entry) \
    ((type*)(((rw::uint8*)(linkvar))-offsetof(type,entry)))

// Have to be careful since the link might be deleted.
#define FORLIST(_link, _list) \
	for(rw::LLLink *_next = nil, *_link = (_list).link.next; \
	_next = (_link)->next, (_link) != (_list).end(); \
	(_link) = _next)

struct LinkList
{
	LLLink link;
	void init(void){
		this->link.next = &this->link;
		this->link.prev = &this->link;
	}
	bool32 isEmpty(void){
		return this->link.next == &this->link;
	}
	void add(LLLink *link){
		link->next = this->link.next;
		link->prev = &this->link;
		this->link.next->prev = link;
		this->link.next = link;
	}
	void append(LLLink *link){
		link->next = &this->link;
		link->prev = this->link.prev;
		this->link.prev->next = link;
		this->link.prev = link;
	}
	LLLink *end(void){
		return &this->link;
	}
	int32 count(void){
		int32 n = 0;
		FORLIST(lnk, (*this))
			n++;
		return n;
	}
};

// Mathematical types

struct RGBA
{
	uint8 red;
	uint8 green;
	uint8 blue;
	uint8 alpha;
};
inline RGBA makeRGBA(uint8 r, uint8 g, uint8 b, uint8 a) { RGBA c = { r, g, b, a }; return c; }
inline bool32 equal(const RGBA &c1, const RGBA &c2) { return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue && c1.alpha == c2.alpha; }
#define RWRGBAINT(r, g, b, a) ((uint32)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

struct RGBAf
{
	float32 red;
	float32 green;
	float32 blue;
	float32 alpha;
};
inline RGBAf makeRGBAf(float32 r, float32 g, float32 b, float32 a) { RGBAf c = { r, g, b, a }; return c; }
inline bool32 equal(const RGBAf &c1, const RGBAf &c2) { return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue && c1.alpha == c2.alpha; }
inline RGBAf add(const RGBAf &a, const RGBAf &b) { return makeRGBAf(a.red+b.red, a.green+b.green, a.blue+b.blue, a.alpha+b.alpha); }
inline RGBAf modulate(const RGBAf &a, const RGBAf &b) { return makeRGBAf(a.red*b.red, a.green*b.green, a.blue*b.blue, a.alpha*b.alpha); }
inline RGBAf scale(const RGBAf &a, float32 f) { return makeRGBAf(a.red*f, a.green*f, a.blue*f, a.alpha*f); }
inline void clamp(RGBAf *a) {
	if(a->red > 1.0f) a->red = 1.0f;
	if(a->red < 0.0f) a->red = 0.0f;
	if(a->green > 1.0f) a->green = 1.0f;
	if(a->green < 0.0f) a->green = 0.0f;
	if(a->blue > 1.0f) a->blue = 1.0f;
	if(a->blue < 0.0f) a->blue = 0.0f;
	if(a->alpha > 1.0f) a->alpha = 1.0f;
	if(a->alpha < 0.0f) a->alpha = 0.0f;
}

inline void convColor(RGBA *i, const RGBAf *f){
	int32 c;
	c = (int32)(f->red*255.0f + 0.5f);
	i->red   = (uint8)c;
	c = (int32)(f->green*255.0f + 0.5f);
	i->green = (uint8)c;
	c = (int32)(f->blue*255.0f + 0.5f);
	i->blue  = (uint8)c;
	c = (int32)(f->alpha*255.0f + 0.5f);
	i->alpha = (uint8)c;
}

inline void convColor(RGBAf *f, const RGBA *i){
	f->red   = i->red/255.0f;
	f->green = i->green/255.0f;
	f->blue  = i->blue/255.0f;
	f->alpha = i->alpha/255.0f;
}

struct TexCoords
{
	float32 u, v;
};
inline bool32 equal(const TexCoords &t1, const TexCoords &t2) { return t1.u == t2.u && t1.v == t2.v; }

struct V2d;
struct V3d;
struct Quat;
struct Matrix;

struct V2d
{
	float32 x, y;
	void set(float32 x, float32 y){
		this->x = x; this->y = y; }
};

inline V2d makeV2d(float32 x, float32 y) { V2d v = { x, y }; return v; }
inline bool32 equal(const V2d &v1, const V2d &v2) { return v1.x == v2.x && v1.y == v2.y; }
inline V2d neg(const V2d &a) { return makeV2d(-a.x, -a.y); }
inline V2d add(const V2d &a, const V2d &b) { return makeV2d(a.x+b.x, a.y+b.y); }
inline V2d sub(const V2d &a, const V2d &b) { return makeV2d(a.x-b.x, a.y-b.y); }
inline V2d scale(const V2d &a, float32 r) { return makeV2d(a.x*r, a.y*r); }
inline float32 length(const V2d &v) { return sqrtf(v.x*v.x + v.y*v.y); }
inline V2d normalize(const V2d &v) { return scale(v, 1.0f/length(v)); }

struct V3d
{
	float32 x, y, z;
	void set(float32 x, float32 y, float32 z){
		this->x = x; this->y = y; this->z = z; }
	static void transformPoints(V3d *out, const V3d *in, int32 n, const Matrix *m);
	static void transformVectors(V3d *out, const V3d *in, int32 n, const Matrix *m);
};

inline V3d makeV3d(float32 x, float32 y, float32 z) { V3d v = { x, y, z }; return v; }
inline bool32 equal(const V3d &v1, const V3d &v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }
inline V3d neg(const V3d &a) { return makeV3d(-a.x, -a.y, -a.z); }
inline V3d add(const V3d &a, const V3d &b) { return makeV3d(a.x+b.x, a.y+b.y, a.z+b.z); }
inline V3d sub(const V3d &a, const V3d &b) { return makeV3d(a.x-b.x, a.y-b.y, a.z-b.z); }
inline V3d scale(const V3d &a, float32 r) { return makeV3d(a.x*r, a.y*r, a.z*r); }
inline float32 length(const V3d &v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
inline V3d normalize(const V3d &v) { return scale(v, 1.0f/length(v)); }
inline V3d setlength(const V3d &v, float32 l) { return scale(v, l/length(v)); }
V3d cross(const V3d &a, const V3d &b);
inline float32 dot(const V3d &a, const V3d &b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline V3d lerp(const V3d &a, const V3d &b, float32 r){
	return makeV3d(a.x + r*(b.x - a.x),
	               a.y + r*(b.y - a.y),
	               a.z + r*(b.z - a.z));
};

struct V4d
{
	float32 x, y, z, w;
};
inline bool32 equal(const V4d &v1, const V4d &v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w; }

enum CombineOp
{
	COMBINEREPLACE,
	COMBINEPRECONCAT,
	COMBINEPOSTCONCAT
};


Quat makeQuat(float32 w, float32 x, float32 y, float32 z);
Quat makeQuat(float32 w, const V3d &vec);

struct Quat
{
	// order is important for streaming and RW compatibility
	float32 x, y, z, w;

	static Quat rotation(float32 angle, const V3d &axis){
		return makeQuat(cosf(angle/2.0f), scale(normalize(axis), sinf(angle/2.0f))); }
	void set(float32 w, float32 x, float32 y, float32 z){
		this->w = w; this->x = x; this->y = y; this->z = z; }
	V3d vec(void){ return makeV3d(x, y, z); }

	Quat *rotate(const V3d *axis, float32 angle, CombineOp op);
};

inline Quat makeQuat(float32 w, float32 x, float32 y, float32 z) { Quat q = { x, y, z, w }; return q; }
inline Quat makeQuat(float32 w, const V3d &vec) { Quat q = { vec.x, vec.y, vec.z, w }; return q; }
inline Quat add(const Quat &q, const Quat &p) { return makeQuat(q.w+p.w, q.x+p.x, q.y+p.y, q.z+p.z); }
inline Quat sub(const Quat &q, const Quat &p) { return makeQuat(q.w-p.w, q.x-p.x, q.y-p.y, q.z-p.z); }
inline Quat negate(const Quat &q) { return makeQuat(-q.w, -q.x, -q.y, -q.z); }
inline float32 dot(const Quat &q, const Quat &p) { return q.w*p.w + q.x*p.x + q.y*p.y + q.z*p.z; }
inline Quat scale(const Quat &q, float32 r) { return makeQuat(q.w*r, q.x*r, q.y*r, q.z*r); }
inline float32 length(const Quat &q) { return sqrtf(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z); }
inline Quat normalize(const Quat &q) { return scale(q, 1.0f/length(q)); }
inline Quat conj(const Quat &q) { return makeQuat(q.w, -q.x, -q.y, -q.z); }
Quat mult(const Quat &q, const Quat &p);
inline V3d rotate(const V3d &v, const Quat &q) { return mult(mult(q, makeQuat(0.0f, v)), conj(q)).vec(); }
Quat lerp(const Quat &q, const Quat &p, float32 r);
Quat slerp(const Quat &q, const Quat &p, float32 a);

struct RawMatrix
{
	V3d right;
	float32 rightw;
	V3d up;
	float32 upw;
	V3d at;
	float32 atw;
	V3d pos;
	float32 posw;

	static void mult(RawMatrix *dst, RawMatrix *src1, RawMatrix *src2);
	static void transpose(RawMatrix *dst, RawMatrix *src);
	static void setIdentity(RawMatrix *dst);
};

struct Matrix
{
	enum Type {
		TYPENORMAL	= 1,
		TYPEORTHOGONAL	= 2,
		TYPEORTHONORMAL	= 3,
		TYPEMASK = 3
	};
	enum Flags {
		IDENTITY = 0x20000
	};
	struct Tolerance {
		float32 normal;
		float32 orthogonal;
		float32 identity;
	};

	V3d right;
	uint32 flags;
	V3d up;
	uint32 pad1;
	V3d at;
	uint32 pad2;
	V3d pos;
	uint32 pad3;

	static Matrix *create(void);
	void destroy(void);
	void setIdentity(void);
	void optimize(Tolerance *tolerance = nil);
	void update(void) { flags &= ~(IDENTITY|TYPEMASK); }
	static Matrix *mult(Matrix *dst, const Matrix *src1, const Matrix *src2);
	static Matrix *invert(Matrix *dst, const Matrix *src);
	static Matrix *transpose(Matrix *dst, const Matrix *src);
	Matrix *rotate(const V3d *axis, float32 angle, CombineOp op);
	Matrix *rotate(const Quat &q, CombineOp op);
	Matrix *translate(const V3d *translation, CombineOp op);
	Matrix *scale(const V3d *scl, CombineOp op);
	Matrix *transform(const Matrix *mat, CombineOp op);
	Quat getRotation(void);
	void lookAt(const V3d &dir, const V3d &up);

	// helper functions. consider private
	static void mult_(Matrix *dst, const Matrix *src1, const Matrix *src2);
	static void invertOrthonormal(Matrix *dst, const Matrix *src);
	static Matrix *invertGeneral(Matrix *dst, const Matrix *src);
	static void makeRotation(Matrix *dst, const V3d *axis, float32 angle);
	static void makeRotation(Matrix *dst, const Quat &q);
private:
	float32 normalError(void);
	float32 orthogonalError(void);
	float32 identityError(void);
};

inline void convMatrix(Matrix *dst, RawMatrix *src){
	*dst = *(Matrix*)src;
	dst->optimize();
}

inline void convMatrix(RawMatrix *dst, Matrix *src){
	*dst = *(RawMatrix*)src;
	dst->rightw = 0.0;
	dst->upw = 0.0;
	dst->atw = 0.0;
	dst->posw = 1.0;
}

struct Line
{
	V3d start;
	V3d end;
};

struct Rect
{
	int32 x, y;
	int32 w, h;
};

struct Sphere
{
	V3d center;
	float32 radius;
};

struct Plane
{
	V3d normal;
	float32 distance;
};

struct BBox
{
	V3d sup;
	V3d inf;

	void initialize(V3d *point);
	void addPoint(V3d *point);
	void calculate(V3d *points, int32 n);
	bool containsPoint(V3d *point);
};

enum PrimitiveType
{
	PRIMTYPENONE = 0,
	PRIMTYPELINELIST,
	PRIMTYPEPOLYLINE,
	PRIMTYPETRILIST,
	PRIMTYPETRISTRIP,
	PRIMTYPETRIFAN,
	PRIMTYPEPOINTLIST
};

/*
 * Memory
 */

void memNative32_func(void *data, uint32 size);
void memNative16_func(void *data, uint32 size);
void memLittle32_func(void *data, uint32 size);
void memLittle16_func(void *data, uint32 size);

#ifdef BIGENDIAN
inline void memNative32(void *data, uint32 size) { memNative32_func(data, size); }
inline void memNative16(void *data, uint32 size) { memNative16_func(data, size); }
inline void memLittle32(void *data, uint32 size) { memLittle32_func(data, size); }
inline void memLittle16(void *data, uint32 size) { memLittle16_func(data, size); }
#define ASSERTLITTLE assert(0 && "unsafe code on big-endian")
#else
inline void memNative32(void *data, uint32 size) { }
inline void memNative16(void *data, uint32 size) { }
inline void memLittle32(void *data, uint32 size) { }
inline void memLittle16(void *data, uint32 size) { }
#define ASSERTLITTLE
#endif

/*
 * Streams
 */

void makePath(char *filename);

class Stream
{
public:
	virtual ~Stream(void) { close(); }
	virtual void close(void) {}
	virtual uint32 write8(const void *data, uint32 length) = 0;
	virtual uint32 read8(void *data, uint32 length) = 0;
	virtual void seek(int32 offset, int32 whence = 1) = 0;
	virtual uint32 tell(void) = 0;
	virtual bool eof(void) = 0;
	uint32  write32(const void *data, uint32 length);
	uint32  write16(const void *data, uint32 length);
	uint32  read32(void *data, uint32 length);
	uint32  read16(void *data, uint32 length);
	int32   writeI8(int8 val);
	int32   writeU8(uint8 val);
	int32   writeI16(int16 val);
	int32   writeU16(uint16 val);
	int32   writeI32(int32 val);
	int32   writeU32(uint32 val);
	int32   writeF32(float32 val);
	int8    readI8(void);
	uint8   readU8(void);
	int16   readI16(void);
	uint16  readU16(void);
	int32   readI32(void);
	uint32  readU32(void);
	float32 readF32(void);
};

class StreamMemory : public Stream
{
	uint8 *data;
	uint32 length;
	uint32 capacity;
	uint32 position;
public:
	void close(void);
	uint32 write8(const void *data, uint32 length);
	uint32 read8(void *data, uint32 length);
	void seek(int32 offset, int32 whence = 1);
	uint32 tell(void);
	bool eof(void);
	StreamMemory *open(uint8 *data, uint32 length, uint32 capacity = 0);
	uint32 getLength(void);

	enum {
		S_EOF = 0xFFFFFFFF
	};
};

class StreamFile : public Stream
{
	FILE *file;
public:
	StreamFile(void) { file = nil; }
	void close(void);
	uint32 write8(const void *data, uint32 length);
	uint32 read8(void *data, uint32 length);
	void seek(int32 offset, int32 whence = 1);
	uint32 tell(void);
	bool eof(void);
	StreamFile *open(const char *path, const char *mode);
};

enum Platform
{
	PLATFORM_NULL = 0,
	// D3D7
	PLATFORM_GL   = 2,
	// MAC
	PLATFORM_PS2  = 4,
	PLATFORM_XBOX = 5,
	// GAMECUBE
	// SOFTRAS
	PLATFORM_D3D8 = 8,
	PLATFORM_D3D9 = 9,
	// PSP

	// non-stock-RW platforms

	PLATFORM_WDGL = 11,	// WarDrum OpenGL
	PLATFORM_GL3  = 12,	// my GL3 implementation

	NUM_PLATFORMS,

	FOURCC_PS2 = 0x00325350		// 'PS2\0'
};

#define MAKEPLUGINID(v, id) (((v & 0xFFFFFF) << 8) | (id & 0xFF))
#define MAKEPIPEID(v, id) (((v & 0xFFFF) << 16) | (id & 0xFFFF))

enum VendorID
{
	VEND_CORE           = 0,
	VEND_CRITERIONTK    = 1,
	VEND_CRITERIONINT   = 4,
	VEND_CRITERIONWORLD = 5,
	// Used for rasters (platform-specific)
	VEND_RASTER         = 10,
	// Used for driver/device allocation tags
	VEND_DRIVER         = 11
};

// TODO: modules (VEND_CRITERIONINT)

enum PluginID
{
	// Core
	ID_NAOBJECT      = MAKEPLUGINID(VEND_CORE, 0x00),
	ID_STRUCT        = MAKEPLUGINID(VEND_CORE, 0x01),
	ID_STRING        = MAKEPLUGINID(VEND_CORE, 0x02),
	ID_EXTENSION     = MAKEPLUGINID(VEND_CORE, 0x03),
	ID_CAMERA        = MAKEPLUGINID(VEND_CORE, 0x05),
	ID_TEXTURE       = MAKEPLUGINID(VEND_CORE, 0x06),
	ID_MATERIAL      = MAKEPLUGINID(VEND_CORE, 0x07),
	ID_MATLIST       = MAKEPLUGINID(VEND_CORE, 0x08),
	ID_WORLD         = MAKEPLUGINID(VEND_CORE, 0x0B),
	ID_MATRIX        = MAKEPLUGINID(VEND_CORE, 0x0D),
	ID_FRAMELIST     = MAKEPLUGINID(VEND_CORE, 0x0E),
	ID_GEOMETRY      = MAKEPLUGINID(VEND_CORE, 0x0F),
	ID_CLUMP         = MAKEPLUGINID(VEND_CORE, 0x10),
	ID_LIGHT         = MAKEPLUGINID(VEND_CORE, 0x12),
	ID_ATOMIC        = MAKEPLUGINID(VEND_CORE, 0x14),
	ID_TEXTURENATIVE = MAKEPLUGINID(VEND_CORE, 0x15),
	ID_TEXDICTIONARY = MAKEPLUGINID(VEND_CORE, 0x16),
	ID_IMAGE         = MAKEPLUGINID(VEND_CORE, 0x18),
	ID_GEOMETRYLIST  = MAKEPLUGINID(VEND_CORE, 0x1A),
	ID_ANIMANIMATION = MAKEPLUGINID(VEND_CORE, 0x1B),
	ID_RIGHTTORENDER = MAKEPLUGINID(VEND_CORE, 0x1F),
	ID_UVANIMDICT    = MAKEPLUGINID(VEND_CORE, 0x2B),

	// Toolkit
	ID_SKYMIPMAP     = MAKEPLUGINID(VEND_CRITERIONTK, 0x10),
	ID_SKIN          = MAKEPLUGINID(VEND_CRITERIONTK, 0x16),
	ID_HANIM         = MAKEPLUGINID(VEND_CRITERIONTK, 0x1E),
	ID_USERDATA      = MAKEPLUGINID(VEND_CRITERIONTK, 0x1F),
	ID_MATFX         = MAKEPLUGINID(VEND_CRITERIONTK, 0x20),
	ID_ANISOT        = MAKEPLUGINID(VEND_CRITERIONTK, 0x27),
	ID_PDS           = MAKEPLUGINID(VEND_CRITERIONTK, 0x31),
	ID_ADC           = MAKEPLUGINID(VEND_CRITERIONTK, 0x34),
	ID_UVANIMATION   = MAKEPLUGINID(VEND_CRITERIONTK, 0x35),

	// World
	ID_MESH          = MAKEPLUGINID(VEND_CRITERIONWORLD, 0x0E),
	ID_NATIVEDATA    = MAKEPLUGINID(VEND_CRITERIONWORLD, 0x10),
	ID_VERTEXFMT     = MAKEPLUGINID(VEND_CRITERIONWORLD, 0x11),

	// custom native raster
	ID_RASTERGL      = MAKEPLUGINID(VEND_RASTER, PLATFORM_GL),
	ID_RASTERPS2     = MAKEPLUGINID(VEND_RASTER, PLATFORM_PS2),
	ID_RASTERXBOX    = MAKEPLUGINID(VEND_RASTER, PLATFORM_XBOX),
	ID_RASTERD3D8    = MAKEPLUGINID(VEND_RASTER, PLATFORM_D3D8),
	ID_RASTERD3D9    = MAKEPLUGINID(VEND_RASTER, PLATFORM_D3D9),
	ID_RASTERWDGL    = MAKEPLUGINID(VEND_RASTER, PLATFORM_WDGL),
	ID_RASTERGL3     = MAKEPLUGINID(VEND_RASTER, PLATFORM_GL3),

	// anything driver/device related (only as allocation tag)
	ID_DRIVER        = MAKEPLUGINID(VEND_DRIVER, 0)
};

enum CoreModuleID
{
	ID_NAMODULE      = MAKEPLUGINID(VEND_CRITERIONINT, 0x00),
	// vector
	// matrix
	ID_FRAMEMODULE   = MAKEPLUGINID(VEND_CRITERIONINT, 0x03),
	// stream
	// camera
	ID_IMAGEMODULE   = MAKEPLUGINID(VEND_CRITERIONINT, 0x06),
	ID_RASTERMODULE  = MAKEPLUGINID(VEND_CRITERIONINT, 0x07),
	ID_TEXTUREMODULE = MAKEPLUGINID(VEND_CRITERIONINT, 0x08)
	// pip
	// immediate
	// resources
	// device
	// color
	// unused
	// error
	// metrics
	// driver
	// chunk group
};

#define ECODE(c, s) c

enum Errors
{
	ERR_NONE = 0x80000000,
#include "base.err"
};

#undef ECODE

extern int32 version;
extern int32 build;
extern int32 platform;
extern bool32 streamAppendFrames;
extern char *debugFile;

int strcmp_ci(const char *s1, const char *s2);
int strncmp_ci(const char *s1, const char *s2, int n);

// 0x04000000	3.1
// 0x08000000	3.2
// 0x0C000000	3.3
// 0x10000000	3.4
// 0x14000000	3.5
// 0x18000000	3.6
// 0x1C000000	3.7

inline uint32
libraryIDPack(int version, int build)
{
	if(version <= 0x31000)
		return version>>8;
	return ((version-0x30000) & 0x3FF00) << 14 | (version&0x3F) << 16 |
	       (build & 0xFFFF);
}

inline int
libraryIDUnpackVersion(uint32 libid)
{
	if(libid & 0xFFFF0000)
		return ((libid>>14 & 0x3FF00) + 0x30000) |
		       (libid>>16 & 0x3F);
	else
		return libid<<8;
}

inline int
libraryIDUnpackBuild(uint32 libid)
{
	if(libid & 0xFFFF0000)
		return libid & 0xFFFF;
	else
		return 0;
}

struct ChunkHeaderInfo
{
	uint32 type;
	uint32 length;
	uint32 version, build;
};

// TODO?: make these methods of ChunkHeaderInfo?
bool writeChunkHeader(Stream *s, int32 type, int32 size);
bool readChunkHeaderInfo(Stream *s, ChunkHeaderInfo *header);
bool findChunk(Stream *s, uint32 type, uint32 *length, uint32 *version);

int32 findPointer(void *p, void **list, int32 num);
uint8 *getFileContents(const char *name, uint32 *len);
}

namespace rw {

// Render states

enum RenderState
{
	TEXTURERASTER,
	TEXTUREADDRESS,
	TEXTUREADDRESSU,
	TEXTUREADDRESSV,
	TEXTUREFILTER,
	VERTEXALPHA,
	SRCBLEND,
	DESTBLEND,
	ZTESTENABLE,
	ZWRITEENABLE,
	FOGENABLE,
	FOGCOLOR,
	CULLMODE,
	// TODO:
	// fog type, density ?
	// ? shademode

	STENCILENABLE,
	STENCILFAIL,
	STENCILZFAIL,
	STENCILPASS,
	STENCILFUNCTION,
	STENCILFUNCTIONREF,
	STENCILFUNCTIONMASK,
	STENCILFUNCTIONWRITEMASK,

	// platform specific or opaque?
	ALPHATESTFUNC,
	ALPHATESTREF,

	// emulation of PS2 GS alpha test
	//  in the mode where it still writes color but nor depth
	GSALPHATEST,
	GSALPHATESTREF
};

enum AlphaTestFunc
{
	ALPHAALWAYS,
	ALPHAGREATEREQUAL,
	ALPHALESS
};

enum StencilOp
{
	STENCILKEEP = 1,
	STENCILZERO,
	STENCILREPLACE,
	STENCILINCSAT,
	STENCILDECSAT,
	STENCILINVERT,
	STENCILINC,
	STENCILDEC
};

enum StencilFunc
{
	STENCILNEVER = 1,
	STENCILLESS,
	STENCILEQUAL,
	STENCILLESSEQUAL,
	STENCILGREATER,
	STENCILNOTEQUAL,
	STENCILGREATEREQUAL,
	STENCILALWAYS
};

enum CullMode
{
	CULLNONE = 1,
	CULLBACK,
	CULLFRONT
};

enum BlendFunction
{
	BLENDZERO = 1,
	BLENDONE,
	BLENDSRCCOLOR,
	BLENDINVSRCCOLOR,
	BLENDSRCALPHA,
	BLENDINVSRCALPHA,
	BLENDDESTALPHA,
	BLENDINVDESTALPHA,
	BLENDDESTCOLOR,
	BLENDINVDESTCOLOR,
	BLENDSRCALPHASAT
	// TODO: add more perhaps
};

void SetRenderState(int32 state, uint32 value);
void SetRenderStatePtr(int32 state, void *value);
uint32 GetRenderState(int32 state);
void *GetRenderStatePtr(int32 state);

// Im2D

namespace im2d {

float32 GetNearZ(void);
float32 GetFarZ(void);
void RenderLine(void *verts, int32 numVerts, int32 vert1, int32 vert2);
void RenderTriangle(void *verts, int32 numVerts, int32 vert1, int32 vert2, int32 vert3);
void RenderIndexedPrimitive(PrimitiveType, void *verts, int32 numVerts, void *indices, int32 numIndices);
void RenderPrimitive(PrimitiveType type, void *verts, int32 numVerts);

}

// Im3D

namespace im3d {

enum TransformFlags
{
	VERTEXUV      = 1,	// has tex Coords
	ALLOPAQUE     = 2,	// no vertex alpha
	NOCLIP        = 4,	// don't frustum clip
	VERTEXXYZ     = 8,	// has position
	VERTEXRGBA    = 16,	// has color
	EVERYTHING = VERTEXUV|VERTEXXYZ|VERTEXRGBA
};

void Transform(void *vertices, int32 numVertices, Matrix *world, uint32 flags);
void RenderPrimitive(PrimitiveType primType);
void RenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices);
void End(void);

}

}


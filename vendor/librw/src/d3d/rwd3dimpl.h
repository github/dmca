namespace rw {
namespace d3d {

#ifdef RW_D3D9
void openIm2D(void);
void closeIm2D(void);
void im2DRenderLine(void *vertices, int32 numVertices, int32 vert1, int32 vert2);
void im2DRenderTriangle(void *vertices, int32 numVertices, int32 vert1, int32 vert2, int32 vert3);
void im2DRenderPrimitive(PrimitiveType primType, void *vertices, int32 numVertices);
void im2DRenderIndexedPrimitive(PrimitiveType primType, void *vertices, int32 numVertices, void *indices, int32 numIndices);

void openIm3D(void);
void closeIm3D(void);
void im3DTransform(void *vertices, int32 numVertices, Matrix *world, uint32 flags);
void im3DRenderPrimitive(PrimitiveType primType);
void im3DRenderIndexedPrimitive(PrimitiveType primType, void *indices, int32 numIndices);
void im3DEnd(void);


struct DisplayMode
{
	D3DDISPLAYMODE mode;
	uint32 flags;
};

struct D3d9Globals
{
	HWND window;

	IDirect3D9 *d3d9;
	int numAdapters;
	int adapter;
	D3DCAPS9 caps;
	DisplayMode *modes;
	int numModes;
	int currentMode;
	DisplayMode startMode;
	
	uint32 msLevel;

	D3DPRESENT_PARAMETERS present;

	IDirect3DSurface9 *defaultRenderTarget;
	IDirect3DSurface9 *defaultDepthSurf;

	int numTextures;
	int numVertexShaders;
	int numPixelShaders;
	int numVertexBuffers;
	int numIndexBuffers;
	int numVertexDeclarations;
};

extern D3d9Globals d3d9Globals;

void addVidmemRaster(Raster *raster);
void removeVidmemRaster(Raster *raster);

void addDynamicVB(uint32 length, uint32 fvf, IDirect3DVertexBuffer9 **buf);	// NB: don't share this pointer
void removeDynamicVB(IDirect3DVertexBuffer9 **buf);

void addDynamicIB(uint32 length, IDirect3DIndexBuffer9 **buf);	// NB: don't share this pointer
void removeDynamicIB(IDirect3DIndexBuffer9 **buf);


int findFormatDepth(uint32 format);
void evictD3D9Raster(Raster *raster);

#endif

Raster *rasterCreate(Raster *raster);
uint8 *rasterLock(Raster *raster, int32 level, int32 lockMode);
void rasterUnlock(Raster *raster, int32 level);
int32 rasterNumLevels(Raster *raster);
bool32 imageFindRasterFormat(Image *img, int32 type,
	int32 *width, int32 *height, int32 *depth, int32 *format);
bool32 rasterFromImage(Raster *raster, Image *image);
Image *rasterToImage(Raster *raster);

}
}

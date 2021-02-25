namespace rw {
namespace ps2 {

Raster *rasterCreate(Raster *raster);
uint8 *rasterLock(Raster*, int32 level, int32 lockMode);
void rasterUnlock(Raster*, int32 level);
uint8 *rasterLockPalette(Raster*, int32 lockMode);
void rasterUnlockPalette(Raster*);
int32 rasterNumLevels(Raster*);
bool32 imageFindRasterFormat(Image *img, int32 type,
	int32 *width, int32 *height, int32 *depth, int32 *format);
bool32 rasterFromImage(Raster *raster, Image *image);
Image *rasterToImage(Raster *raster);

}
}

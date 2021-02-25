namespace rw {
namespace xbox {

Raster *rasterCreate(Raster *raster);
uint8 *rasterLock(Raster *raster, int32 level, int32 lockMode);
void rasterUnlock(Raster*, int32);
int32 rasterNumLevels(Raster *raster);
Image *rasterToImage(Raster *raster);

}
}

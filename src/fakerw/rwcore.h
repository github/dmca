#pragma once

#define RWCORE_H	// needed by CVector

#include <rw.h>

#include <rwplcore.h>

/*
 ***********************************************
 *
 * RwIm2D and RwIm3D
 *
 ***********************************************
 */

typedef rw::RWDEVICE::Im2DVertex RwIm2DVertex;
typedef rw::RWDEVICE::Im3DVertex RwIm3DVertex;
typedef RwUInt16 RwImVertexIndex;

enum RwIm3DTransformFlags
{
	rwIM3D_VERTEXUV      = rw::im3d::VERTEXUV,
	rwIM3D_ALLOPAQUE     = rw::im3d::ALLOPAQUE,
	rwIM3D_NOCLIP        = rw::im3d::NOCLIP,
	rwIM3D_VERTEXXYZ     = rw::im3d::VERTEXXYZ,
	rwIM3D_VERTEXRGBA    = rw::im3d::VERTEXRGBA,
};

void RwIm2DVertexSetCameraX(RwIm2DVertex *vert, RwReal camx);
void RwIm2DVertexSetCameraY(RwIm2DVertex *vert, RwReal camy);
void RwIm2DVertexSetCameraZ(RwIm2DVertex *vert, RwReal camz);
void RwIm2DVertexSetRecipCameraZ(RwIm2DVertex *vert, RwReal recipz);
void RwIm2DVertexSetScreenX(RwIm2DVertex *vert, RwReal scrnx);
void RwIm2DVertexSetScreenY(RwIm2DVertex *vert, RwReal scrny);
void RwIm2DVertexSetScreenZ(RwIm2DVertex *vert, RwReal scrnz);
void RwIm2DVertexSetU(RwIm2DVertex *vert, RwReal texU, RwReal recipz);
void RwIm2DVertexSetV(RwIm2DVertex *vert, RwReal texV, RwReal recipz);
void RwIm2DVertexSetIntRGBA(RwIm2DVertex *vert, RwUInt8 red, RwUInt8 green, RwUInt8 blue, RwUInt8 alpha);

RwReal RwIm2DGetNearScreenZ(void);
RwReal RwIm2DGetFarScreenZ(void);
RwBool RwIm2DRenderLine(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool RwIm2DRenderTriangle(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool RwIm2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);


void RwIm3DVertexSetPos(RwIm3DVertex *vert, RwReal x, RwReal y, RwReal z);
void RwIm3DVertexSetU(RwIm3DVertex *vert, RwReal u);
void RwIm3DVertexSetV(RwIm3DVertex *vert, RwReal v);
void RwIm3DVertexSetRGBA(RwIm3DVertex *vert, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a);

void  *RwIm3DTransform(RwIm3DVertex *pVerts, RwUInt32 numVerts, RwMatrix *ltm, RwUInt32 flags);
RwBool RwIm3DEnd(void);
RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2);
RwBool RwIm3DRenderTriangle(RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex *indices, RwInt32 numIndices);
RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType);


/*
 ***********************************************
 *
 * RwRaster
 *
 ***********************************************
 */

//struct RwRaster;
typedef rw::Raster RwRaster;

enum RwRasterType
{
	rwRASTERTYPENORMAL = rw::Raster::NORMAL,
	rwRASTERTYPEZBUFFER = rw::Raster::ZBUFFER,
	rwRASTERTYPECAMERA = rw::Raster::CAMERA,
	rwRASTERTYPETEXTURE = rw::Raster::TEXTURE,
	rwRASTERTYPECAMERATEXTURE = rw::Raster::CAMERATEXTURE,
	rwRASTERTYPEMASK = 0x07,
	rwRASTERDONTALLOCATE = rw::Raster::DONTALLOCATE,
};

enum RwRasterFormat
{
	rwRASTERFORMATDEFAULT = rw::Raster::DEFAULT,
	rwRASTERFORMAT1555 = rw::Raster::C1555,
	rwRASTERFORMAT565 = rw::Raster::C565,
	rwRASTERFORMAT4444 = rw::Raster::C4444,
	rwRASTERFORMATLUM8 = rw::Raster::LUM8,
	rwRASTERFORMAT8888 = rw::Raster::C8888,
	rwRASTERFORMAT888 = rw::Raster::C888,
	rwRASTERFORMAT16 = rw::Raster::D16,
	rwRASTERFORMAT24 = rw::Raster::D24,
	rwRASTERFORMAT32 = rw::Raster::D32,
	rwRASTERFORMAT555 = rw::Raster::C555,

	rwRASTERFORMATAUTOMIPMAP = rw::Raster::AUTOMIPMAP,
	rwRASTERFORMATPAL8 = rw::Raster::PAL8,
	rwRASTERFORMATPAL4 = rw::Raster::PAL4,
	rwRASTERFORMATMIPMAP = rw::Raster::MIPMAP,

	rwRASTERFORMATPIXELFORMATMASK = 0x0f00,
	rwRASTERFORMATMASK = 0xff00
};

enum RwRasterFlipMode
{
	rwRASTERFLIPDONTWAIT = 0,
	rwRASTERFLIPWAITVSYNC = 1,
};

RwRaster    *RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool       RwRasterDestroy(RwRaster * raster);
RwInt32      RwRasterGetWidth(const RwRaster *raster);
RwInt32      RwRasterGetHeight(const RwRaster *raster);
RwInt32      RwRasterGetStride(const RwRaster *raster);
RwInt32      RwRasterGetDepth(const RwRaster *raster);
RwInt32      RwRasterGetFormat(const RwRaster *raster);
RwInt32      RwRasterGetType(const RwRaster *raster);
RwRaster    *RwRasterGetParent(const RwRaster *raster);
RwRaster    *RwRasterGetOffset(RwRaster *raster,  RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32      RwRasterGetNumLevels(RwRaster * raster);
RwRaster    *RwRasterSubRaster(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster    *RwRasterRenderFast(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster    *RwRasterRender(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster    *RwRasterRenderScaled(RwRaster * raster, RwRect * rect);
RwRaster    *RwRasterPushContext(RwRaster * raster);
RwRaster    *RwRasterPopContext(void);
RwRaster    *RwRasterGetCurrentContext(void);
RwBool       RwRasterClear(RwInt32 pixelValue);
RwBool       RwRasterClearRect(RwRect * rpRect, RwInt32 pixelValue);
RwRaster    *RwRasterShowRaster(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8     *RwRasterLock(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster    *RwRasterUnlock(RwRaster * raster);
RwUInt8     *RwRasterLockPalette(RwRaster * raster, RwInt32 lockMode);
RwRaster    *RwRasterUnlockPalette(RwRaster * raster);
RwInt32      RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32      RwRasterGetPluginOffset(RwUInt32 pluginID);
RwBool       RwRasterValidatePlugins(const RwRaster * raster);


/*
 ***********************************************
 *
 * RwImage
 *
 ***********************************************
 */

//struct RwImage;
typedef rw::Image RwImage;

RwImage     *RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool       RwImageDestroy(RwImage * image);
RwImage     *RwImageAllocatePixels(RwImage * image);
RwImage     *RwImageFreePixels(RwImage * image);
RwImage     *RwImageCopy(RwImage * destImage, const RwImage * sourceImage);
RwImage     *RwImageResize(RwImage * image, RwInt32 width, RwInt32 height);
RwImage     *RwImageApplyMask(RwImage * image, const RwImage * mask);
RwImage     *RwImageMakeMask(RwImage * image);
RwImage     *RwImageReadMaskedImage(const RwChar * imageName, const RwChar * maskname);
RwImage     *RwImageRead(const RwChar * imageName);
RwImage     *RwImageWrite(RwImage * image, const RwChar * imageName);
RwChar      *RwImageGetPath(void);
const RwChar *RwImageSetPath(const RwChar * path);
RwImage     *RwImageSetStride(RwImage * image, RwInt32 stride);
RwImage     *RwImageSetPixels(RwImage * image, RwUInt8 * pixels);
RwImage     *RwImageSetPalette(RwImage * image, RwRGBA * palette);
RwInt32      RwImageGetWidth(const RwImage * image);
RwInt32      RwImageGetHeight(const RwImage * image);
RwInt32      RwImageGetDepth(const RwImage * image);
RwInt32      RwImageGetStride(const RwImage * image);
RwUInt8     *RwImageGetPixels(const RwImage * image);
RwRGBA      *RwImageGetPalette(const RwImage * image);
RwUInt32     RwRGBAToPixel(RwRGBA * rgbIn, RwInt32 rasterFormat);
RwRGBA      *RwRGBASetFromPixel(RwRGBA * rgbOut, RwUInt32 pixelValue, RwInt32 rasterFormat);
RwBool       RwImageSetGamma(RwReal gammaValue);
RwReal       RwImageGetGamma(void);
RwImage     *RwImageGammaCorrect(RwImage * image);
RwRGBA      *RwRGBAGammaCorrect(RwRGBA * rgb);
RwInt32      RwImageRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32      RwImageGetPluginOffset(RwUInt32 pluginID);
RwBool       RwImageValidatePlugins(const RwImage * image);
//RwBool       RwImageRegisterImageFormat(const RwChar * extension, RwImageCallBackRead imageRead, RwImageCallBackWrite imageWrite);
const RwChar *RwImageFindFileType(const RwChar * imageName);
RwInt32      RwImageStreamGetSize(const RwImage * image);
RwImage     *RwImageStreamRead(RwStream * stream);
const RwImage *RwImageStreamWrite(const RwImage * image, RwStream * stream);


/*
 ***********************************************
 *
 * RwTexture
 *
 ***********************************************
 */

//struct RwTexture;
typedef rw::Texture RwTexture;
//struct RwTexDictionary;
typedef rw::TexDictionary RwTexDictionary;

typedef RwTexture *(*RwTextureCallBackRead)(const RwChar *name, const RwChar *maskName);
typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *pData);
typedef RwTexDictionary *(*RwTexDictionaryCallBack)(RwTexDictionary *dict, void *data);
typedef RwRaster *(*RwTextureCallBackMipmapGeneration)(RwRaster * raster, RwImage * image);
typedef RwBool (*RwTextureCallBackMipmapName)(RwChar *name, RwChar *maskName, RwUInt8 mipLevel, RwInt32 format);

RwTexture *RwTextureCreate(RwRaster * raster);
RwBool RwTextureDestroy(RwTexture * texture);
RwTexture *RwTextureAddRef(RwTexture *texture);
RwBool RwTextureSetMipmapping(RwBool enable);
RwBool RwTextureGetMipmapping(void);
RwBool RwTextureSetAutoMipmapping(RwBool enable);
RwBool RwTextureGetAutoMipmapping(void);
RwBool RwTextureSetMipmapGenerationCallBack(RwTextureCallBackMipmapGeneration callback);
RwTextureCallBackMipmapGeneration RwTextureGetMipmapGenerationCallBack(void);
RwBool RwTextureSetMipmapNameCallBack(RwTextureCallBackMipmapName callback);
RwTextureCallBackMipmapName RwTextureGetMipmapNameCallBack(void);
RwBool RwTextureGenerateMipmapName(RwChar * name, RwChar * maskName, RwUInt8 mipLevel, RwInt32 format);
RwBool RwTextureRasterGenerateMipmaps(RwRaster * raster, RwImage * image);
RwTextureCallBackRead RwTextureGetReadCallBack(void);
RwBool RwTextureSetReadCallBack(RwTextureCallBackRead fpCallBack);
RwTexture *RwTextureSetName(RwTexture * texture, const RwChar * name);
RwTexture *RwTextureSetMaskName(RwTexture * texture, const RwChar * maskName);
RwChar *RwTextureGetName(RwTexture *texture);
RwChar *RwTextureGetMaskName(RwTexture *texture);
RwTexture *RwTextureSetRaster(RwTexture * texture, RwRaster * raster);
RwTexture   *RwTextureRead(const RwChar * name, const RwChar * maskName);
RwRaster *RwTextureGetRaster(const RwTexture *texture);
RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32 RwTextureGetPluginOffset(RwUInt32 pluginID);
RwBool RwTextureValidatePlugins(const RwTexture * texture);

RwTexDictionary *RwTextureGetDictionary(RwTexture *texture);
RwTexture *RwTextureSetFilterMode(RwTexture *texture, RwTextureFilterMode filtering);
RwTextureFilterMode RwTextureGetFilterMode(const RwTexture *texture);
RwTexture *RwTextureSetAddressing(RwTexture *texture, RwTextureAddressMode addressing);
RwTexture *RwTextureSetAddressingU(RwTexture *texture, RwTextureAddressMode addressing);
RwTexture *RwTextureSetAddressingV(RwTexture *texture, RwTextureAddressMode addressing);
RwTextureAddressMode RwTextureGetAddressing(const RwTexture *texture);
RwTextureAddressMode RwTextureGetAddressingU(const RwTexture *texture);
RwTextureAddressMode RwTextureGetAddressingV(const RwTexture *texture);

void _rwD3D8TexDictionaryEnableRasterFormatConversion(bool enable);

// hack for reading native textures
RwBool rwNativeTextureHackRead(RwStream *stream, RwTexture **tex, RwInt32 size);


RwTexDictionary *RwTexDictionaryCreate(void);
RwBool RwTexDictionaryDestroy(RwTexDictionary * dict);
RwTexture *RwTexDictionaryAddTexture(RwTexDictionary * dict, RwTexture * texture);
RwTexture *RwTexDictionaryRemoveTexture(RwTexture * texture);
RwTexture *RwTexDictionaryFindNamedTexture(RwTexDictionary * dict, const RwChar * name);
RwTexDictionary *RwTexDictionaryGetCurrent(void);
RwTexDictionary *RwTexDictionarySetCurrent(RwTexDictionary * dict);
const RwTexDictionary *RwTexDictionaryForAllTextures(const RwTexDictionary * dict, RwTextureCallBack fpCallBack, void *pData);
RwBool RwTexDictionaryForAllTexDictionaries(RwTexDictionaryCallBack fpCallBack, void *pData);
RwInt32 RwTexDictionaryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32 RwTexDictionaryGetPluginOffset(RwUInt32 pluginID);
RwBool RwTexDictionaryValidatePlugins(const RwTexDictionary * dict);
RwUInt32 RwTexDictionaryStreamGetSize(const RwTexDictionary *texDict);
RwTexDictionary *RwTexDictionaryStreamRead(RwStream *stream);
const RwTexDictionary *RwTexDictionaryStreamWrite(const RwTexDictionary *texDict, RwStream *stream);

/* RwImage/RwRaster */

RwImage *RwImageSetFromRaster(RwImage *image, RwRaster *raster);
RwRaster *RwRasterSetFromImage(RwRaster *raster, RwImage *image);
RwRGBA *RwRGBAGetRasterPixel(RwRGBA *rgbOut, RwRaster *raster, RwInt32 x, RwInt32 y);
RwRaster *RwRasterRead(const RwChar *filename);
RwRaster *RwRasterReadMaskedRaster(const RwChar *filename, const RwChar *maskname);
RwImage *RwImageFindRasterFormat(RwImage *ipImage,RwInt32 nRasterType, RwInt32 *npWidth,RwInt32 *npHeight, RwInt32 *npDepth,RwInt32 *npFormat);


/*
 ***********************************************
 *
 * RwFrame
 *
 ***********************************************
 */

//struct RwFrame;
typedef rw::Frame RwFrame;

typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);


RwFrame *RwFrameForAllObjects(RwFrame * frame, RwObjectCallBack callBack, void *data);
RwFrame *RwFrameTranslate(RwFrame * frame, const RwV3d * v, RwOpCombineType combine);
RwFrame *RwFrameRotate(RwFrame * frame, const RwV3d * axis, RwReal angle, RwOpCombineType combine);
RwFrame *RwFrameScale(RwFrame * frame, const RwV3d * v, RwOpCombineType combine);
RwFrame *RwFrameTransform(RwFrame * frame, const RwMatrix * m, RwOpCombineType combine);
RwFrame *RwFrameOrthoNormalize(RwFrame * frame);
RwFrame *RwFrameSetIdentity(RwFrame * frame);
RwFrame *RwFrameCloneHierarchy(RwFrame * root);
RwBool RwFrameDestroyHierarchy(RwFrame * frame);
RwFrame *RwFrameForAllChildren(RwFrame * frame, RwFrameCallBack callBack, void *data);
RwFrame *RwFrameRemoveChild(RwFrame * child);
RwFrame *RwFrameAddChild(RwFrame * parent, RwFrame * child);
RwFrame *RwFrameGetParent(const RwFrame * frame);
RwFrame *RwFrameGetRoot(const RwFrame * frame);
RwMatrix *RwFrameGetLTM(RwFrame * frame);
RwMatrix *RwFrameGetMatrix(RwFrame * frame);
RwFrame *RwFrameUpdateObjects(RwFrame * frame);
RwFrame *RwFrameCreate(void);
RwBool RwFrameInit(RwFrame *frame);
RwBool RwFrameDeInit(RwFrame *frame);
RwBool RwFrameDestroy(RwFrame * frame);
void _rwFrameInit(RwFrame *frame);
void _rwFrameDeInit(RwFrame *frame);
RwBool RwFrameDirty(const RwFrame * frame);
RwInt32 RwFrameCount(RwFrame * frame);
RwBool RwFrameSetStaticPluginsSize(RwInt32 size);
RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32 RwFrameGetPluginOffset(RwUInt32 pluginID);
RwBool RwFrameValidatePlugins(const RwFrame * frame);
RwFrame *_rwFrameCloneAndLinkClones(RwFrame * root);
RwFrame *_rwFramePurgeClone(RwFrame *root);

RwInt32 RwFrameRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB);
RwInt32 RwFrameSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB);

typedef rw::FrameList_ rwFrameList;
rwFrameList *rwFrameListInitialize(rwFrameList *frameList, RwFrame *frame);
RwBool  rwFrameListFindFrame(const rwFrameList *frameList, const RwFrame *frame, RwInt32 *npIndex);
rwFrameList *rwFrameListDeinitialize(rwFrameList *frameList);
RwUInt32 rwFrameListStreamGetSize(const rwFrameList *frameList);
rwFrameList *rwFrameListStreamRead(RwStream *stream, rwFrameList *fl);
const rwFrameList *rwFrameListStreamWrite(const rwFrameList *frameList, RwStream *stream);


typedef rw::BBox RwBBox;

/*
 ***********************************************
 *
 * RwCamera
 *
 ***********************************************
 */

//struct RwCamera;
typedef rw::Camera RwCamera;

typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);

enum RwCameraClearMode
{
	rwCAMERACLEARIMAGE = 0x1,
	rwCAMERACLEARZ = 0x2,
	rwCAMERACLEARSTENCIL = 0x4
};

enum RwCameraProjection
{
	rwNACAMERAPROJECTION = 0,
	rwPERSPECTIVE = 1,
	rwPARALLEL = 2
};

enum RwFrustumTestResult
{
	rwSPHEREOUTSIDE = 0,
	rwSPHEREBOUNDARY = 1,
	rwSPHEREINSIDE = 2
};

RwCamera    *RwCameraBeginUpdate(RwCamera * camera);
RwCamera    *RwCameraEndUpdate(RwCamera * camera);
RwCamera    *RwCameraClear(RwCamera * camera, RwRGBA * colour, RwInt32 clearMode);
RwCamera    *RwCameraShowRaster(RwCamera * camera, void *pDev, RwUInt32 flags);
RwBool       RwCameraDestroy(RwCamera * camera);
RwCamera    *RwCameraCreate(void);
RwCamera    *RwCameraClone(RwCamera * camera);
RwCamera    *RwCameraSetViewOffset(RwCamera *camera, const RwV2d *offset);
RwCamera    *RwCameraSetViewWindow(RwCamera *camera, const RwV2d *viewWindow);
RwCamera    *RwCameraSetProjection(RwCamera *camera, RwCameraProjection projection);
RwCamera    *RwCameraSetNearClipPlane(RwCamera *camera, RwReal nearClip);
RwCamera    *RwCameraSetFarClipPlane(RwCamera *camera, RwReal farClip);
RwInt32      RwCameraRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
RwInt32      RwCameraGetPluginOffset(RwUInt32 pluginID);
RwBool       RwCameraValidatePlugins(const RwCamera * camera);
RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera * camera, const RwSphere * sphere);
const RwV2d *RwCameraGetViewOffset(const RwCamera *camera);
RwCamera    *RwCameraSetRaster(RwCamera *camera, RwRaster *raster);
RwRaster    *RwCameraGetRaster(const RwCamera *camera);
RwCamera    *RwCameraSetZRaster(RwCamera *camera, RwRaster *zRaster);
RwRaster    *RwCameraGetZRaster(const RwCamera *camera);
RwReal       RwCameraGetNearClipPlane(const RwCamera *camera);
RwReal       RwCameraGetFarClipPlane(const RwCamera *camera);
RwCamera    *RwCameraSetFogDistance(RwCamera *camera, RwReal fogDistance);
RwReal       RwCameraGetFogDistance(const RwCamera *camera);
RwCamera    *RwCameraGetCurrentCamera(void);
RwCameraProjection RwCameraGetProjection(const RwCamera *camera);
const RwV2d *RwCameraGetViewWindow(const RwCamera *camera);
RwMatrix    *RwCameraGetViewMatrix(RwCamera *camera);
RwCamera    *RwCameraSetFrame(RwCamera *camera, RwFrame *frame);
RwFrame     *RwCameraGetFrame(const RwCamera *camera);


/*
 *
 * D3D-engine specific stuff
 *
 */

void RwD3D8EngineSetRefreshRate(RwUInt32 refreshRate);
RwBool RwD3D8DeviceSupportsDXTTexture(void);
void RwD3D8EngineSetMultiSamplingLevels(RwUInt32 level);
RwUInt32 RwD3D8EngineGetMaxMultiSamplingLevels(void);

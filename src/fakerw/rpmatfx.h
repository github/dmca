#pragma once

enum RpMatFXMaterialFlags
{
	rpMATFXEFFECTNULL       = rw::MatFX::NOTHING,
	rpMATFXEFFECTBUMPMAP    = rw::MatFX::BUMPMAP,
	rpMATFXEFFECTENVMAP     = rw::MatFX::ENVMAP,
	rpMATFXEFFECTBUMPENVMAP = rw::MatFX::BUMPENVMAP,
	rpMATFXEFFECTDUAL       = rw::MatFX::DUAL,

	rpMATFXEFFECTMAX,
	rpMATFXNUMEFFECTS       = rpMATFXEFFECTMAX - 1,
};

RwBool RpMatFXPluginAttach( void );
RpAtomic *RpMatFXAtomicEnableEffects( RpAtomic *atomic );
RwBool RpMatFXAtomicQueryEffects( RpAtomic *atomic );
//RpWorldSector *RpMatFXWorldSectorEnableEffects( RpWorldSector *worldSector );
//RwBool RpMatFXWorldSectorQueryEffects( RpWorldSector *worldSector );
RpMaterial *RpMatFXMaterialSetEffects( RpMaterial *material, RpMatFXMaterialFlags flags );
RpMaterial *RpMatFXMaterialSetupBumpMap( RpMaterial *material, RwTexture *texture, RwFrame *frame, RwReal coef );
RpMaterial *RpMatFXMaterialSetupEnvMap( RpMaterial *material, RwTexture *texture, RwFrame *frame, RwBool useFrameBufferAlpha, RwReal coef );
RpMaterial *RpMatFXMaterialSetupDualTexture( RpMaterial *material, RwTexture *texture, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode );
RpMatFXMaterialFlags RpMatFXMaterialGetEffects( const RpMaterial *material );
RpMaterial *RpMatFXMaterialSetBumpMapTexture( RpMaterial *material, RwTexture *texture );
RpMaterial *RpMatFXMaterialSetBumpMapFrame( RpMaterial *material, RwFrame *frame );
RpMaterial *RpMatFXMaterialSetBumpMapCoefficient( RpMaterial *material, RwReal coef );
RwTexture *RpMatFXMaterialGetBumpMapTexture( const RpMaterial *material );
RwTexture *RpMatFXMaterialGetBumpMapBumpedTexture( const RpMaterial *material );
RwFrame *RpMatFXMaterialGetBumpMapFrame( const RpMaterial *material );
RwReal RpMatFXMaterialGetBumpMapCoefficient( const RpMaterial *material );
RpMaterial *RpMatFXMaterialSetEnvMapTexture( RpMaterial *material, RwTexture *texture );
RpMaterial *RpMatFXMaterialSetEnvMapFrame( RpMaterial *material, RwFrame *frame );
RpMaterial *RpMatFXMaterialSetEnvMapFrameBufferAlpha( RpMaterial *material, RwBool useFrameBufferAlpha );
RpMaterial *RpMatFXMaterialSetEnvMapCoefficient( RpMaterial *material, RwReal coef );
RwTexture *RpMatFXMaterialGetEnvMapTexture( const RpMaterial *material );
RwFrame *RpMatFXMaterialGetEnvMapFrame( const RpMaterial *material );
RwBool RpMatFXMaterialGetEnvMapFrameBufferAlpha( const RpMaterial *material );
RwReal RpMatFXMaterialGetEnvMapCoefficient( const RpMaterial *material );
RpMaterial *RpMatFXMaterialSetDualTexture( RpMaterial *material, RwTexture *texture );
RpMaterial *RpMatFXMaterialSetDualBlendModes( RpMaterial *material, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode );
RwTexture *RpMatFXMaterialGetDualTexture( const RpMaterial *material );
const RpMaterial *RpMatFXMaterialGetDualBlendModes( const RpMaterial *material, RwBlendFunction *srcBlendMode, RwBlendFunction *dstBlendMode );

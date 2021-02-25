#pragma once

#include <rphanim.h>

//struct RpSkin;
typedef rw::Skin RpSkin;

struct RwMatrixWeights
{
	RwReal w0;
	RwReal w1;
	RwReal w2;
	RwReal w3;
};

RwBool RpSkinPluginAttach(void);

RwUInt32 RpSkinGetNumBones( RpSkin *skin );
const RwMatrixWeights *RpSkinGetVertexBoneWeights( RpSkin *skin );
const RwUInt32 *RpSkinGetVertexBoneIndices( RpSkin *skin );
const RwMatrix *RpSkinGetSkinToBoneMatrices( RpSkin *skin );

RpSkin *RpSkinGeometryGetSkin( RpGeometry *geometry );

RpAtomic *RpSkinAtomicSetHAnimHierarchy( RpAtomic *atomic, RpHAnimHierarchy *hierarchy );
RpHAnimHierarchy *RpSkinAtomicGetHAnimHierarchy( const RpAtomic *atomic );

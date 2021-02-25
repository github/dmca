#pragma once

// Same layout as rw::Quat but with ugly imag,real separation which i don't want in librw
struct RtQuat
{
	rw::V3d imag;
	rw::float32 real;
};

RwBool RtQuatConvertFromMatrix(RtQuat * const qpQuat, const RwMatrix * const mpMatrix);
RtQuat *RtQuatRotate(RtQuat * quat, const RwV3d * axis, RwReal angle, RwOpCombineType combineOp);
const RtQuat *RtQuatQueryRotate(const RtQuat *quat, RwV3d * unitAxis, RwReal * angle);
RwV3d *RtQuatTransformVectors(RwV3d * vectorsOut, const RwV3d * vectorsIn, const RwInt32 numPoints, const RtQuat *quat);

void RtQuatConvertToMatrix(const RtQuat * const qpQuat, RwMatrix * const mpMatrix);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#define PSEP_C '/'
#define PSEP_S "/"
#ifdef __unix__
#include <sys/types.h>
#include <dirent.h>
#endif

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

namespace rw {

#define PLUGIN_ID 0

int32 version = 0x36003;
int32 build = 0xFFFF;
#ifdef RW_PS2
	int32 platform = PLATFORM_PS2;
#elif RW_WDGL
	int32 platform = PLATFORM_WDGL;
#elif RW_GL3
	int32 platform = PLATFORM_GL3;
#elif RW_D3D9
	int32 platform = PLATFORM_D3D9;
#else
	int32 platform = PLATFORM_NULL;
#endif
bool32 streamAppendFrames = 0;
char *debugFile = nil;

static Matrix identMat = {
	{ 1.0f, 0.0f, 0.0f }, Matrix::IDENTITY|Matrix::TYPEORTHONORMAL,
	{ 0.0f, 1.0f, 0.0f }, 0,
	{ 0.0f, 0.0f, 1.0f }, 0,
	{ 0.0f, 0.0f, 0.0f }, 0
};

// lazy implementation
int
strcmp_ci(const char *s1, const char *s2)
{
	char c1, c2;
	for(;;){
		c1 = tolower(*s1);
		c2 = tolower(*s2);
		if(c1 != c2)
			return c1 - c2;
		if(c1 == '\0')
			return 0;
		s1++;
		s2++;
	}
}

int
strncmp_ci(const char *s1, const char *s2, int n)
{
	char c1, c2;
	while(n--){
		c1 = tolower(*s1);
		c2 = tolower(*s2);
		if(c1 != c2)
			return c1 - c2;
		if(c1 == '\0')
			return 0;
		s1++;
		s2++;
	}
	return 0;
}

Quat
mult(const Quat &q, const Quat &p)
{
	return makeQuat(q.w*p.w - q.x*p.x - q.y*p.y - q.z*p.z,
	                q.w*p.x + q.x*p.w + q.y*p.z - q.z*p.y,
	                q.w*p.y + q.y*p.w + q.z*p.x - q.x*p.z,
	                q.w*p.z + q.z*p.w + q.x*p.y - q.y*p.x);
}


Quat*
Quat::rotate(const V3d *axis, float32 angle, CombineOp op)
{
	Quat rot = rotation(angle, *axis);
	switch(op){
	case COMBINEREPLACE:
		*this = rot;
		break;
	case COMBINEPRECONCAT:
		*this = mult(*this, rot);
		break;
	case COMBINEPOSTCONCAT:
		*this = mult(rot, *this);
		break;
	}
	return this;
}

Quat
lerp(const Quat &q, const Quat &p, float32 r)
{
	float32 c;
	Quat q1 = q;
	c = dot(q1, p);
	if(c < 0.0f){
		c = -c;
		q1 = negate(q1);
	}
	return makeQuat(q1.w + r*(p.w - q1.w),
	                q1.x + r*(p.x - q1.x),
	                q1.y + r*(p.y - q1.y),
	                q1.z + r*(p.z - q1.z));
};

Quat
slerp(const Quat &q, const Quat &p, float32 a)
{
	float32 c;
	Quat q1 = q;
	c = dot(q1, p);
	if(c < 0.0f){
		c = -c;
		q1 = negate(q1);
	}
	float32 phi = acosf(c);
	if(phi > 0.00001f){
		float32 s = sinf(phi);
		return add(scale(q1, sinf((1.0f-a)*phi)/s),
		           scale(p,  sinf(a*phi)/s));
	}
	return q1;
}

//
// V3d
//

V3d
cross(const V3d &a, const V3d &b)
{
	return makeV3d(a.y*b.z - a.z*b.y,
	               a.z*b.x - a.x*b.z,
	               a.x*b.y - a.y*b.x);
}

void
V3d::transformPoints(V3d *out, const V3d *in, int32 n, const Matrix *m)
{
	int32 i;
	V3d tmp;
	for(i = 0; i < n; i++){
		tmp.x = in[i].x*m->right.x + in[i].y*m->up.x + in[i].z*m->at.x + m->pos.x;
		tmp.y = in[i].x*m->right.y + in[i].y*m->up.y + in[i].z*m->at.y + m->pos.y;
		tmp.z = in[i].x*m->right.z + in[i].y*m->up.z + in[i].z*m->at.z + m->pos.z;
		out[i] = tmp;
	}
}

void
V3d::transformVectors(V3d *out, const V3d *in, int32 n, const Matrix *m)
{
	int32 i;
	V3d tmp;
	for(i = 0; i < n; i++){
		tmp.x = in[i].x*m->right.x + in[i].y*m->up.x + in[i].z*m->at.x;
		tmp.y = in[i].x*m->right.y + in[i].y*m->up.y + in[i].z*m->at.y;
		tmp.z = in[i].x*m->right.z + in[i].y*m->up.z + in[i].z*m->at.z;
		out[i] = tmp;
	}
}

//
// RawMatrix
//

void
RawMatrix::mult(RawMatrix *dst, RawMatrix *src1, RawMatrix *src2)
{
	dst->right.x = src1->right.x*src2->right.x + src1->right.y*src2->up.x + src1->right.z*src2->at.x + src1->rightw*src2->pos.x;
	dst->right.y = src1->right.x*src2->right.y + src1->right.y*src2->up.y + src1->right.z*src2->at.y + src1->rightw*src2->pos.y;
	dst->right.z = src1->right.x*src2->right.z + src1->right.y*src2->up.z + src1->right.z*src2->at.z + src1->rightw*src2->pos.z;
	dst->rightw  = src1->right.x*src2->rightw  + src1->right.y*src2->upw  + src1->right.z*src2->atw  + src1->rightw*src2->posw;
	dst->up.x    = src1->up.x*src2->right.x    + src1->up.y*src2->up.x    + src1->up.z*src2->at.x + src1->upw*src2->pos.x;
	dst->up.y    = src1->up.x*src2->right.y    + src1->up.y*src2->up.y    + src1->up.z*src2->at.y + src1->upw*src2->pos.y;
	dst->up.z    = src1->up.x*src2->right.z    + src1->up.y*src2->up.z    + src1->up.z*src2->at.z + src1->upw*src2->pos.z;
	dst->upw     = src1->up.x*src2->rightw     + src1->up.y*src2->upw     + src1->up.z*src2->atw  + src1->upw*src2->posw;
	dst->at.x    = src1->at.x*src2->right.x    + src1->at.y*src2->up.x    + src1->at.z*src2->at.x + src1->atw*src2->pos.x;
	dst->at.y    = src1->at.x*src2->right.y    + src1->at.y*src2->up.y    + src1->at.z*src2->at.y + src1->atw*src2->pos.y;
	dst->at.z    = src1->at.x*src2->right.z    + src1->at.y*src2->up.z    + src1->at.z*src2->at.z + src1->atw*src2->pos.z;
	dst->atw     = src1->at.x*src2->rightw     + src1->at.y*src2->upw     + src1->at.z*src2->atw  + src1->atw*src2->posw;
	dst->pos.x   = src1->pos.x*src2->right.x   + src1->pos.y*src2->up.x   + src1->pos.z*src2->at.x + src1->posw*src2->pos.x;
	dst->pos.y   = src1->pos.x*src2->right.y   + src1->pos.y*src2->up.y   + src1->pos.z*src2->at.y + src1->posw*src2->pos.y;
	dst->pos.z   = src1->pos.x*src2->right.z   + src1->pos.y*src2->up.z   + src1->pos.z*src2->at.z + src1->posw*src2->pos.z;
	dst->posw    = src1->pos.x*src2->rightw    + src1->pos.y*src2->upw    + src1->pos.z*src2->atw  + src1->posw*src2->posw;
}

void
RawMatrix::transpose(RawMatrix *dst, RawMatrix *src)
{
	dst->right.x = src->right.x;
	dst->up.x = src->right.y;
	dst->at.x = src->right.z;
	dst->pos.x = src->rightw;
	dst->right.y = src->up.x;
	dst->up.y = src->up.y;
	dst->at.y = src->up.z;
	dst->pos.y = src->upw;
	dst->right.z = src->at.x;
	dst->up.z = src->at.y;
	dst->at.z = src->at.z;
	dst->pos.z = src->atw;
	dst->rightw = src->pos.x;
	dst->upw = src->pos.y;
	dst->atw = src->pos.z;
	dst->posw = src->posw;
}

void
RawMatrix::setIdentity(RawMatrix *dst)
{
	static RawMatrix identity = {
		{ 1.0f, 0.0f, 0.0f }, 0.0f,
		{ 0.0f, 1.0f, 0.0f }, 0.0f,
		{ 0.0f, 0.0f, 1.0f }, 0.0f,
		{ 0.0f, 0.0f, 0.0f }, 1.0f
	};
	*dst = identity;
}

//
// Matrix
//

static Matrix::Tolerance matrixDefaultTolerance = { 0.01f, 0.01f, 0.01f };

Matrix*
Matrix::create(void)
{
	Matrix *m = (Matrix*)rwMalloc(sizeof(Matrix), MEMDUR_EVENT | ID_MATRIX);
	m->setIdentity();
	return m;
}

void
Matrix::destroy(void)
{
	rwFree(this);
}

void
Matrix::setIdentity(void)
{
	*this = identMat;
}

void
Matrix::optimize(Tolerance *tolerance)
{
	bool32 isnormal, isorthogonal, isidentity;
	if(tolerance == nil)
		tolerance = &matrixDefaultTolerance;
	isnormal = normalError() <= tolerance->normal;
	isorthogonal = orthogonalError() <= tolerance->orthogonal;
	isidentity = isnormal && isorthogonal && identityError() <= tolerance->identity;
	if(isnormal)
		flags |= TYPENORMAL;
	else
		flags &= ~TYPENORMAL;
	if(isorthogonal)
		flags |= TYPEORTHOGONAL;
	else
		flags &= ~TYPEORTHOGONAL;
	if(isidentity)
		flags |= IDENTITY;
	else
		flags &= ~IDENTITY;
}

Matrix*
Matrix::mult(Matrix *dst, const Matrix *src1, const Matrix *src2)
{
	if(src1->flags & IDENTITY)
		*dst = *src2;
	else if(src2->flags & IDENTITY)
		*dst = *src1;
	else{
		mult_(dst, src1, src2);
		dst->flags = src1->flags & src2->flags;
	}
	return dst;
}

Matrix*
Matrix::invert(Matrix *dst, const Matrix *src)
{
	if(src->flags & IDENTITY)
		*dst = *src;
	else if((src->flags & TYPEMASK) == TYPEORTHONORMAL)
		invertOrthonormal(dst, src);
	else
		return invertGeneral(dst, src);
	return dst;
}

// transpose the 3x3 submatrix, pos is set to 0
Matrix*
Matrix::transpose(Matrix *dst, const Matrix *src)
{
	if(src->flags & IDENTITY)
		*dst = *src;
	dst->right.x = src->right.x;
	dst->up.x = src->right.y;
	dst->at.x = src->right.z;
	dst->right.y = src->up.x;
	dst->up.y = src->up.y;
	dst->at.y = src->up.z;
	dst->right.z = src->at.x;
	dst->up.z = src->at.y;
	dst->at.z = src->at.z;
	dst->pos.x = 0.0;
	dst->pos.y = 0.0;
	dst->pos.z = 0.0;
	return dst;
}

Matrix*
Matrix::rotate(const V3d *axis, float32 angle, CombineOp op)
{
	Matrix tmp, rot;
	makeRotation(&rot, axis, angle);
	switch(op){
	case COMBINEREPLACE:
		*this = rot;
		break;
	case COMBINEPRECONCAT:
		mult(&tmp, &rot, this);
		*this = tmp;
		break;
	case COMBINEPOSTCONCAT:
		mult(&tmp, this, &rot);
		*this = tmp;
		break;
	}
	return this;
}

Matrix*
Matrix::rotate(const Quat &q, CombineOp op)
{
	Matrix tmp, rot;
	makeRotation(&rot, q);
	switch(op){
	case COMBINEREPLACE:
		*this = rot;
		break;
	case COMBINEPRECONCAT:
		mult(&tmp, &rot, this);
		*this = tmp;
		break;
	case COMBINEPOSTCONCAT:
		mult(&tmp, this, &rot);
		*this = tmp;
		break;
	}
	return this;
}
Matrix*
Matrix::translate(const V3d *translation, CombineOp op)
{
	Matrix tmp;
	Matrix trans = identMat;
	trans.pos = *translation;
	trans.flags &= ~IDENTITY;
	switch(op){
	case COMBINEREPLACE:
		*this = trans;
		break;
	case COMBINEPRECONCAT:
		mult(&tmp, &trans, this);
		*this = tmp;
		break;
	case COMBINEPOSTCONCAT:
		mult(&tmp, this, &trans);
		*this = tmp;
		break;
	}
	return this;
}

Matrix*
Matrix::scale(const V3d *scale, CombineOp op)
{
	Matrix tmp;
	Matrix scl = identMat;
	scl.right.x = scale->x;
	scl.up.y = scale->y;
	scl.at.z = scale->z;
	scl.flags &= ~IDENTITY;
	switch(op){
	case COMBINEREPLACE:
		*this = scl;
		break;
	case COMBINEPRECONCAT:
		mult(&tmp, &scl, this);
		*this = tmp;
		break;
	case COMBINEPOSTCONCAT:
		mult(&tmp, this, &scl);
		*this = tmp;
		break;
	}
	return this;
}

Matrix*
Matrix::transform(const Matrix *mat, CombineOp op)
{
	Matrix tmp;
	switch(op){
	case COMBINEREPLACE:
		*this = *mat;
		break;
	case COMBINEPRECONCAT:
		mult(&tmp, mat, this);
		*this = tmp;
		break;
	case COMBINEPOSTCONCAT:
		mult(&tmp, this, mat);
		*this = tmp;
		break;
	}
	return this;
}

Quat
Matrix::getRotation(void)
{
	Quat q = { 0.0f, 0.0f, 0.0f, 1.0f };
	float32 tr = right.x + up.y + at.z;
	float s;
	if(tr > 0.0f){
		s = sqrtf(1.0f + tr) * 2.0f;
		q.w = s / 4.0f;
		q.x = (up.z - at.y) / s;
		q.y = (at.x - right.z) / s;
		q.z = (right.y - up.x) / s;
	}else if(right.x > up.y && right.x > at.z){
		s = sqrtf(1.0f + right.x - up.y - at.z) * 2.0f;
		q.w = (up.z - at.y) / s;
		q.x = s / 4.0f;
		q.y = (up.x + right.y) / s;
		q.z = (at.x + right.z) / s;
	}else if(up.y > at.z){
		s = sqrtf(1.0f + up.y - right.x - at.z) * 2.0f;
		q.w = (at.x - right.z) / s;
		q.x = (up.x + right.y) / s;
		q.y = s / 4.0f;
		q.z = (at.y + up.z) / s;
	}else{
		s = sqrtf(1.0f + at.z - right.x - up.y) * 2.0f;
		q.w = (right.y - up.x) / s;
		q.x = (at.x + right.z) / s;
		q.y = (at.y + up.z) / s;
		q.z = s / 4.0f;
	}
	return q;
}

void
Matrix::lookAt(const V3d &dir, const V3d &up)
{
	// this->right is really pointing left
	this->at = normalize(dir);
	this->right = normalize(cross(up, this->at));
	this->up = cross(this->at, this->right);
	this->flags = TYPEORTHONORMAL;
}

/* For a row-major representation, this calculates src1 * src2.
 * For column-major src2 * src1.
 * i.e. a vector is first xformed by src1, then by src2
 */
void
Matrix::mult_(Matrix *dst, const Matrix *src1, const Matrix *src2)
{
	dst->right.x = src1->right.x*src2->right.x + src1->right.y*src2->up.x + src1->right.z*src2->at.x;
	dst->right.y = src1->right.x*src2->right.y + src1->right.y*src2->up.y + src1->right.z*src2->at.y;
	dst->right.z = src1->right.x*src2->right.z + src1->right.y*src2->up.z + src1->right.z*src2->at.z;
	dst->up.x    = src1->up.x*src2->right.x    + src1->up.y*src2->up.x    + src1->up.z*src2->at.x;
	dst->up.y    = src1->up.x*src2->right.y    + src1->up.y*src2->up.y    + src1->up.z*src2->at.y;
	dst->up.z    = src1->up.x*src2->right.z    + src1->up.y*src2->up.z    + src1->up.z*src2->at.z;
	dst->at.x    = src1->at.x*src2->right.x    + src1->at.y*src2->up.x    + src1->at.z*src2->at.x;
	dst->at.y    = src1->at.x*src2->right.y    + src1->at.y*src2->up.y    + src1->at.z*src2->at.y;
	dst->at.z    = src1->at.x*src2->right.z    + src1->at.y*src2->up.z    + src1->at.z*src2->at.z;
	dst->pos.x   = src1->pos.x*src2->right.x   + src1->pos.y*src2->up.x   + src1->pos.z*src2->at.x + src2->pos.x;
	dst->pos.y   = src1->pos.x*src2->right.y   + src1->pos.y*src2->up.y   + src1->pos.z*src2->at.y + src2->pos.y;
	dst->pos.z   = src1->pos.x*src2->right.z   + src1->pos.y*src2->up.z   + src1->pos.z*src2->at.z + src2->pos.z;
}

void
Matrix::invertOrthonormal(Matrix *dst, const Matrix *src)
{
	dst->right.x = src->right.x;
	dst->right.y = src->up.x;
	dst->right.z = src->at.x;
	dst->up.x = src->right.y;
	dst->up.y = src->up.y;
	dst->up.z = src->at.y;
	dst->at.x = src->right.z;
	dst->at.y = src->up.z;
	dst->at.z = src->at.z;
	dst->pos.x = -(src->pos.x*src->right.x +
	               src->pos.y*src->right.y +
	               src->pos.z*src->right.z);
	dst->pos.y = -(src->pos.x*src->up.x +
	               src->pos.y*src->up.y +
	               src->pos.z*src->up.z);
	dst->pos.z = -(src->pos.x*src->at.x +
	               src->pos.y*src->at.y +
	               src->pos.z*src->at.z);
	dst->flags = TYPEORTHONORMAL;
}

Matrix*
Matrix::invertGeneral(Matrix *dst, const Matrix *src)
{
	float32 det, invdet;
	// calculate a few cofactors
	dst->right.x = src->up.y*src->at.z - src->up.z*src->at.y;
	dst->right.y = src->at.y*src->right.z - src->at.z*src->right.y;
	dst->right.z = src->right.y*src->up.z - src->right.z*src->up.y;
	// get the determinant from that
	det = src->up.x * dst->right.y + src->at.x * dst->right.z + dst->right.x * src->right.x;
	invdet = 1.0;
	if(det != 0.0f)
		invdet = 1.0f/det;
	dst->right.x *= invdet;
	dst->right.y *= invdet;
	dst->right.z *= invdet;
	dst->up.x = invdet * (src->up.z*src->at.x - src->up.x*src->at.z);
	dst->up.y = invdet * (src->at.z*src->right.x - src->at.x*src->right.z);
	dst->up.z = invdet * (src->right.z*src->up.x - src->right.x*src->up.z);
	dst->at.x = invdet * (src->up.x*src->at.y - src->up.y*src->at.x);
	dst->at.y = invdet * (src->at.x*src->right.y - src->at.y*src->right.x);
	dst->at.z = invdet * (src->right.x*src->up.y - src->right.y*src->up.x);
	dst->pos.x = -(src->pos.x*dst->right.x + src->pos.y*dst->up.x + src->pos.z*dst->at.x);
	dst->pos.y = -(src->pos.x*dst->right.y + src->pos.y*dst->up.y + src->pos.z*dst->at.y);
	dst->pos.z = -(src->pos.x*dst->right.z + src->pos.y*dst->up.z + src->pos.z*dst->at.z);
	dst->flags &= ~IDENTITY;
	return dst;
}

void
Matrix::makeRotation(Matrix *dst, const V3d *axis, float32 angle)
{
//	V3d v = normalize(*axis);
	float32 len = dot(*axis, *axis);
	if(len != 0.0f) len = 1.0f/sqrtf(len);
	V3d v = rw::scale(*axis, len);
	angle = angle*(float)M_PI/180.0f;
	float32 s = sinf(angle);
	float32 c = cosf(angle);
	float32 t = 1.0f - c;

	dst->right.x = c + v.x*v.x*t;
	dst->right.y = v.x*v.y*t + v.z*s;
	dst->right.z = v.z*v.x*t - v.y*s;
	dst->up.x = v.x*v.y*t - v.z*s;
	dst->up.y = c + v.y*v.y*t;
	dst->up.z = v.y*v.z*t + v.x*s;
	dst->at.x = v.z*v.x*t + v.y*s;
	dst->at.y = v.y*v.z*t - v.x*s;
	dst->at.z = c + v.z*v.z*t;
	dst->pos.x = 0.0;
	dst->pos.y = 0.0;
	dst->pos.z = 0.0;
	dst->flags = TYPEORTHONORMAL;
}

/* q must be normalized */
void
Matrix::makeRotation(Matrix *dst, const Quat &q)
{
	float xx = q.x*q.x;
	float yy = q.y*q.y;
	float zz = q.z*q.z;
	float yz = q.y*q.z;
	float zx = q.z*q.x;
	float xy = q.x*q.y;
	float wx = q.w*q.x;
	float wy = q.w*q.y;
	float wz = q.w*q.z;

	dst->right.x = 1.0f - 2.0f*(yy + zz);
	dst->right.y =        2.0f*(xy + wz);
	dst->right.z =        2.0f*(zx - wy);
	dst->up.x =        2.0f*(xy - wz);
	dst->up.y = 1.0f - 2.0f*(xx + zz);
	dst->up.z =        2.0f*(yz + wx);
	dst->at.x =        2.0f*(zx + wy);
	dst->at.y =        2.0f*(yz - wx);
	dst->at.z = 1.0f - 2.0f*(xx + yy);
	dst->pos.x = 0.0;
	dst->pos.y = 0.0;
	dst->pos.z = 0.0;
	dst->flags = TYPEORTHONORMAL;
}

float32
Matrix::normalError(void)
{
	float32 x, y, z;
	x = dot(right, right) - 1.0f;
	y = dot(up, up) - 1.0f;
	z = dot(at, at) - 1.0f;
	return x*x + y*y + z*z;
}

float32
Matrix::orthogonalError(void)
{
	float32 x, y, z;
	x = dot(at, up);
	y = dot(at, right);
	z = dot(up, right);
	return x*x + y*y + z*z;
}

float32
Matrix::identityError(void)
{
	V3d r = { right.x-1.0f, right.y, right.z };
	V3d u = { up.x, up.y-1.0f, up.z };
	V3d a = { at.x, at.y, at.z-1.0f };
	return dot(r,r) + dot(u,u) + dot(a,a) + dot(pos,pos);
}

void
correctPathCase(char *filename)
{
#ifdef __unix__
	DIR *direct;
	struct dirent *dirent;

	char *dir, *arg;
	char copy[1024], sofar[1024] = ".";
	strncpy(copy, filename, 1024);
	arg = copy;
	// hack for absolute paths
	if(filename[0] == '/'){
		sofar[0] = '/';
		sofar[1] = '/';
		sofar[2] = '\0';
		arg++;
	}
	while((dir = strtok(arg, PSEP_S))){
		arg = nil;
		if(direct = opendir(sofar), dir == nil)
			return;
		while(dirent = readdir(direct), dirent != nil)
			if(strncmp_ci(dirent->d_name, dir, 1024) == 0){
				strncat(sofar, PSEP_S, 1024);
				strncat(sofar, dirent->d_name, 1024);
				break;
			}
		closedir(direct);
		if(dirent == nil)
			return;
	}
	strcpy(filename, sofar+2);
#endif
}

void
makePath(char *filename)
{
	size_t len = strlen(filename);
	for(size_t i = 0; i < len; i++)
		if(filename[i] == '/' || filename[i] == '\\')
			filename[i] = PSEP_C;
#ifdef __unix__
	correctPathCase(filename);
#endif
}

void
memNative32_func(void *data, uint32 size)
{
	uint8 *bytes = (uint8*)data;
	uint32 *words = (uint32*)data;
	size >>= 2;
	while(size--){
		*words++ = (uint32)bytes[0] | (uint32)bytes[1]<<8 |
			(uint32)bytes[2]<<16 | (uint32)bytes[3]<<24;
		bytes += 4;
	}
}

void
memNative16_func(void *data, uint32 size)
{
	uint8 *bytes = (uint8*)data;
	uint16 *words = (uint16*)data;
	size >>= 1;
	while(size--){
		*words++ = (uint16)bytes[0] | (uint16)bytes[1]<<8;
		bytes += 2;
	}
}

void
memLittle32_func(void *data, uint32 size)
{
	uint32 w;
	uint8 *bytes = (uint8*)data;
	uint32 *words = (uint32*)data;
	size >>= 2;
	while(size--){
		w = *words++;
		*bytes++ = w;
		*bytes++ = w >> 8;
		*bytes++ = w >> 16;
		*bytes++ = w >> 24;
	}
}

void
memLittle16_func(void *data, uint32 size)
{
	uint16 w;
	uint8 *bytes = (uint8*)data;
	uint16 *words = (uint16*)data;
	size >>= 1;
	while(size--){
		w = *words++;
		*bytes++ = (uint8)w;
		*bytes++ = w >> 8;
	}
}

uint32
Stream::write32(const void *data, uint32 length)
{
#ifdef BIGENDIAN
	uint8 *src = (uint8*)data;
	uint32 buf[256];
	int32 n, len;
	for(len = length >>= 2; len > 0; len -= 256){
		n = len < 256 ? len : 256;
		memcpy(buf, src, n*4);
		memLittle32(buf, n*4);
		write8(buf, n*4);
		src += n*4;
	}
	return length;
#else
	return write8(data, length);
#endif
}

uint32
Stream::write16(const void *data, uint32 length)
{
#ifdef BIGENDIAN
	uint8 *src = (uint8*)data;
	uint16 buf[256];
	int32 n, len;
	for(len = length >>= 1; len > 0; len -= 256){
		n = len < 256 ? len : 256;
		memcpy(buf, src, n*2);
		memLittle16(buf, n*2);
		write8(buf, n*2);
		src += n*2;
	}
	return length;
#else
	return write8(data, length);
#endif
}

uint32
Stream::read32(void *data, uint32 length)
{
	uint32 ret;
	ret = read8(data, length);
	memNative32(data, length);
	return ret;
}

uint32
Stream::read16(void *data, uint32 length)
{
	uint32 ret;
	ret = read8(data, length);
	memNative16(data, length);
	return ret;
}

int32
Stream::writeI8(int8 val)
{
	return write8(&val, sizeof(int8));
}

int32
Stream::writeU8(uint8 val)
{
	return write8(&val, sizeof(uint8));
}

int32
Stream::writeI16(int16 val)
{
	return write16(&val, sizeof(int16));
}

int32
Stream::writeU16(uint16 val)
{
	return write16(&val, sizeof(uint16));
}

int32
Stream::writeI32(int32 val)
{
	return write32(&val, sizeof(int32));
}

int32
Stream::writeU32(uint32 val)
{
	return write32(&val, sizeof(uint32));
}

int32
Stream::writeF32(float32 val)
{
	return write32(&val, sizeof(float32));
}

int8
Stream::readI8(void)
{
	int8 tmp;
	read8(&tmp, sizeof(int8));
	return tmp;
}

uint8
Stream::readU8(void)
{
	uint8 tmp;
	read8(&tmp, sizeof(uint8));
	return tmp;
}

int16
Stream::readI16(void)
{
	int16 tmp;
	read16(&tmp, sizeof(int16));
	return tmp;
}

uint16
Stream::readU16(void)
{
	uint16 tmp;
	read16(&tmp, sizeof(uint16));
	return tmp;
}

int32
Stream::readI32(void)
{
	int32 tmp;
	read32(&tmp, sizeof(int32));
	return tmp;
}

uint32
Stream::readU32(void)
{
	uint32 tmp;
	read32(&tmp, sizeof(uint32));
	return tmp;
}

float32
Stream::readF32(void)
{
	float32 tmp;
	read32(&tmp, sizeof(float32));
	return tmp;
}



void
StreamMemory::close(void)
{
}

uint32
StreamMemory::write8(const void *data, uint32 len)
{
	if(this->eof())
		return 0;
	uint32 l = len;
	if(this->position+l > this->length){
		if(this->position+l > this->capacity)
			l = this->capacity-this->position;
		this->length = this->position+l;
	}
	memcpy(&this->data[this->position], data, l);
	this->position += l;
	if(len != l)
		this->position = S_EOF;
	return l;
}

uint32
StreamMemory::read8(void *data, uint32 len)
{
	if(this->eof())
		return 0;
	uint32 l = len;
	if(this->position+l > this->length)
		l = this->length-this->position;
	memcpy(data, &this->data[this->position], l);
	this->position += l;
	if(len != l)
		this->position = S_EOF;
	return l;
}

void
StreamMemory::seek(int32 offset, int32 whence)
{
	if(whence == 0)
		this->position = offset;
	else if(whence == 1)
		this->position += offset;
	else
		this->position = this->length-offset;
	if(this->position > this->length){
		// TODO: ideally this would depend on the mode
		if(this->position > this->capacity)
			this->position = S_EOF;
		else
			this->length = this->position;
	}
}

uint32
StreamMemory::tell(void)
{
	return this->position;
}

bool
StreamMemory::eof(void)
{
	return this->position == S_EOF;
}

StreamMemory*
StreamMemory::open(uint8 *data, uint32 length, uint32 capacity)
{
	this->data = data;
	this->capacity = capacity;
	this->length = length;
	if(this->capacity < this->length)
		this->capacity = this->length;
	this->position = 0;
	return this;
}

uint32
StreamMemory::getLength(void)
{
	return this->length;
}


StreamFile*
StreamFile::open(const char *path, const char *mode)
{
	assert(this->file == nil);
	this->file = fopen(path, mode);
	if(this->file == nil){
		RWERROR((ERR_FILE, path));
		return nil;
	}
	return this;
}

void
StreamFile::close(void)
{
	assert(this->file);
	fclose(this->file);
	this->file = nil;
}

uint32
StreamFile::write8(const void *data, uint32 length)
{
	return (uint32)fwrite(data, 1, length, this->file);
}

uint32
StreamFile::read8(void *data, uint32 length)
{
	return (uint32)fread(data, 1, length, this->file);
}

void
StreamFile::seek(int32 offset, int32 whence)
{
	fseek(this->file, offset, whence);
}

uint32
StreamFile::tell(void)
{
	return ftell(this->file);
}

bool
StreamFile::eof(void)
{
	return ( feof(this->file) != 0 );
}

bool
writeChunkHeader(Stream *s, int32 type, int32 size)
{
	struct {
		int32 type, size;
		uint32 id;
	} buf = { type, size, libraryIDPack(version, build) };
	s->write32(&buf, 12);
	return true;
}

bool
readChunkHeaderInfo(Stream *s, ChunkHeaderInfo *header)
{
	struct {
		int32 type, size;
		uint32 id;
	} buf;
	s->read32(&buf, 12);
	if(s->eof())
		return false;
	assert(header != nil);
	header->type = buf.type;
	header->length = buf.size;
	header->version = libraryIDUnpackVersion(buf.id);
	header->build = libraryIDUnpackBuild(buf.id);
	return true;
}

bool
findChunk(Stream *s, uint32 type, uint32 *length, uint32 *version)
{
	ChunkHeaderInfo header;
	while(readChunkHeaderInfo(s, &header)){
		if(header.type == ID_NAOBJECT)
			return false;
		if(header.type == type){
			if(length)
				*length = header.length;
			if(version)
				*version = header.version;
			return true;
		}
		s->seek(header.length);
	}
	return false;
}

int32
findPointer(void *p, void **list, int32 num)
{
	int i;
	for(i = 0; i < num; i++)
		if(list[i] == p)
			return i;
	return -1;
}

uint8*
getFileContents(const char *name, uint32 *len)
{
	FILE *cf = fopen(name, "rb");
	if(cf == nil)
		return nil;
	fseek(cf, 0, SEEK_END);
	*len = ftell(cf);
	fseek(cf, 0, SEEK_SET);
	uint8 *data = rwNewT(uint8, *len, MEMDUR_EVENT);
	fread(data, 1, *len, cf);
	fclose(cf);
	return data;
}

}

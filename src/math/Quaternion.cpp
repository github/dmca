#include "common.h"
#include "Quaternion.h"

void
CQuaternion::Normalise(void)
{
	float sq = MagnitudeSqr();
	if (sq == 0.0f)
		w = 1.0f;
	else {
		float invsqrt = RecipSqrt(sq);
		x *= invsqrt;
		y *= invsqrt;
		z *= invsqrt;
		w *= invsqrt;
	}
}

void
CQuaternion::Slerp(const CQuaternion &q1, const CQuaternion &q2, float theta, float invSin, float t)
{
	if (theta == 0.0f)
		*this = q2;
	else {
		float w1, w2;
		if (theta > PI / 2) {
			theta = PI - theta;
			w1 = Sin((1.0f - t) * theta) * invSin;
			w2 = -Sin(t * theta) * invSin;
		} else {
			w1 = Sin((1.0f - t) * theta) * invSin;
			w2 = Sin(t * theta) * invSin;
		}
		// TODO: VU0 code
		*this = w1 * q1 + w2 * q2;
	}
}

void
CQuaternion::Multiply(const CQuaternion &q1, const CQuaternion &q2)
{
	x = (q2.z * q1.y) - (q1.z * q2.y) + (q1.x * q2.w) + (q2.x * q1.w);
	y = (q2.x * q1.z) - (q1.x * q2.z) + (q1.y * q2.w) + (q2.y * q1.w);
	z = (q2.y * q1.x) - (q1.y * q2.x) + (q1.z * q2.w) + (q2.z * q1.w);
	w = (q2.w * q1.w) - (q2.x * q1.x) - (q2.y * q1.y) - (q2.z * q1.z);
}

void
CQuaternion::Get(RwV3d *axis, float *angle)
{
	*angle = Acos(w);
	float s = Sin(*angle);

	axis->x = x * (1.0f / s);
	axis->y = y * (1.0f / s);
	axis->z = z * (1.0f / s);
}

void
CQuaternion::Set(RwV3d *axis, float angle)
{
	float halfCos = Cos(angle * 0.5f);
	float halfSin = Sin(angle * 0.5f);
	x = axis->x * halfSin;
	y = axis->y * halfSin;
	z = axis->z * halfSin;
	w = halfCos;
}

void
CQuaternion::Get(RwMatrix *matrix)
{
	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;

	float x_2x = x * x2;
	float x_2y = x * y2;
	float x_2z = x * z2;
	float y_2y = y * y2;
	float y_2z = y * z2;
	float z_2z = z * z2;
	float w_2x = w * x2;
	float w_2y = w * y2;
	float w_2z = w * z2;

	matrix->right.x = 1.0f - (y_2y + z_2z);
	matrix->up.x = x_2y - w_2z;
	matrix->at.x = x_2z + w_2y;
	matrix->right.y = x_2y + w_2z;
	matrix->up.y = 1.0f - (x_2x + z_2z);
	matrix->at.y = y_2z - w_2x;
	matrix->right.z = x_2z - w_2y;
	matrix->up.z = y_2z + w_2x;
	matrix->at.z = 1.0f - (x_2x + y_2y);
}

void
CQuaternion::Set(const RwMatrix &matrix)
{
	float f, s, m;

	f = matrix.up.y + matrix.right.x + matrix.at.z;
	if (f >= 0.0f) {
		s = Sqrt(f + 1.0f);
		w = 0.5f * s;
		m = 0.5f / s;
		x = (matrix.up.z - matrix.at.y) * m;
		y = (matrix.at.x - matrix.right.z) * m;
		z = (matrix.right.y - matrix.up.x) * m;
		return;
	}

	f = matrix.right.x - matrix.up.y - matrix.at.z;
	if (f >= 0.0f) {
		s = Sqrt(f + 1.0f);
		x = 0.5f * s;
		m = 0.5f / s;
		y = (matrix.up.x + matrix.right.y) * m;
		z = (matrix.at.x + matrix.right.z) * m;
		w = (matrix.up.z - matrix.at.y) * m;
		return;
	}

	f = matrix.up.y - matrix.right.x - matrix.at.z;
	if (f >= 0.0f) {
		s = Sqrt(f + 1.0f);
		y = 0.5f * s;
		m = 0.5f / s;
		w = (matrix.at.x - matrix.right.z) * m;
		x = (matrix.up.x - matrix.right.y) * m;
		z = (matrix.at.y + matrix.up.z) * m;
		return;
	}

	f = matrix.at.z - (matrix.up.y + matrix.right.x);
	s = Sqrt(f + 1.0f);
	z = 0.5f * s;
	m = 0.5f / s;
	w = (matrix.right.y - matrix.up.x) * m;
	x = (matrix.at.x + matrix.right.z) * m;
	y = (matrix.at.y + matrix.up.z) * m;
}

void
CQuaternion::Get(float *f1, float *f2, float *f3)
{
	RwMatrix matrix;

	Get(&matrix);
	*f3 = Atan2(matrix.right.y, matrix.up.y);
	if (*f3 < 0.0f)
		*f3 += TWOPI;
	float s = Sin(*f3);
	float c = Cos(*f3);
	*f1 = Atan2(-matrix.at.y, s * matrix.right.y + c * matrix.up.y);
	if (*f1 < 0.0f)
		*f1 += TWOPI;
	*f2 = Atan2(-(matrix.right.z * c - matrix.up.z * s), matrix.right.x * c - matrix.up.x * s);
	if (*f2 < 0.0f)
		*f2 += TWOPI;
}

void
CQuaternion::Set(float f1, float f2, float f3)
{
	float c1 = Cos(f1 * 0.5f);
	float c2 = Cos(f2 * 0.5f);
	float c3 = Cos(f3 * 0.5f);
	float s1 = Sin(f1 * 0.5f);
	float s2 = Sin(f2 * 0.5f);
	float s3 = Sin(f3 * 0.5f);
	x = ((c2 * c1) * s3) - ((s2 * s1) * c3);
	y = ((s1 * c2) * c3) + ((s2 * c1) * s3);
	z = ((s2 * c1) * c3) - ((s1 * c2) * s3);
	w = ((c2 * c1) * c3) + ((s2 * s1) * s3);
}
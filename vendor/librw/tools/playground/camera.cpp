#include <cstdio>
#include <cassert>

#include <rw.h>

#define PI 3.14159265359f
#include "camera.h"

using rw::Quat;
using rw::V3d;

void
Camera::update(void)
{
	if(m_rwcam){
		m_rwcam->setNearPlane(m_near);
		m_rwcam->setFarPlane(m_far);
		m_rwcam->setFOV(m_fov, m_aspectRatio);

		rw::Frame *f = m_rwcam->getFrame();
		if(f){
			V3d forward = normalize(sub(m_target, m_position));
			V3d left = normalize(cross(m_up, forward));
			V3d nup = cross(forward, left);
			f->matrix.right = left; // lol
			f->matrix.up = nup;
			f->matrix.at = forward;
			f->matrix.pos = m_position;
			f->matrix.optimize();
			f->updateObjects();
		}
	}
}

void
Camera::setTarget(V3d target)
{
	m_position = sub(m_position, sub(m_target, target));
	m_target = target;
}

float
Camera::getHeading(void)
{
	V3d dir = sub(m_target, m_position);
	float a = atan2(dir.y, dir.x)-PI/2.0f;
	return m_localup.z < 0.0f ? a-PI : a;
}

void
Camera::turn(float yaw, float pitch)
{
	V3d dir = sub(m_target, m_position);
	Quat r = Quat::rotation(yaw, rw::makeV3d(0.0f, 0.0f, 1.0f));
	dir = rotate(dir, r);
	m_localup = rotate(m_localup, r);

	V3d right = normalize(cross(dir, m_localup));
	r = Quat::rotation(pitch, right);
	dir = rotate(dir, r);
	m_localup = normalize(cross(right, dir));
	if(m_localup.z >= 0.0) m_up.z = 1.0;
	else m_up.z = -1.0f;

	m_target = add(m_position, dir);
}

void
Camera::orbit(float yaw, float pitch)
{
	V3d dir = sub(m_target, m_position);
	Quat r = Quat::rotation(yaw, rw::makeV3d(0.0f, 0.0f, 1.0f));
	dir = rotate(dir, r);
	m_localup = rotate(m_localup, r);

	V3d right = normalize(cross(dir, m_localup));
	r = Quat::rotation(-pitch, right);
	dir = rotate(dir, r);
	m_localup = normalize(cross(right, dir));
	if(m_localup.z >= 0.0) m_up.z = 1.0;
	else m_up.z = -1.0f;

	m_position = sub(m_target, dir);
}

void
Camera::dolly(float dist)
{
	V3d dir = setlength(sub(m_target, m_position), dist);
	m_position = add(m_position, dir);
	m_target = add(m_target, dir);
}

void
Camera::zoom(float dist)
{
	V3d dir = sub(m_target, m_position);
	float curdist = length(dir);
	if(dist >= curdist)
		dist = curdist-0.01f;
	dir = setlength(dir, dist);
	m_position = add(m_position, dir);
}

void
Camera::pan(float x, float y)
{
	V3d dir = normalize(sub(m_target, m_position));
	V3d right = normalize(cross(dir, m_up));
	V3d localup = normalize(cross(right, dir));
	dir = add(scale(right, x), scale(localup, y));
	m_position = add(m_position, dir);
	m_target = add(m_target, dir);
}

float
Camera::distanceTo(V3d v)
{
	return length(sub(m_position, v));
}

Camera::Camera()
{
	m_position.set(0.0f, 6.0f, 0.0f);
	m_target.set(0.0f, 0.0f, 0.0f);
	m_up.set(0.0f, 0.0f, 1.0f);
	m_localup = m_up;
	m_fov = 70.0f;
	m_aspectRatio = 1.0f;
	m_near = 0.1f;
	m_far = 100.0f;
	m_rwcam = NULL;
}


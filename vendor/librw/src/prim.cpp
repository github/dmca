#include <stdio.h>
#include <stdlib.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

#define PLUGIN_ID 0

namespace rw {


void
BBox::initialize(V3d *point)
{
	this->inf = *point;
	this->sup = *point;
}

void
BBox::addPoint(V3d *point)
{
	if(point->x < this->inf.x)
		this->inf.x = point->x;
	if(point->y < this->inf.y)
		this->inf.y = point->y;
	if(point->z < this->inf.z)
		this->inf.z = point->z;
	if(point->x > this->sup.x)
		this->sup.x = point->x;
	if(point->y > this->sup.y)
		this->sup.y = point->y;
	if(point->z > this->sup.z)
		this->sup.z = point->z;
}

void
BBox::calculate(V3d *points, int32 n)
{
	this->inf = points[0];
	this->sup = points[0];
	while(--n){
		points++;
		if(points->x < this->inf.x)
			this->inf.x = points->x;
		if(points->y < this->inf.y)
			this->inf.y = points->y;
		if(points->z < this->inf.z)
			this->inf.z = points->z;
		if(points->x > this->sup.x)
			this->sup.x = points->x;
		if(points->y > this->sup.y)
			this->sup.y = points->y;
		if(points->z > this->sup.z)
			this->sup.z = points->z;
	}
}

bool
BBox::containsPoint(V3d *point)
{
	return point->x >= this->inf.x && point->x <= this->sup.x &&
		point->y >= this->inf.y && point->y <= this->sup.y &&
		point->z >= this->inf.z && point->z <= this->sup.z;
}

}

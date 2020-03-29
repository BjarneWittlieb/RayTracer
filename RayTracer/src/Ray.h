#pragma once

#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

class Ray
{
public:
	Vector3 Origin;
	Vector3 Direction;
public:
	Ray(Vector3 origin, Vector3 dir)
		:Direction(dir), Origin(origin) {}

	Vector3 inline At(double t) const { return Origin + t * Direction; }
};

#endif


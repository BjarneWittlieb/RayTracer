#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"

class Material;

struct HitRecord {
	Vector3 EntryPoint;
	Vector3 Normal;
	shared_ptr<Material> MatPtr;
	double t;
	bool FrontFace;

	// Function that sets the Normal to the opposit direction of the ray.
	inline void setFaceNormal(const Ray & r, const Vector3& OutwardNormal)
	{
		FrontFace = r.Direction * OutwardNormal < 0;
		Normal = FrontFace ? OutwardNormal : -OutwardNormal;
	}
};

class Hittable {
public:
	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

#endif
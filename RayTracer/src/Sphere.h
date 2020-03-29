#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Vector3.h"

class Sphere :
	public Hittable
{
public:
	double Radius;
	Vector3 Center;
	shared_ptr<Material> MatPtr;

public:
	Sphere() {}
	Sphere(Vector3 cen, double r, shared_ptr<Material> m)
		: Center(cen), Radius(r), MatPtr(m) {}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const;
};

bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	Vector3 oc = r.Origin - Center;
	double a = r.Direction * r.Direction;
	double halfB = (oc * r.Direction);
	double c = (oc * oc) - Radius * Radius;
	double discriminant = halfB * halfB - a*c;

	if (discriminant > 0)
	{
		double root = sqrt(discriminant);
		double temp = (-halfB - root) / a;
		if (!(temp < tMax && temp > tMin))
			temp = (-halfB + root) / a;
		if (temp < tMax && temp > tMin)
		{
			rec.t = temp;
			rec.EntryPoint = r.At(temp);
			rec.Normal = (rec.EntryPoint - Center) / Radius;
			rec.setFaceNormal(r, rec.Normal);
			rec.MatPtr = MatPtr;
			return true;
		}
	}
	return false;
}

#endif
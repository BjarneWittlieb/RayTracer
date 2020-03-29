#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "MainDef.h"

class Camera {
public:
	Vector3 Origin;
	Vector3 LowerLeftCorner;
	Vector3 Horizontal;
	Vector3 Vertical;
	Vector3 u, v, w;
	double LenseRadius;

public:
	Camera(
		Vector3 lookfrom, Vector3 lookat, Vector3 vup,
		double vfov, // top to bottom in degrees
		double aspect, double aperture, double focusDist
	) {
		Origin = lookfrom;
		LenseRadius = aperture / 2;

		double theta = DegreesToRadians(vfov);
		double halfHeight = tan(theta / 2);
		double halfWidth = aspect * halfHeight;
		w = (lookfrom - lookat).Normalized();
		// vector along width
		u = vup.Cross(w).Normalized();
		// vector along height
		v = w.Cross(u);

		LowerLeftCorner = Origin 
			- halfWidth * focusDist * u
			- halfHeight * focusDist * v
			- focusDist * w;

		Horizontal = 2 * halfWidth * focusDist * u;
		Vertical = 2 * halfHeight * focusDist* v;
	}

	Ray GetRay(double s, double t) {
		Vector3 rd = LenseRadius * RandomInUnitDisk();
		Vector3 offset = u * rd.x + v * rd.y;
		return Ray(
			Origin + offset,
			LowerLeftCorner + s * Horizontal + t * Vertical - Origin - offset);
	}
};

#endif
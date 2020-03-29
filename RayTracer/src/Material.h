#pragma once

#include "Ray.h"
#include "Hittable.h"

class Material
{
public:
	virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const = 0;
};

// Class for scattering a ray on non reflective objects using the lambertian diffuse method.
class Lambertian : public Material
{
public:
	Vector3 Albedo;
	std::mutex* mtx;

public:
	Lambertian(const Vector3& a)
		: Albedo(a)
	{
		mtx = new std::mutex();
	}

	virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const
	{
		std::lock_guard<std::mutex> lock(*mtx);
		Vector3 scattDir = rec.Normal + RandomUnitVector();
		scattered = Ray(rec.EntryPoint, scattDir);
		attenuation = Albedo;
		return true;
	}
};

// Metal that is reflecting rays
class Metal : public Material
{
public:
	Vector3 Albedo;
	std::mutex* mtx;
	double Fuzz;

public:
	Metal(const Vector3& a, double f = 0)
		: Albedo(a), Fuzz(f)
	{
		mtx = new std::mutex();
	}

	virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const
	{
		std::lock_guard<std::mutex> lock(*mtx);
		// Reflecting at the right angle
		Vector3 reflected = Reflect(rIn.Direction.Normalized(), rec.Normal);
		scattered = Ray(rec.EntryPoint, reflected + Fuzz * RandomInUnitSphere());
		attenuation = Albedo;
		return scattered.Direction * rec.Normal > 0.0;
	}
};

// Polinomial approximation of reflection probability when hit glass
double Schlick(double cosine, double refIdx)
{
	double r0 = (1 - refIdx) / (1 + refIdx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1-cosine), 5);
}

// Class for object that refract like glass or water
// in combination with spheres a negative radius can be used to make it hole.
class Dielectric : public Material
{
public:
	double RefIdx;
	std::mutex* mtx;

public:
	Dielectric(double ri) : RefIdx(ri) {
		mtx = new std::mutex();
	}

	virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const
	{
		std::lock_guard<std::mutex> lock(*mtx);
		attenuation = Vector3(1, 1, 1);
		double etai_over_etat;
		if (rec.FrontFace)
		{
			etai_over_etat = 1.0 / RefIdx;
		}
		else
		{
			etai_over_etat = RefIdx;
		}

		Vector3 unitDir = rIn.Direction.Normalized();

		// Reflect when array is within the dielectric
		double cos_theta = ffmin( (-unitDir) * rec.Normal, 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
		if (etai_over_etat * sin_theta > 1.0) {
			Vector3 reflected = Reflect(unitDir, rec.Normal);
			scattered = Ray(rec.EntryPoint, reflected);
			return true;
		}

		// Reflect with certain probability when ray hits dielectric depending on incomming angle
		double reflect_prob = Schlick(cos_theta, etai_over_etat);
		if (ffrnd() < reflect_prob)
		{
			Vector3 reflected = Reflect(unitDir, rec.Normal);
			scattered = Ray(rec.EntryPoint, reflected);
			return true;
		}

		// Otherwise refract
		Vector3 refracted = Refract(unitDir, rec.Normal, etai_over_etat);
		scattered = Ray(rec.EntryPoint, refracted);
		return true;
	}
};
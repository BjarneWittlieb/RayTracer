#pragma once
#include <math.h>
#include <fstream>

class Vector3
{
public:
	double x, y, z;
public:
	Vector3(double x, double y, double z)
		:x(x), y(y), z(z) {}
	Vector3()
		:x(0), y(0), z(0) {}

	Vector3 Normalized() const
	{
		double t = 1 / Length();
		return Vector3(t * x, t * y, t * z);
	}
	inline double LengthSquared() const { return x * x + y * y + z * z; }
	inline double Length() const { return sqrt(LengthSquared()); }
	// inline double Distance(Vector3& v) { return (*this - v).Length(); }
	// inline double Angle(Vector3& v) { return acos((*this * v) / (Length() * v.Length())); }
	Vector3 Cross(Vector3& v)
	{
		double x1 = y * v.z - z * v.y;
		double y1 = z * v.x - x * v.z;
		double z1 = x * v.y - y * v.x;
		return Vector3(x1, y1, z1);
	}

	void WriteColor(std::ofstream &out, int samplesPerPixel, double gammavalue)
	{
		// Dividing color by total amount of samples
		double scale = 1.0 / samplesPerPixel;
		double power = 1.0 / gammavalue;
		double r = pow(scale * x, power);
		double g = pow(scale * y, power);
		double b = pow(scale * z, power);

		// Write the translated [0, 255] value of each color component
		out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
			<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
			<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
	}

	inline Vector3 operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector3 operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector3 operator*= (const double t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	inline Vector3 operator/= (const double t)
	{
		return *this *= (1/t);
	}

	inline static Vector3 Random()
	{
		return Vector3(ffrnd(), ffrnd(), ffrnd());
	}

	inline static Vector3 Random(double min, double max)
	{
		return Vector3(ffrnd(min, max), ffrnd(min, max), ffrnd(min, max));
	}
};

// Some convinient operations

inline std::ostream& operator<<(std::ostream &out, const Vector3 &v)
{
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
inline Vector3 operator-(const Vector3& v)
{
	return Vector3(-v.x, -v.y, -v.z);
}
inline Vector3 operator*(double t, const Vector3& v)
{
	return Vector3(t * v.x, t * v.y, t * v.z);
}
inline Vector3 operator*(const Vector3& v, double t)
{
	return t * v;
}
inline Vector3 operator/(const Vector3& v, double t)
{
	return (1 / t) * v;
}

double operator*(const Vector3& v1, const Vector3& v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}
Vector3 mult(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}


Vector3 RandomUnitVector()
{
	double a = ffrnd(0, 2 * PI);
	double z = ffrnd(-1, 1);
	double r = sqrt(1 - z * z);
	return Vector3(r*cos(a), r*sin(a), z);
}

// Selects a random vector within the unit sphere
Vector3 RandomInUnitSphere()
{
	while (true) {
		auto p = Vector3::Random(-1, 1);
		if (p.LengthSquared() >= 1) continue;
		return p;
	}
}

Vector3 RandomInHemisphere(const Vector3& normal)
{
	Vector3 u = RandomInUnitSphere();
	if (normal * u > 0.0)
		return u;
	else
		return -u;
}

Vector3 RandomInUnitDisk()
{
	while (true) {
		Vector3 p = Vector3(ffrnd(-1, 1), ffrnd(-1, 1), 0);
		if (p.LengthSquared() >= 1) continue;
		return p;
	}
}

// Returns the reflected vector of v along the normal n
Vector3 Reflect(const Vector3& v, const Vector3& n)
{
	return v - (2 * (v * n)) * n;
}

Vector3 Refract(const Vector3& uv, const Vector3& n, double etai_over_etat)
{
	double cos_theta = (-uv) * n;
	Vector3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	Vector3 r_out_perp = -sqrt(1.0 - r_out_parallel.LengthSquared()) * n;
	return r_out_parallel + r_out_perp;
}


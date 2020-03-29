#ifndef MAINDEF
#define MAINDEF

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

using std::shared_ptr;
using std::make_shared;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

inline double DegreesToRadians(double degrees) {
	return degrees * PI / 180;
}

inline double ffmin(double a, double b) { return a <= b ? a : b; }
inline double ffmax(double a, double b) { return a >= b ? a : b; }

// returnes a random double in [0, 1)
inline double ffrnd()
{
	return rand() / (RAND_MAX + 1.0);
}

// returnes a random double in [min, max)
inline double ffrnd(double min, double max)
{
	return min + (max - min) * ffrnd();
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// Common Headers

#include "Ray.h"
#include "Vector3.h"

#endif

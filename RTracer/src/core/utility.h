#pragma once

#include <limits>
#include <random>

using uint = unsigned int;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
const double epsilon = 0.0001;
const double inv_180 = 1 / 180.0;

// Utility Functions
inline double degrees_to_radians(double degrees)
{
	return degrees * pi * inv_180;
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline double random_double(double min = 0.0, double max = 1.0)
{
	static std::uniform_real_distribution<double> distribution(min, max);
	static std::mt19937 generator;
	return distribution(generator);
}
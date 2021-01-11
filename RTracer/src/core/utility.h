#pragma once

#include <limits>
#include <random>

using uint = unsigned int;

/*
 * Constants 
 */

// double infinity
const double infinity = std::numeric_limits<double>::infinity();
// double pi
const double pi = 3.1415926535897932385;
// double smallest value
const double epsilon = 0.0001;
// double 1/180
const double inv_180 = 1 / 180.0;

/*
 * Utility functions
 */

// convert given degrees to radians
inline double degrees_to_radians(double degrees)
{
	return degrees * pi * inv_180;
}

// clamp given double value from min to max
inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// return a random double from min to max
inline double random_double(double min = 0.0, double max = 1.0)
{
	static std::uniform_real_distribution<double> distribution(min, max);
	static std::mt19937 generator;
	return distribution(generator);
}
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

namespace random
{
	// return a random double from min to max
	template <typename T>
	inline T get(T min = 0, T max = 1)
	{
		static std::mt19937 generator;
		if constexpr (std::_Is_any_of_v<T, float, double, long double>)
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(generator);	
		}
		else
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);	
		}
	}
}

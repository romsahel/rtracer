#pragma once

#include <limits>

using uint = unsigned int;

/*
 * Constants 
 */

namespace constants
{
	// float infinity
	inline constexpr float infinity = std::numeric_limits<float>::infinity();

	// float pi
	inline constexpr float pi = 3.1415926535897932385f;
	inline constexpr float inv_pi = 1.0f / pi;

	// float smallest value
	inline constexpr float epsilon = 0.0001f;

	// float 1/180
	inline constexpr float inv_180 = 1.0f / 180.0f;
}

/*
 * Utility functions
 */

// convert given degrees to radians
inline float degrees_to_radians(float degrees)
{
	return degrees * constants::pi * constants::inv_180;
}

// clamp given float value from min to max
__forceinline float clamp(float x, float min, float max)
{
	return x < min ? min : x > max ? max : x;
}

template <typename T>
__forceinline int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

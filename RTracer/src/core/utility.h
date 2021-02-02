#pragma once

#include <limits>
#include <random>

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
	inline constexpr float inv_180 = 1 / 180.0f;
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
inline float clamp(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace random
{
	// return a random float from min to max
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

	template <int N, typename T>
	struct static_random_generator
	{
		static_random_generator() : arr()
		{
			for (auto i = 0; i != N; ++i)
				arr[i] = random::get<T>();
		}

		T get()
		{
			if (index >= N)
				index = 0;
			return arr[index++];
		}

		T arr[N];
		int index = 0;
	};

	inline static_random_generator<4096, float> static_double;

}

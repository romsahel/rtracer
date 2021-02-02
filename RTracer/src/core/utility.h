#pragma once

#include <limits>
#include <random>

using uint = unsigned int;

/*
 * Constants 
 */

namespace constants
{
	// double infinity
	inline constexpr double infinity = std::numeric_limits<double>::infinity();

	// double pi
	inline constexpr double pi = 3.1415926535897932385;
	inline constexpr double inv_pi = 1.0 / pi;

	// double smallest value
	inline constexpr double epsilon = 0.0001;

	// double 1/180
	inline constexpr double inv_180 = 1 / 180.0;
}

/*
 * Utility functions
 */

// convert given degrees to radians
inline double degrees_to_radians(double degrees)
{
	return degrees * constants::pi * constants::inv_180;
}

// clamp given double value from min to max
inline double clamp(double x, double min, double max)
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

	inline static_random_generator<4096, double> static_double;

}

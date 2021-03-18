#pragma once

#include <random>

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

	inline static_random_generator<8192, float> static_float;

}

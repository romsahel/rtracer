#pragma once

#include <cmath>
#include <emmintrin.h>

#include "utility.h"

// STRUCT TEMPLATE enable_if
template <bool _Test, class _Ty = void>
struct check_access
{
}; // no member "type" when !_Test

template <class _Ty>
struct check_access<true, _Ty>
{
	// value is _Ty for _Test
	using value = _Ty;
};

#define ALLOW_FOR(N) template<typename check_access<(Size == N), bool>::value = 0>

template <int Size>
struct vec
{
	union data_t
	{
		__m128 sse;
		float array[Size];

		explicit data_t(const __m128& sse) : sse{sse} {}

		explicit data_t(float x, float y, float z, float w)
			: sse(_mm_set_ps(w, z, y, x))
		{
		}
	};

	data_t data;

	vec(float x, float y, float z = 0.0f, float w = 0.0f)
		: data{x, y, z, w}
	{
	}

	template<bool B>
	using EnableIfB = typename std::enable_if<B, int>::type;
	
	template<size_t D1 = Size, EnableIfB<D1 == 2> = 0>
	explicit vec(float xyzw) : vec(xyzw, xyzw)
	{
	}
	
	template<size_t D1 = Size, EnableIfB<D1 == 3> = 0>
	explicit vec(float xyzw) : vec(xyzw, xyzw, xyzw)
	{
	}
	
	template<size_t D1 = Size, EnableIfB<D1 == 4> = 0>
	explicit vec(float xyzw) : vec(xyzw, xyzw, xyzw, xyzw)
	{
	}

	vec() : vec(0.0f)
	{
	}

	explicit vec(const __m128& m_sse)
		: data(m_sse)
	{
	}

	float& x() { return data.array[0]; }
	float x() const { return data.array[0]; }
	
	float& y() { return data.array[1]; } 
	float y() const { return data.array[1]; }
	
	float& z() { static_assert(Size > 2); return data.array[2]; } 
	float z() const { static_assert(Size > 2); return data.array[2]; }
	
	float& w() { static_assert(Size > 3); return data.array[3]; } 
	float w() const { static_assert(Size > 3); return data.array[3]; }

	float operator[](size_t index) const
	{
		return data.array[index];
	}

	float& operator[](size_t index)
	{
		return data.array[index];
	}

	vec<Size> operator-() const
	{
		return vec<Size>(_mm_sub_ps(_mm_setzero_ps(), data.sse));
	}

	vec<Size>& operator+=(const vec<Size>& v)
	{
		data.sse = _mm_add_ps(data.sse, v.data.sse);
		return *this;
	}

	vec<Size>& operator-=(const vec<Size>& v)
	{
		return this->operator+=(-v);
	}

	vec<Size>& operator*=(float v)
	{
		const __m128 scalar = _mm_set1_ps(v);
		data.sse = _mm_mul_ps(data.sse, scalar);
		return *this;
	}

	vec<Size>& operator/=(float v)
	{
		const __m128 scalar = _mm_set1_ps(v);
		data.sse = _mm_div_ps(data.sse, scalar);
		return *this;
	}

	float length_squared() const
	{
		auto squared = data_t(_mm_mul_ps(data.sse, data.sse));
		if constexpr (Size == 2) return squared.array[0] + squared.array[1];
		else if constexpr (Size == 3) return squared.array[0] + squared.array[1] + squared.array[2];
		else return squared.array[0] + squared.array[1] + squared.array[2] + squared.array[3];
	}

	float length() const
	{
		return std::sqrt(length_squared());
	}

	vec<Size>& normalize()
	{
		operator/=(length());
		return *this;
	}

	bool is_near_zero() const
	{
		constexpr static auto e = 1e-8f;
		if constexpr (Size == 2) return fabsf(data.array[0]) < e && fabsf(data.array[1]) < e;
		if constexpr (Size == 3) return fabsf(data.array[0]) < e && fabsf(data.array[1]) < e && fabsf(data.array[2]) < e;
		if constexpr (Size == 4) return fabsf(data.array[0]) < e && fabsf(data.array[1]) < e && fabsf(data.array[2]) < e && fabsf(data.array[3]) < e;
	}

	// return a random vec3
	static vec<Size> random(float min = 0.0, float max = 1.0f)
	{
		return vec<Size>(random::get<float>(min, max), random::get<float>(min, max), random::get<float>(min, max),
		                 random::get<float>(min, max));
	}
};

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

namespace vector3
{
	// return a random vec3 contained in a sphere placed at the origin and of a radius of 1
	vec3 random_in_unit_sphere(float sphere_angle = 2 * constants::pi);
	// return a random vec3 contained in a disk placed at the origin and of a radius of 1
	vec3 random_in_unit_disk();
	// return a random vec3 contained in a hemisphere placed at the origin and of a radius of 1
	vec3 random_in_hemisphere(const vec3& normal);
	
	inline vec3 zero() { return vec3(0.0f, 0.0f, 0.0f); }
	inline vec3 up() { return vec3(0.0f, 1.0f, 0.0f); }
	inline vec3 down() { return vec3(0.0f, -1.0f, 0.0f); }
	inline vec3 right() { return vec3(1.0f, 0.0f, 0.0f); }
	inline vec3 left() { return vec3(-1.0f, 0.0f, 0.0f); }
	inline vec3 forward() { return vec3(0.0f, 0.0f, 1.0f); }
	inline vec3 backward() { return vec3(0.0f, 0.0f, -1.0f); }
};

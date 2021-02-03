#pragma once

#include <cmath>
#include <emmintrin.h>

#include "utility.h"

struct vec3
{
	__m128 m_sse;

	vec3(float x, float y, float z)
		: m_sse(_mm_set_ps(0.0f, z, y, x))
	{
	}

	vec3(float xyz) : vec3(xyz, xyz, xyz)
	{
	}

	vec3() : vec3(0.0)
	{
	}

	float& x() { return m_sse.m128_f32[0]; }
	float& y() { return m_sse.m128_f32[1]; }
	float& z() { return m_sse.m128_f32[2]; }
	
	const float& x() const { return m_sse.m128_f32[0]; }
	const float& y() const { return m_sse.m128_f32[1]; }
	const float& z() const { return m_sse.m128_f32[2]; }

	float operator[](size_t index) const
	{
		return m_sse.m128_f32[index];
	}

	float& operator[](size_t index)
	{
		return m_sse.m128_f32[index];
	}

	vec3 operator-() const
	{
		return vec3(-m_sse.m128_f32[0], -m_sse.m128_f32[1], -m_sse.m128_f32[2]);
	}

	vec3& operator+=(const vec3& v)
	{
		m_sse = _mm_add_ps(m_sse, v.m_sse);
		return *this;
	}

	vec3& operator-=(const vec3& v)
	{
		return this->operator+=(-v);
	}

	vec3& operator*=(float v)
	{
		const __m128 scalar = _mm_set1_ps(v);
		m_sse = _mm_mul_ps(m_sse, scalar);
		return *this;
	}

	vec3& operator/=(float v)
	{
		const __m128 scalar = _mm_set1_ps(v);
		m_sse = _mm_div_ps(m_sse, scalar);
		return *this;
	}

	float length_squared() const
	{
		return x() * x() + y() * y() + z() * z();
	}

	float length() const
	{
		return std::sqrt(length_squared());
	}

	vec3& normalize()
	{
		operator/=(length());
		return *this;
	}

	bool is_near_zero() const
	{
		const static auto e = 1e-8;
		return fabsf(m_sse.m128_f32[0]) < e && fabsf(m_sse.m128_f32[1]) < e && fabsf(m_sse.m128_f32[2]) < e;
	}

	// return a random vec3
	static vec3 random(float min = 0.0, float max = 1.0f)
	{
		return vec3(random::get<float>(min, max), random::get<float>(min, max), random::get<float>(min, max));
	}

	// return a random vec3 contained in a sphere placed at the origin and of a radius of 1
	static vec3 random_in_unit_sphere(float sphere_angle = 2 * constants::pi);
	// return a random vec3 contained in a disk placed at the origin and of a radius of 1
	static vec3 random_in_unit_disk();
	// return a random vec3 contained in a hemisphere placed at the origin and of a radius of 1
	static vec3 random_in_hemisphere(const vec3& normal);

	static vec3 zero() { return vec3(0.0, 0.0, 0.0); }
	static vec3 up() { return vec3(0.0, 1.0f, 0.0); }
	static vec3 down() { return vec3(0.0, -1.0f, 0.0); }
	static vec3 right() { return vec3(1.0f, 0.0, 0.0); }
	static vec3 left() { return vec3(-1.0f, 0.0, 0.0); }
	static vec3 forward() { return vec3(0.0, 0.0, 1.0f); }
	static vec3 backward() { return vec3(0.0, 0.0, -1.0f); }
};

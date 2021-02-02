﻿#pragma once

#include "vec3.h"

inline vec3 operator+(vec3 v1, const vec3& v2)
{
	return v1 += v2;
}

inline vec3 operator-(vec3 v1, const vec3& v2)
{
	return v1 += -v2;
}

inline vec3 operator*(vec3 v1, float v)
{
	return v1 *= v;
}

inline vec3 operator/(vec3 v1, float v)
{
	return v1 /= v;
}

inline vec3 operator*(float v, vec3 v1)
{
	return v1 *= v;
}

inline vec3 operator/(float v, vec3 v1)
{
	return v1 /= v;
}

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline float dot(const vec3& u, const vec3& v)
{
	return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(
		u.y() * v.z() - u.z() * v.y(),
		u.z() * v.x() - u.x() * v.z(),
		u.x() * v.y() - u.y() * v.x()
	);
}

inline vec3 mul(const vec3& u, const vec3& v)
{
	return vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline vec3 normalize(vec3 u)
{
	return u.normalize();
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, float refraction_ratio)
{
	const auto cos_theta = fmin(dot(-uv, n), 1.0f);
	const vec3 r_out_perp = refraction_ratio * (uv + cos_theta * n);
	const vec3 r_out_parallel = -sqrtf(fabs(1.0f - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

inline float sum(const vec3& v)
{
	return v.x() + v.y() + v.z();
}

inline vec3 abs(const vec3& v)
{
	return vec3{std::abs(v.x()), std::abs(v.y()), std::abs(v.z())};
}
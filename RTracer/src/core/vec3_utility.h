#pragma once

#include "vec3.h"
#include <iostream>

#include "point3.h"
#include "direction3.h"

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline float sum(const vec3& v)
{
	return v.x + v.y + v.z;
}

inline float sum(const vec4& v)
{
	return v.x + v.y + v.z + v.w;
}

inline vec3 mul(const vec3& u, const vec3& v)
{
	return u * v;
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, float refraction_ratio)
{
	const auto cos_theta = fmin(dot(-uv, n), 1.0f);
	const vec3 r_out_perp = refraction_ratio * (uv + cos_theta * n);
	const vec3 r_out_parallel = -sqrtf(fabs(1.0f - length2(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

inline vec3 abs(const vec3& v)
{
	return vec3{std::abs(v.x), std::abs(v.y), std::abs(v.z)};
}

inline bool is_near_zero(const vec3& v)
{
	return length2(v) < constants::epsilon * constants::epsilon;
}
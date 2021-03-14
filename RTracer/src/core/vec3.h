#pragma once

#include <iostream>

#include <cmath>
#include <emmintrin.h>

#include "utility.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/scalar_relational.hpp>
#include <glm/ext/vector_relational.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using direction3 = vec3;
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

template <typename vec_t>
inline float sum(const vec_t& v)
{
	return glm::compAdd(v);
}

template <typename vec_t>
inline bool is_near_zero(const vec_t& v)
{
	return length2(v) < constants::epsilon * constants::epsilon;
}

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

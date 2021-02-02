#pragma once

#include "vec3.h"

struct point3 : vec3
{
	point3(float x, float y, float z)
		: vec3(x, y, z)
	{
	}

	explicit point3(float xyz)
		: vec3(xyz)
	{
	}

	point3() = default;

	explicit point3(const vec3& v)
		: vec3(v)
	{
	}
};

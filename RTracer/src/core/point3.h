#pragma once

#include "vec3.h"

struct point3 : vec3
{
	point3(double x, double y, double z)
		: vec3(x, y, z)
	{
	}

	explicit point3(double xyz)
		: vec3(xyz)
	{
	}

	point3() = default;

	explicit point3(const vec3& v)
		: vec3(v)
	{
	}
};

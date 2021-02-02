﻿#pragma once

#include "vec3.h"

struct direction3 : vec3
{
	direction3(float x, float y, float z)
		: vec3(x, y, z)
	{
	}

	explicit direction3(float xyz)
		: vec3(xyz)
	{
	}

	direction3() = default;

	explicit direction3(const vec3& v)
		: vec3(v)
	{
	}
};

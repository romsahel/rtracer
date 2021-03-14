#pragma once

#include "vec3.h"

struct color : vec3
{
	explicit color(const vec3& v) : vec3(v)
	{
	}

	color(float in_x, float in_y, float in_z) : vec3(in_x, in_y, in_z)
	{
	}

	explicit color(float xyz) : vec3(xyz)
	{
	}

	color() = default;

	static color black() { return color(0, 0, 0); }
	static color white() { return color(1, 1, 1); }
	static color red() { return color(1, 0, 0); }
	static color blue() { return color(0, 0, 1); }
	static color green() { return color(0, 1, 0); }
	static color magenta() { return color(1, 0, 1); }
	static color gray() { return color(0.5f, 0.5f, 0.5f); }
};

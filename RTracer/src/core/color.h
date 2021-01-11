#pragma once

#include <ostream>

#include "utility.h"
#include "vec3.h"

struct color : vec3
{
	explicit color(const vec3& v) : vec3(v)
	{
	}

	color(double x, double y, double z) : vec3(x, y, z)
	{
	}

	explicit color(double xyz) : vec3(xyz)
	{
	}

	color() = default;

	static color black() { return color(0, 0, 0); }
	static color white() { return color(1, 1, 1); }
	static color red() { return color(1, 0, 0); }
	static color blue() { return color(0, 0, 1); }
	static color green() { return color(0, 1, 0); }
	static color magenta() { return color(1, 0, 1); }
	static color gray() { return color(0.5, 0.5, 0.5); }
};

inline unsigned char to_writable_color(double rgb, double inv_samples_per_pixel)
{
	return static_cast<unsigned char>((clamp(255.0 * sqrt(inv_samples_per_pixel * rgb), 0.0, 255.0)));
}

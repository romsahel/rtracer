#pragma once

#include <ostream>

#include "utility.h"
#include "vec3.h"

struct color : vec3
{
	explicit color(const vec3& v) : vec3(v)
	{
	}

	color(float x, float y, float z) : vec3(x, y, z)
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

inline vec3 to_writable_color(vec3 rgb, float inv_samples_per_pixel)
{
	return sqrt((inv_samples_per_pixel * rgb)) * 255.0f;
}

__forceinline unsigned char to_writable_color(float rgb)
{
	return static_cast<unsigned char>(clamp(rgb, 0.0f, 255.0f));
}

inline unsigned char to_writable_color(float rgb, float inv_samples_per_pixel)
{
	return static_cast<unsigned char>((clamp(255.0f * sqrt(inv_samples_per_pixel * rgb), 0.0f, 255.0f)));
}

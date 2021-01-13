#pragma once
#include "object_store.h"
#include "core/color.h"
#include "core/point3.h"

class texture
{
public:
	virtual color value_at(vec3 uv_coordinates, const point3& p) const = 0;
};

inline object_store<texture>& texture_store()
{
	static auto instance = object_store<texture>();
	return instance;
}

class solid_color : public texture
{
public:
	color color_value;

	explicit solid_color(const color& value)
		: color_value(value)
	{
	}

	color value_at(vec3 uv_coordinates, const point3& p) const override
	{
		return color_value;
	}
};

class checker_texture : public texture
{
public:
	checker_texture(texture* odd_texture, texture* even_texture)
		: odd(odd_texture),
		  even(even_texture)
	{
	}

	checker_texture(color odd_color, color even_color)
		: odd(&texture_store().add<solid_color>(odd_color)),
		  even(&texture_store().add<solid_color>(even_color))
	{
	}

	color value_at(vec3 uv_coordinates, const point3& p) const override
	{
		const auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value_at(uv_coordinates, p);
		else
			return even->value_at(uv_coordinates, p);
	}

	texture* odd;
	texture* even;
};

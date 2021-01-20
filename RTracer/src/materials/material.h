#pragma once
#include "object_store.h"
#include "texture.h"
#include "core/color.h"
#include "core/vec3_utility.h"

struct hit_info;
class ray;

class material
{
public:
	explicit material(const char* name)
		: name(name)
	{
	}

	virtual ~material() = default;

	virtual bool scatter(const ray& raycast, const hit_info& rec, color& attenuation, ray& scattered) const = 0;

	virtual color emitted(const vec3& coordinates, const point3& point)
	{
		return color(emission_strength * emission->value_at(coordinates, point));
	}

	const char* name;
	texture* emission = solid_color::black();
	double emission_strength = 0.0;
};


inline object_store<material>& material_store()
{
	static auto instance = object_store<material>();
	return instance;
}

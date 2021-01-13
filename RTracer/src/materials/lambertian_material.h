#pragma once

#include "material.h"
#include "texture.h"

#include "core/color.h"
#include "core/hittable.h"

/// <summary>
/// lambertian material are solid/mat-looking materials. They are the default 'diffuse' material
/// Light bounces in random direction (from the surface normal) when hitting the surface
/// </summary>
class lambertian_material : public material
{
public:
	lambertian_material(const char* name, const color& a)
		: material(name), albedo(&texture_store().add<solid_color>(a))
	{
	}

	lambertian_material(const char* name, texture& a)
		: material(name), albedo(&a)
	{
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		direction3 scatter_direction = direction3(hit.normal + vec3::random_in_hemisphere(hit.normal));
		if (scatter_direction.is_near_zero())
			scatter_direction = hit.normal;

		scattered = ray(hit.point, scatter_direction);
		attenuation = albedo->value_at(hit.uv_coordinates, hit.point);
		return true;
	}

	texture* albedo;

	static lambertian_material& default_material()
	{
		static lambertian_material value{"default lambertian", color::gray()};
		return value;
	}
};

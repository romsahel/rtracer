#pragma once

#include "material.h"

#include "core/color.h"
#include "core/hittable.h"

class lambertian_material : public material
{
public:
	lambertian_material(const color& a) : albedo(a)
	{
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		direction3 scatter_direction = direction3(hit.normal + vec3::random_in_hemisphere(hit.normal));
		if (scatter_direction.is_near_zero())
			scatter_direction = hit.normal;

		scattered = ray(hit.point, scatter_direction);
		attenuation = albedo;
		return true;
	}

	color albedo;

	static lambertian_material& default_material()
	{
		static lambertian_material value{color::gray()};
		return value;
	}
};

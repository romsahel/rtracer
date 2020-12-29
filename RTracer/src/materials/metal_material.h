#pragma once

#include "material.h"

#include "core/color.h"
#include "core/hittable.h"

class metal_material : public material
{
public:
	metal_material(const color& a, double r) : albedo(a), roughness(r)
	{
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		const auto reflected = direction3(reflect(raycast.direction(), hit.normal));
		scattered = ray(hit.point, direction3(reflected + roughness * vec3::random_in_unit_sphere()));
		attenuation = albedo;
		return dot(reflected, hit.normal) > 0;
	}

	color albedo;
	double roughness;

	static metal_material& default_material()
	{
		static metal_material value{color::gray(), 0.0};
		return value;
	}
};

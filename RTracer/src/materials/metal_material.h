#pragma once

#include "material.h"

#include "core/color.h"
#include "geometry/abstract/hittable.h"

/// <summary>
/// metal material are metal-looking materials.
/// Light is reflected when hitting the surface.
/// Depending on the roughness of the metal, the bounce can be more or less random to produce blurry reflection
/// </summary>
class metal_material : public material
{
public:
	metal_material(const char* name, const color& a, float r)
		: material(name), albedo(a), roughness(r)
	{
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		const auto reflected = direction3(reflect(raycast.direction, hit.normal));
		scattered = ray(hit.point, direction3(reflected + roughness * vector3::random_in_unit_sphere()));
		attenuation = albedo;
		return dot(reflected, hit.normal) > 0.0f;
	}

	color albedo;
	float roughness;

	static metal_material& default_material()
	{
		static metal_material value{"Default Metal", color::gray(), 0.0f};
		return value;
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		return std::make_shared<serializable_node_base>(
			name, serializable_list{
				std::make_shared<serializable_node<color>>("Albedo", &albedo),
				std::make_shared<serializable_node<float>>("Roughness", &roughness),
				material::serialize()
			}
		);
	}
};

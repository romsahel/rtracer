#pragma once

#include "material.h"

#include "core/color.h"
#include "geometry/abstract/hittable.h"

/// <summary>
/// dieletric material are glass-like materials
/// Light can be both refracted and reflected when hitting the surface, but in precise and predictable ways
/// </summary>
class dielectric_material : public material
{
public:
	explicit dielectric_material(const char* name, float index_of_refraction)
		: material(name)
	{
		this->index_of_refraction(index_of_refraction);
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		const float refraction_ratio = hit.front_face ? m_inv_index_of_refraction : m_index_of_refraction;
		const float cos_theta = fmin(dot(-raycast.direction, hit.normal), 1.0f);
		const float sin_theta = sqrt(1.0f - cos_theta * cos_theta);
		if (refraction_ratio * sin_theta < 1.0f || reflectance(cos_theta, refraction_ratio) > random::static_float.get())
		{
			const direction3 reflected = direction3(reflect(raycast.direction, hit.normal));
			scattered = ray(hit.point, reflected);
		}
		else
		{
			const direction3 refracted = direction3(refract(raycast.direction, hit.normal, refraction_ratio));
			scattered = ray(hit.point, refracted);
		}

		attenuation = color(1.0f, 1.0f, 1.0f);
		return true;
	}

	float index_of_refraction()
	{
		return m_index_of_refraction;
	}
	
	void index_of_refraction(float value)
	{
		m_index_of_refraction = value;
		m_inv_index_of_refraction = 1.0f / value;
	}

	static dielectric_material& default_material()
	{
		static dielectric_material value{"Default Dielectric", 1.0f};
		return value;
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		return std::make_shared<serializable_node_base>(
			name, serializable_list{
				std::make_shared<serializable_node<float>>("IOR", &m_index_of_refraction),
				material::serialize()
			}
		);
	}

private:
	static float reflectance(float cosine, float ref_index)
	{
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_index) / (1 + ref_index);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * powf((1 - cosine), 5);
	}

private:
	float m_index_of_refraction = 1.0f;
	float m_inv_index_of_refraction = 1.0f;
};

#pragma once

#include "material.h"

#include "core/color.h"
#include "core/hittable.h"

class dielectric_material : public material
{
public:
	explicit dielectric_material(const char* name, double index_of_refraction)
		: material(name)
	{
		this->index_of_refraction(index_of_refraction);
	}

	bool scatter(const ray& raycast, const hit_info& hit, color& attenuation, ray& scattered) const override
	{
		const double refraction_ratio = hit.front_face ? m_inv_index_of_refraction : m_index_of_refraction;
		const double cos_theta = fmin(dot(-raycast.direction(), hit.normal), 1.0);
		const double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
		if (refraction_ratio * sin_theta < 1.0 || reflectance(cos_theta, refraction_ratio) > random_double())
		{
			const direction3 reflected = direction3(reflect(raycast.direction(), hit.normal));
			scattered = ray(hit.point, reflected);
		}
		else
		{
			const direction3 refracted = direction3(refract(raycast.direction(), hit.normal, refraction_ratio));
			scattered = ray(hit.point, refracted);
		}

		attenuation = color(1.0, 1.0, 1.0);
		return true;
	}

	double index_of_refraction()
	{
		return m_index_of_refraction;
	}
	
	void index_of_refraction(double value)
	{
		m_index_of_refraction = value;
		m_inv_index_of_refraction = 1.0 / value;
	}

	static dielectric_material& default_material()
	{
		static dielectric_material value{"default dielectric", 1.0};
		return value;
	}

private:
	static double reflectance(double cosine, double ref_index)
	{
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_index) / (1 + ref_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}

private:
	double m_index_of_refraction = 1.0;
	double m_inv_index_of_refraction = 1.0;
};

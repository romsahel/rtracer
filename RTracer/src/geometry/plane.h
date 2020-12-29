#pragma once

#include "core/utility.h"
#include "core/hittable.h"

#include "materials/lambertian_material.h"

class plane : public hittable
{
public:
	material* material = &lambertian_material::default_material();

	plane(const point3& point, const direction3& normal)
		: m_point(point),
		  m_normal(normalize(normal))
	{
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const override
	{
		double denominator = dot(m_normal, ray.direction());
		bool is_hit = std::abs(denominator) > epsilon; // ray and planes are not parallel
		if (is_hit) 
		{
			const double distance = dot(m_point - ray.origin(), m_normal) / denominator;
			if (distance >= t_min && distance <= t_max)
			{
				info.distance = distance;
				info.point = ray.at(info.distance);
				info.set_face_normal(ray, m_normal);
			}
			else
			{
				is_hit = false;
			}
		}

		return is_hit;
	}

private:
	point3 m_point;
	direction3 m_normal;
};

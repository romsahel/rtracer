#pragma once

#include "core/utility.h"
#include "core/hittable.h"

#include "materials/lambertian_material.h"

class plane : public hittable
{
public:
	material* material = &lambertian_material::default_material();

	plane(const char* name, const point3& point, const direction3& normal)
		: hittable(name), point(point), normal(normalize(normal))
	{
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const override
	{
		double denominator = dot(normal, ray.direction());
		bool is_hit = std::abs(denominator) > epsilon; // ray and planes are not parallel
		if (is_hit)
		{
			const double distance = dot(point - ray.origin(), normal) / denominator;
			if (distance >= t_min && distance <= t_max)
			{
				info.distance = distance;
				info.point = ray.at(info.distance);
				info.set_face_normal(ray, normal);
			}
			else
			{
				is_hit = false;
			}
		}

		return is_hit;
	}

	point3 point;
	direction3 normal;
};

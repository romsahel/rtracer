#pragma once

#include "core/hittable.h"

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class sphere : public hittable
{
public:
	sphere(const char* name, const point3& center, double radius)
		: hittable(name), center(center), radius(radius)
	{
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const override
	{
		vec3 oc = ray.origin() - center;
		double a = ray.direction().length_squared();
		double half_b = dot(ray.direction(), oc);
		double c = oc.length_squared() - radius * radius;
		double squared_discriminant = half_b * half_b - a * c;
		bool is_hit = squared_discriminant >= 0;
		if (is_hit)
		{
			double discriminant = std::sqrt(squared_discriminant);
			double root = (-half_b - discriminant) / a;
			is_hit = root >= t_min && root <= t_max;
			if (!is_hit)
			{
				root = (-half_b + discriminant) / a;
				is_hit = root >= t_min && root <= t_max;
			}

			if (is_hit)
			{
				info.distance = root;
				info.point = ray.at(info.distance);
				const direction3 outward_normal = direction3((info.point - center) / radius);
				info.set_face_normal(ray, outward_normal);
				info.material = material;
			}
		}

		return is_hit;
	}

	point3 center;
	double radius;
};

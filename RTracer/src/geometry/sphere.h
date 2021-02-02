#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class sphere : public hittable
{
public:
	sphere(const char* name, const point3& center, float radius)
		: hittable(name), center(center), radius(radius)
	{
	}


	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		return hit(*this, ray, t_min, t_max, info);
	}

	static inline bool hit(sphere& sphere, const ray& ray, float t_min, float t_max, hit_info& info)
	{
		vec3 oc = ray.origin - sphere.center;
		float half_b = dot(ray.direction, oc);
		float c = oc.length_squared() - sphere.radius * sphere.radius;
		float squared_discriminant = half_b * half_b - c;
		bool is_hit = squared_discriminant >= 0;
		if (is_hit)
		{
			float discriminant = std::sqrt(squared_discriminant);
			float root = (-half_b - discriminant);
			is_hit = root >= t_min && root <= t_max;
			if (!is_hit)
			{
				root = (-half_b + discriminant);
				is_hit = root >= t_min && root <= t_max;
			}

			if (is_hit)
			{
				info.distance = root;
				info.point = ray.at(info.distance);
				const direction3 outward_normal = direction3((info.point - sphere.center) / sphere.radius);
				info.set_face_normal(ray, outward_normal);
				info.material = sphere.material;
				set_uv_at(outward_normal, info.uv_coordinates);
				info.object = &sphere;
			}
		}

		return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		const vec3 size(radius);
		output_aabb = aabb(point3(center - size), point3(center + size));
		return true;
	}

	point3 center;
	float radius;

private:
	static void set_uv_at(const vec3& p, vec3& uv_coordinates)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		const float theta = acos(-p.y());
		const float phi = atan2(-p.z(), p.x()) + constants::pi;
		uv_coordinates.x() = phi / (2 * constants::pi);
		uv_coordinates.y() = theta * constants::inv_pi;
	}
};

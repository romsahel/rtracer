#pragma once

#include "core/aabb.h"
#include "core/hittable.h"
#include <math.h>       /* isnan, sqrt */
bool use_transform = false;

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class sphere : public hittable
{
public:
	sphere(const char* name, const point3& center, float radius)
		: hittable(name), radius(radius)
	{
		transform = translate(transform, center);
		inv_transform = inverse(transform);
	}

	void update() override
	{
		const vec3 size(radius);
		bbox = aabb(point3(-size), point3(size));
		bbox.transform(transform);
	}


	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		const vec3 oc = ray.origin;
		const float half_b = dot(oc, ray.direction);
		const float c = length2(oc) - radius * radius;
		const float squared_discriminant = half_b * half_b - c;

		bool is_hit = squared_discriminant >= 0;
		if (is_hit)
		{
			const float discriminant = std::sqrt(squared_discriminant);
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
				const direction3 outward_normal = info.point / radius;
				info.set_face_normal(ray.direction, outward_normal);
				info.material = material;
				
				set_uv_at(outward_normal, info.uv_coordinates);
				info.object = this;

			}
		}

		return is_hit;
	}

	float radius;

private:
	static void set_uv_at(const vec3& p, vec2& uv_coordinates)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		static const float inv_2pi = 1.0f / (2 * constants::pi);
		const float theta = acos(-p.y);
		const float phi = atan2(-p.z, p.x) + constants::pi;
		uv_coordinates.x = phi * inv_2pi;
		uv_coordinates.y = theta * constants::inv_pi;
	}
};

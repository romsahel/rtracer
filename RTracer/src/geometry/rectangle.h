#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

class rectangle : public hittable
{
public:
	explicit rectangle(const char* name = "", point3 position = point3(vector3::zero()), float width = 1.0f,
	                   float height = 1.0f)
		: hittable(name), width(width), height(height)
	{
		transform = translate(transform, position);
		inv_transform = inverse(transform);
	}

	void update() override
	{
		inv_transform = inverse(transform);

		const vec3 offset{width * 0.5f, height * 0.5f, constants::epsilon};
		bbox = aabb(point3(-offset), point3(offset));
		bbox.transform(transform);
	}

	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		const float t = -ray.origin.z * ray.inv_direction.z;
		bool is_hit = t >= t_min && t <= t_max;
		if (is_hit)
		{
			const vec2 offset{width * 0.5f, height * 0.5f};
			const vec2 hitpoint = ray.origin + t * ray.direction;
			is_hit = all(greaterThan(hitpoint, -offset)) && all(lessThan(hitpoint, offset));
			if (is_hit)
			{
				info.distance = t;
				info.point = ray.at(info.distance);
				const direction3 outward_normal(0.0f, 0.0f, 1.0f);
				info.set_face_normal(ray.direction, outward_normal);
				info.material = material;
				info.uv_coordinates = (hitpoint + offset) / (offset + offset);
				info.object = this;
			}
		}

		return is_hit;
	}

	float width;
	float height;
};

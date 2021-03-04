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
		outward_normal = direction3(0.0f, 0.0f, 1.0f);
		const vec3 offset{width * 0.5f, height * 0.5f, constants::epsilon};
		m_bbox = aabb(point3(-offset), point3(offset));
		inv_transform = inverse(transform);
		m_bbox.transform(transform);
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
				info.set_face_normal(ray, outward_normal);
				info.material = material;
				info.uv_coordinates = (hitpoint + offset) / (offset + offset);
				info.object = this;
			}
		}

		return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		output_aabb = m_bbox;
		return true;
	}

	float width;
	float height;

private:
	aabb m_bbox;
	direction3 outward_normal;
};

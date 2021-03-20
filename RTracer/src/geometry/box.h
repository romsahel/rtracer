#pragma once

#include "core/aabb.h"
#include "abstract/hittable.h"

class box : public hittable
{
public:
	box(const char* name, vec3 size = vec3(1, 1, 1)) :
		hittable(name), size(size)
	{
	}

	void internal_update() override
	{
		const auto extent = vec3(size * 0.5f);
		m_aabb = aabb(point3(-extent), point3(extent));
		bbox = aabb(m_aabb);
	}

	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		const auto [has_hit, axis, distance] = m_aabb.hit_with_info(ray, t_min, t_max);
		if (has_hit)
		{
			info.distance = distance;
			info.point = ray.at(distance);
			direction3 outward_normal(0.0f);
			outward_normal[axis] = 1.0f;
			info.set_face_normal(ray.direction, outward_normal);
			const vec2 offset{size * outward_normal};
			info.uv_coordinates = (vec2(info.point) + offset) / (offset + offset);
			
			info.object = this;
			info.material = material;
			return true;
		}
		return false;
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		auto node = hittable::serialize();
		node->children.push_back(std::make_shared<serializable_node<vec3>>("Size", &size));
		return node;
	}

	[[nodiscard]] hittable* clone() const override
	{
		return new box(*this);
	}

	vec3 size;

private:
	aabb m_aabb;
};

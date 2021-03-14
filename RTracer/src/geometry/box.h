#pragma once

#include "rectangle.h"
#include "core/hittable.h"

class box : public hittable
{
public:
	box(const char* name, vec3 size = vec3(1, 1, 1)) :
	hittable(name), size(size)
	{
	}
	
	void update() override
	{
		inv_transform = inverse(transform);

		// front
		m_sides[0].transform = translate(point3(0, 0, size.z * 0.5f));
		m_sides[0].size = vec2(size.x, size.y);
		m_sides[0].update();

		// back
		m_sides[1].transform = translate(point3(0, 0, -size.z * 0.5f));
		m_sides[1].size = vec2(size.x, size.y);
		m_sides[1].update();

		// left
		m_sides[2].transform = rotate(translate(point3(-size.x * 0.5f, 0, 0)), glm::radians(-90.0f), vec3{0.0f, 1.0f, 0.0f});
		m_sides[2].size = vec2(size.z, size.y);
		m_sides[2].update();

		// right
		m_sides[3].transform = rotate(translate(point3(size.x * 0.5f, 0, 0)), glm::radians(90.0f), vec3{0.0f, 1.0f, 0.0f});
		m_sides[3].size = vec2(size.z, size.y);
		m_sides[3].update();

		// top
		m_sides[4].transform = rotate(translate(point3(0, size.y * 0.5f, 0)), glm::radians(90.0f), vec3{1.0f, 0.0f, 0.0f});
		m_sides[4].size = vec2(size.x, size.z);
		m_sides[4].update();

		// bottom
		m_sides[5].transform = rotate(translate(point3(0, -size.y * 0.5f, 0)), glm::radians(-90.0f), vec3{1.0f, 0.0f, 0.0f});
		m_sides[5].size = vec2(size.x, size.z);
		m_sides[5].update();

		const auto extent = vec3(size * 0.5f);
		bbox = aabb(point3(-extent), point3(extent));
		bbox.transform(transform);
	}

	bool hit(const ray& ray, float t_min, float /*t_max*/, hit_info& info) override
	{
		bool is_hit = false;
		for (int i = 0; i < 6; ++i)
		{
			is_hit |= m_sides[i].base_hit(ray, t_min, info.distance, info);
		}
		return is_hit;
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		auto node = hittable::serialize();
		node->children.push_back(std::make_shared<serializable_node<vec3>>("Size", &size));
		return node;
	}

	vec3 size;

private:
	rectangle m_sides[6];
};

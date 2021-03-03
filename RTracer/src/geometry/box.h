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
		
		m_sides[0].transform = translate(point3(0, 0, -size.z * 0.5f));
		m_sides[0].width = size.x;
		m_sides[0].height = size.y;
		m_sides[0].update();
		m_sides[1].transform = translate(point3(0, 0, size.z * 0.5f));
		m_sides[1].width = size.x;
		m_sides[1].height = size.y;
		m_sides[1].update();

		m_sides[2].transform = rotate(translate(point3(-size.x * 0.5f, 0, 0)), glm::radians(90.0f), vec3{0.0f, 1.0f, 0.0f});
		m_sides[2].width = size.z;
		m_sides[2].height = size.y;
		m_sides[2].update();
		m_sides[3].transform = rotate(translate(point3(size.x * 0.5f, 0, 0)), glm::radians(90.0f), vec3{0.0f, 1.0f, 0.0f});
		m_sides[3].width = size.z;
		m_sides[3].height = size.y;
		m_sides[3].update();

		m_sides[4].transform = rotate(translate(point3(0, size.y * 0.5f, 0)), glm::radians(90.0f), vec3{1.0f, 0.0f, 0.0f});
		m_sides[4].width = size.z;
		m_sides[4].height = size.x;
		m_sides[4].update();
		m_sides[5].transform = rotate(translate(point3(0, -size.y * 0.5f, 0)), glm::radians(90.0f), vec3{1.0f, 0.0f, 0.0f});
		m_sides[5].width = size.z;
		m_sides[5].height = size.x;
		m_sides[5].update();
	}

	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		bool is_hit = false;
		for (int i = 0; i < 6; ++i)
		{
			is_hit |= m_sides[i].base_hit(ray, t_min, info.distance, info);
		}
		return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		auto extent = vec3(size * 0.5f);
		output_aabb = aabb(point3(-extent), point3(extent));
		output_aabb.transform(transform);
		return true;
	}

	vec3 size;

private:
	rectangle m_sides[6];
};

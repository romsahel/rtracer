#pragma once

#include "rectangle.h"
#include "core/hittable.h"

class box : public hittable
{
public:
	box(const char* name, vec3 size = vec3(1, 1, 1)) :
	hittable(name), size(size)
	{
		m_sides[2].right_axis(1);
		m_sides[3].right_axis(1);
		m_sides[4].right_axis(2);
		m_sides[5].right_axis(2);
	}

	void update() override
	{
		m_sides[0].position = point3(0, 0, -size.z() * 0.5);
		m_sides[0].width = size.x();
		m_sides[0].height = size.y();
		m_sides[0].update();
		m_sides[1].position = point3(0, 0, size.z() * 0.5);
		m_sides[1].width = size.x();
		m_sides[1].height = size.y();
		m_sides[1].update();

		m_sides[2].position = point3(-size.x() * 0.5, 0, 0);
		m_sides[2].width = size.y();
		m_sides[2].height = size.z();
		m_sides[2].update();
		m_sides[3].position = point3(size.x() * 0.5, 0, 0);
		m_sides[3].width = size.y();
		m_sides[3].height = size.z();
		m_sides[3].update();

		m_sides[4].position = point3(0, size.y() * 0.5, 0);
		m_sides[4].width = size.z();
		m_sides[4].height = size.x();
		m_sides[4].update();
		m_sides[5].position = point3(0, -size.y() * 0.5, 0);
		m_sides[5].width = size.z();
		m_sides[5].height = size.x();
		m_sides[5].update();
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) override
	{
		bool is_hit = false;
		for (int i = 0; i < 6; ++i)
		{
			is_hit |= m_sides[i].hit(ray, t_min, info.distance, info);
		}
		return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		auto extent = vec3(size * 0.5);
		output_aabb = aabb(point3(-extent), point3(extent));
		return true;
	}

	vec3 size;

private:
	rectangle m_sides[6];
};

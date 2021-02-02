#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

class rectangle : public hittable
{
public:
	explicit rectangle(const char* name = "", point3 position = point3(vec3::zero()), float width = 1.0f, float height = 1.0f)
		: hittable(name), position(position), width(width), height(height)
	{
	}

	void update() override
	{
		outward_normal = direction3(0.0f);
		outward_normal[m_forward_axis] = flip_normal * 1.0f;
		vec3 offset;
		offset[m_right_axis] = width * 0.5f;
		offset[m_up_axis] = height * 0.5f;
		offset[m_forward_axis] = constants::epsilon;
		m_bbox = aabb(point3(position - offset), point3(position + offset));
	}


	bool hit(const ray& transformed_ray, float t_min, float t_max, hit_info& info) override
	{
		float t = (position[m_forward_axis] - transformed_ray.origin[m_forward_axis]) / transformed_ray.direction[
			m_forward_axis];
		bool is_hit = t >= t_min && t <= t_max;
		if (is_hit)
		{
			float x = transformed_ray.origin[m_right_axis] + t * transformed_ray.direction[m_right_axis];
			float y = transformed_ray.origin[m_up_axis] + t * transformed_ray.direction[m_up_axis];

			is_hit = x >= m_bbox.minimum[m_right_axis] && x <= m_bbox.maximum[m_right_axis]
				&& y >= m_bbox.minimum[m_up_axis] && y <= m_bbox.maximum[m_up_axis];
			if (is_hit)
			{
				info.distance = t;
				info.point = transformed_ray.at(info.distance);
				info.set_face_normal(transformed_ray, outward_normal);
				info.material = material;
				info.uv_coordinates.x() = (x - m_bbox.minimum[m_right_axis]) / width;
				info.uv_coordinates.y() = (y - m_bbox.minimum[m_up_axis]) / height;
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

	inline void right_axis(int index)
	{
		m_right_axis = index;
		m_up_axis = (index + 1) % 3;
		m_forward_axis = (index + 2) % 3;
	}

	inline int right_axis()
	{
		return m_right_axis;
	}

	point3 position;
	float width;
	float height;

	float flip_normal = 1.0;

private:
	aabb m_bbox;
	int m_right_axis = 0;
	int m_up_axis = 1;
	int m_forward_axis = 2;
	direction3 outward_normal;
};

#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

class rectangle : public hittable
{
public:
	explicit rectangle(const char* name = "", point3 position = point3(vec3::zero()), double width = 1.0, double height = 1.0)
		: hittable(name), position(position), width(width), height(height)
	{
	}

	void update()
	{
		outward_normal = direction3(0.0);
		outward_normal[m_forward_axis] = 1.0;

		vec3 offset;
		offset[m_right_axis] = width * 0.5;
		offset[m_up_axis] = height * 0.5;
		offset[m_forward_axis] = constants::epsilon;
		m_bbox = aabb(point3(position - offset), point3(position + offset));
	}

	//bool bounding_box(aabb& output_aabb) const override
	//{
	//	output_aabb = m_bbox;
	//	return true;
	//}

	inline void right_axis(char index)
	{
		m_right_axis = index;
		m_up_axis = m_right_axis == 2 ? 0 : m_right_axis + 1;
		m_forward_axis = m_up_axis == 2 ? 0 : m_up_axis + 1;
	}

	inline int right_axis()
	{
		return m_right_axis;
	}
	
	unsigned char m_right_axis = 0;
	unsigned char m_up_axis = 1;
	unsigned char m_forward_axis = 2;
	
	point3 position;
	double width;
	double height;

	direction3 outward_normal;

	aabb m_bbox;
};

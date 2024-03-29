﻿#pragma once

#include "serializable.h"
#include "serializable_node.h"
#include "core/vec3.h"
#include "core/ray.h"

class camera : public serializable
{
public:
	camera(float aspect_ratio)
	{
		m_aspect_ratio = aspect_ratio;
		m_focal_length = 1.0f;
		update();
	}

	float aspect_ratio() const
	{
		return m_aspect_ratio;
	}

	ray compute_ray_to(float x_pixel, float y_pixel) const
	{
		vec3 offset{0.0f};
		if (aperture > constants::epsilon)
		{
			const vec3 disk = aperture * 2.0f * vector3::random_in_unit_disk();
			offset = m_u * disk.x + m_v * disk.y;
		}
		return ray(point3(origin + offset), direction3(
			           m_lower_left_corner + x_pixel * m_horizontal + y_pixel * m_vertical - origin - offset));
	}

	void update()
	{
		const auto h = tan(degrees_to_radians(vertical_fov) / 2.0f);
		m_viewport_height = 2.0f * h;
		m_viewport_width = m_aspect_ratio * m_viewport_height;

		m_w = normalize(origin - target);
		m_u = normalize(cross(vector3::up(), m_w));
		m_v = cross(m_w, m_u);

		m_horizontal = focus_distance * m_viewport_width * m_u;
		m_vertical = focus_distance * m_viewport_height * m_v;
		m_lower_left_corner = point3(origin - m_horizontal * 0.5f - m_vertical * 0.5f - focus_distance * m_w);

		m_has_changed = true;
	}

	bool m_has_changed;

	bool has_changed()
	{
		bool flag = m_has_changed;
		m_has_changed = false;
		return flag;
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		return std::make_shared<serializable_node_base>(
			"Camera", serializable_list{
				std::make_shared<serializable_node<point3>>("Origin", &origin),
				std::make_shared<serializable_node<point3>>("Target", &target),
				std::make_shared<serializable_node<float>>("Vertical FoV", &vertical_fov),
				std::make_shared<serializable_node<float>>("Aperture", &aperture),
				std::make_shared<serializable_node<float>>("Focus distance", &focus_distance),
			}
		);
	}

	point3 origin = point3(vector3::backward() * 2.0f);
	point3 target = point3(vector3::zero());
	float vertical_fov = 70.0f;
	float aperture = 0.0f;
	float focus_distance = 2.0f;

private:
	float m_aspect_ratio;
	float m_viewport_height;
	float m_viewport_width;
	float m_focal_length;
	vec3 m_horizontal;
	vec3 m_vertical;
	point3 m_lower_left_corner;

	vec3 m_w;
	vec3 m_u;
	vec3 m_v;
};

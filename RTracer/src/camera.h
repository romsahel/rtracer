#pragma once

#include "core/vec3.h"
#include "core/direction3.h"
#include "core/point3.h"
#include "core/vec3_utility.h"
#include "core/ray.h"

class camera
{
public:
	camera(double aspect_ratio)
	{
		m_aspect_ratio = aspect_ratio;
		m_focal_length = 1.0;
		update();
	}

	double aspect_ratio() const
	{
		return m_aspect_ratio;
	}

	ray compute_ray_to(double x_pixel, double y_pixel) const
	{
		vec3 offset = 0.0;
		if (aperture > epsilon)
		{
			const vec3 disk = aperture * 2.0 * vec3::random_in_unit_disk();
			offset = m_u * disk.x() + m_v * disk.y();
		}
		return ray(point3(origin + offset), direction3(
			           m_lower_left_corner + x_pixel * m_horizontal + y_pixel * m_vertical - origin - offset));
	}

	void update()
	{
		const auto h = tan(degrees_to_radians(vertical_fov) / 2);
		m_viewport_height = 2.0 * h;
		m_viewport_width = m_aspect_ratio * m_viewport_height;

		m_w = normalize(origin - target);
		m_u = normalize(cross(vec3::up(), m_w));
		m_v = cross(m_w, m_u);

		m_horizontal = focus_distance * m_viewport_width * m_u;
		m_vertical = focus_distance * m_viewport_height * m_v;
		m_lower_left_corner = point3(origin - m_horizontal * 0.5 - m_vertical * 0.5 - focus_distance * m_w);
	}

	point3 origin = point3(vec3::backward() * 2.0);
	point3 target = point3(vec3::zero());
	double vertical_fov = 70.0;
	double aperture = 0.0;
	double focus_distance = 2.0;

private:
	double m_aspect_ratio;
	double m_viewport_height;
	double m_viewport_width;
	double m_focal_length;
	vec3 m_horizontal;
	vec3 m_vertical;
	point3 m_lower_left_corner;

	vec3 m_w;
	vec3 m_u;
	vec3 m_v;
};

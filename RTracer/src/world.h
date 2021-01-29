#pragma once

#include <vector>
#include <type_traits>


#include "bvh.h"
#include "core/hittable.h"
#include "materials/material.h"

inline bool hit_spheres(std::vector<sphere>& spheres, const ray& ray, double t_min, hit_info& info)
{
	bool has_hit = false;
	for (sphere& sphere : spheres)
	{
		const vec3 oc = ray.origin - sphere.center;
		const double half_b = -dot(ray.direction, oc);
		const double c = oc.length_squared() - sphere.radius * sphere.radius;
		const double squared_discriminant = half_b * half_b - c;
		bool is_hit = squared_discriminant >= 0;
		if (is_hit)
		{
			const double discriminant = std::sqrt(squared_discriminant);
			double root = half_b - discriminant;
			is_hit = root >= t_min && root <= info.distance;
			if (!is_hit)
			{
				root = half_b + discriminant;
				is_hit = root >= t_min && root <= info.distance;
			}

			if (is_hit)
			{
				info.distance = root;
				info.point = ray.at(info.distance);
				const direction3 outward_normal = direction3((info.point - sphere.center) / sphere.radius);
				info.set_face_normal(ray, outward_normal);
				// p: a given point on the sphere of radius one, centered at the origin.
				// u: returned value [0,1] of angle around the Y axis from X=-1.
				// v: returned value [0,1] of angle from Y=-1 to Y=+1.
				//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
				//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
				//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

				const double theta = acos(-outward_normal.y());
				const double phi = atan2(-outward_normal.z(), outward_normal.x()) + constants::pi;
				info.uv_coordinates.x() = phi * constants::inv_pi * 0.5;
				info.uv_coordinates.y() = theta * constants::inv_pi;
				info.object = &sphere;
			}
			has_hit |= is_hit;
		}
	}
	return has_hit;
}

inline bool hit_rectangles(std::vector<rectangle>& rectangles, const ray& ray, double t_min, hit_info& info)
{
	bool has_hit = false;
	for (auto& rect : rectangles)
	{
		double t = (rect.position[rect.m_forward_axis] - ray.origin[rect.m_forward_axis])
			/ ray.direction[rect.m_forward_axis];
		bool is_hit = t >= t_min && t <= info.distance;
		if (is_hit)
		{
			double x = ray.origin[rect.m_right_axis] + t * ray.direction[rect.m_right_axis];
			double y = ray.origin[rect.m_up_axis] + t * ray.direction[rect.m_up_axis];

			is_hit = x >= rect.m_bbox.minimum[rect.m_right_axis] && x <= rect.m_bbox.maximum[rect.m_right_axis]
				&& y >= rect.m_bbox.minimum[rect.m_up_axis] && y <= rect.m_bbox.maximum[rect.m_up_axis];
			if (is_hit)
			{
				info.distance = t;
				info.point = ray.at(info.distance);
				info.set_face_normal(ray, rect.outward_normal);
				info.uv_coordinates.x() = (x - rect.m_bbox.minimum[rect.m_right_axis]) / rect.width;
				info.uv_coordinates.y() = (y - rect.m_bbox.minimum[rect.m_up_axis]) / rect.height;
				info.object = &rect;
			}
		}
		has_hit |= is_hit;
	}

	return has_hit;
}

class world
{
public:
	template <typename T, class... Args>
	T& add(Args&&... args)
	{
		if constexpr (std::is_assignable<sphere, T>::value)
		{
			return spheres.emplace_back(std::forward<Args>(args)...);
		}
		if constexpr (std::is_assignable<rectangle, T>::value)
		{
			return rectangles.emplace_back(std::forward<Args>(args)...);
		}
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info)
	{
		info.distance = t_max;

		//if (use_bvh)
		//{
		//	return m_bvh->hit(ray, t_min, t_max, info);
		//}
		//else
		{
			bool has_hit = false;
			has_hit |= hit_spheres(spheres, ray, t_min, info);
			has_hit |= hit_rectangles(rectangles, ray, t_min, info);
			return has_hit;
		}
	}

	void signal_scene_change()
	{
		//m_bvh = new bvh_node(m_list, 0, m_list.size());
	}

	bool use_bvh{false};

	std::vector<sphere> spheres;
	std::vector<rectangle> rectangles;

private:
	hittable* m_bvh{nullptr};
};

#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class xy_rect : public hittable
{
public:
	xy_rect(const char* name, point3 position, double width, double height)
		: hittable(name), center(position), width(width), height(height)
	{
		update();
	}

	void update()
	{
		const double radians_x = degrees_to_radians(rotation.x());
		const double radians_y = degrees_to_radians(rotation.y());
		sin_thetas[0] = sin(radians_x);
		cos_thetas[0] = cos(radians_x);
		sin_thetas[1] = sin(radians_y);
		cos_thetas[1] = cos(radians_y);
		auto p0 = unmove(point3(unrotate(this->p0() * 2.0))); //  * 2.0 is a workaround: when rotating plane by 90x and 45y, the sides were cut: its x coordinates are halved compared to what it's supposed to be
		auto p1 = unmove(point3(unrotate(this->p1() * 2.0))); //  * 2.0 is a workaround 
		m_bbox = aabb(center, center);
		m_bbox.encapsulate(p0);
		m_bbox.encapsulate(p1);
	}

	ray move(const ray& ray)
	{
		return ::ray(point3(ray.origin() - center), ray.direction());
	}

	point3 unmove(const point3& p)
	{
		return point3(p + center);
	}

	vec3 rotate_x(const vec3& v, double factor = 1.0)
	{
		return vec3{
			v.x(),
			cos_thetas[0] * v.y() - factor * sin_thetas[0] * v.z(),
			factor * sin_thetas[0] * v.y() + cos_thetas[0] * v.z()
		};
	}

	vec3 rotate_y(const vec3& v, double factor = 1.0)
	{
		return vec3{
			cos_thetas[1] * v.x() - factor * sin_thetas[1] * v.z(),
			v.y(),
			factor * sin_thetas[1] * v.x() + cos_thetas[1] * v.z()
		};
	}

	vec3 rotate(const vec3& v, double factor = 1.0)
	{
		return rotate_x(rotate_y(v, factor));
	}

	ray rotate(const ray& ray)
	{
		return ::ray(point3(rotate(ray.origin())), direction3(rotate(ray.direction())));
	}

	vec3 unrotate(const vec3& v)
	{
		return rotate(v, -1.0);
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) override
	{
		auto moved_ray = move(ray);
		::ray transformed_ray = rotate(moved_ray);

		double t = (-transformed_ray.origin().z()) / transformed_ray.direction().z();
		bool is_hit = t >= t_min && t <= t_max;
		if (is_hit)
		{
			double x = transformed_ray.origin().x() + t * transformed_ray.direction().x();
			double y = transformed_ray.origin().y() + t * transformed_ray.direction().y();
			auto p0 = this->p0();
			auto p1 = this->p1();
			is_hit = x >= p0.x() && x <= p1.x() && y >= p0.y() && y <= p1.y();
			if (is_hit)
			{
				info.distance = t;
				info.point = ray.at(info.distance);
				const direction3 outward_normal = direction3(vec3::forward());
				info.set_face_normal(ray, outward_normal);
				info.material = material;
				info.uv_coordinates.x() = (x - p0.x()) / width;
				info.uv_coordinates.y() = (y - p0.y()) / height;
				info.object = this;

				info.point = unmove(info.point);
				info.set_face_normal(moved_ray, info.normal);

				info.point = point3(unrotate(info.point));
				info.normal = direction3(unrotate(info.normal));
				info.set_face_normal(transformed_ray, info.normal);
			}
		}

		return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		output_aabb = m_bbox;
		return true;
	}

	inline point3 p0() const
	{
		return point3(
			-width * 0.5,
			-height * 0.5,
			-constants::epsilon
		);
	}

	inline point3 p1() const
	{
		return point3(
			+width * 0.5,
			+height * 0.5,
			+constants::epsilon
		);
	}

	point3 center;
	vec3 rotation{vec3::zero()};
	double width;
	double height;

private:
	double sin_thetas[2];
	double cos_thetas[2];
	aabb m_bbox;
};

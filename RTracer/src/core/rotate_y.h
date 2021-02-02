#pragma once
#include "aabb.h"
#include "hittable.h"

class transform_root : public hittable
{
public:
	explicit transform_root(const char* name, hittable* object)
		: hittable(name), object(object)
	{
	}

	~transform_root() override
	{
		delete object;
	}

	void update() override
	{
		object->update();
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		output_aabb = bbox;
		return has_bbox;
	}

	hittable* object;
	bool has_bbox{false};
	aabb bbox;
};

class translator : public transform_root
{
public:
	point3 position;

	translator(hittable* object): transform_root(object->name.c_str(), object)
	{
	}

	void update() override
	{
		transform_root::update();
		has_bbox = object->bounding_box(bbox);
		bbox.minimum += position;
		bbox.maximum += position;
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) override
	{
		auto moved_ray = ::ray(point3(ray.origin() - position), ray.direction());
		if (!object->hit(moved_ray, t_min, t_max, info))
		{
			return false;
		}

		info.point += position;
		info.set_face_normal(moved_ray, info.normal);
		return true;
	}
};

template <int axis>
class rotate_base : public transform_root
{
public:
	rotate_base(hittable* object, double angle): transform_root(object->name.c_str(), object), angle(angle)
	{
	}

	void update() override
	{
		transform_root::update();
		const double radians = degrees_to_radians(angle);
		sin_theta = sin(radians);
		cos_theta = cos(radians);
		has_bbox = object->bounding_box(bbox);
		point3 min(constants::infinity, constants::infinity, constants::infinity);
		point3 max(-constants::infinity, -constants::infinity, -constants::infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					const auto x = i * bbox.maximum.x() + (1 - i) * bbox.minimum.x();
					const auto y = j * bbox.maximum.y() + (1 - j) * bbox.minimum.y();
					const auto z = k * bbox.maximum.z() + (1 - k) * bbox.minimum.z();

					auto tester = rotate(vec3(x, y, z));
					for (int c = 0; c < 3; c++)
					{
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	vec3 rotate(const vec3& v, double sin_factor = 1.0) const
	{
		if constexpr (axis == 0)
		{
			return {
				v.x(),
				cos_theta * v.y() - sin_factor * sin_theta * v.z(),
				sin_factor * sin_theta * v.y() + cos_theta * v.z(),
			};
		}

		if constexpr (axis == 1)
		{
			return {
				cos_theta * v[0] + sin_factor * sin_theta * v[2],
				v.y(),
				sin_factor * -sin_theta * v[0] + cos_theta * v[2],
			};
		}

		if constexpr (axis == 2)
		{
			return {
				cos_theta * v[0] - sin_factor * sin_theta * v[2],
				sin_factor * sin_theta * v[0] + cos_theta * v[2],
				v.z()
			};
		}
	}

	bool hit(const ray& r, double t_min, double t_max, hit_info& info) override
	{
		auto origin = point3(rotate(r.origin(), -1.0));
		auto direction = direction3(rotate(r.direction(), -1.0));
		ray rotated_r(origin, direction);

		if (!object->hit(rotated_r, t_min, t_max, info))
			return false;

		info.point = point3(rotate(info.point));
		info.set_face_normal(rotated_r, direction3(rotate(info.normal)));

		return true;
	}

	double angle{0.0};

private:
	double sin_theta{0.0};
	double cos_theta{0.0};
};

class rotate_x : public rotate_base<0>
{
public:
	explicit rotate_x(hittable* object, double angle = 0.0)
		: rotate_base<0>(object, angle)
	{
	}
};

class rotate_y : public rotate_base<1>
{
public:
	explicit rotate_y(hittable* object, double angle = 0.0)
		: rotate_base<1>(object, angle)
	{
	}
};

class rotate_z : public rotate_base<2>
{
public:
	explicit rotate_z(hittable* object, double angle = 0.0)
		: rotate_base<2>(object, angle)
	{
	}
};

#pragma once
#include "aabb.h"
#include "hittable.h"

class rotate_y : public hittable
{
public:

	rotate_y(hittable* obj, double angle): hittable(obj->name.c_str()), object(obj), angle(angle)
	{
		update();
	}

	void update()
	{
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

					const auto newx = cos_theta * x + sin_theta * z;
					const auto newz = -sin_theta * x + cos_theta * z;

					vec3 tester(newx, y, newz);

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

	bool hit(const ray& r, double t_min, double t_max, hit_info& info) override
	{
		auto origin = r.origin();
		auto direction = r.direction();

		origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
		origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

		direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
		direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

		ray rotated_r(origin, direction);

		if (!object->hit(rotated_r, t_min, t_max, info))
			return false;

		auto p = info.point;
		auto normal = info.normal;

		p[0] = cos_theta * info.point[0] + sin_theta * info.point[2];
		p[2] = -sin_theta * info.point[0] + cos_theta * info.point[2];

		normal[0] = cos_theta * info.normal[0] + sin_theta * info.normal[2];
		normal[2] = -sin_theta * info.normal[0] + cos_theta * info.normal[2];

		info.point = p;
		info.set_face_normal(rotated_r, normal);

		return true;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		output_aabb = bbox;
		return has_bbox;
	}

public:
	hittable* object;
	double angle;

private:
	double sin_theta{0.0};
	double cos_theta{0.0};
	bool has_bbox{false};
	aabb bbox;
};

#pragma once

#include "direction3.h"
#include "point3.h"
#include "ray.h"

class aabb
{
public:
	aabb()
		: minimum(constants::infinity),
		  maximum(-constants::infinity)
	{
	}

	aabb(const point3& min, const point3& max)
		: minimum(min),
		  maximum(max)
	{
	}

	aabb(const point3& center, const vec3& extent)
		: minimum(constants::infinity)
		, maximum(-constants::infinity)
	{
		for (int i = -1; i < 2; i += 2)
		{
			for (int j = -1; j < 2; j += 2)
			{
				for (int k = -1; k < 2; k += 2)
				{
					double x = center.x() + i * extent.x();
					double y = center.y() + j * extent.y();
					double z = center.z() + k * extent.z();

					vec3 position{x,y,z};
					for (int l = 0; l < 3; l++)
					{
						minimum[l] = fmin(minimum[l], position[l]);
						maximum[l] = fmax(maximum[l], position[l]);
					}
				}
			}
		}
	}

	static aabb surrounding(aabb box0, aabb box1)
	{
		point3 small(fmin(box0.minimum.x(), box1.minimum.x()),
		             fmin(box0.minimum.y(), box1.minimum.y()),
		             fmin(box0.minimum.z(), box1.minimum.z()));

		point3 big(fmax(box0.maximum.x(), box1.maximum.x()),
		           fmax(box0.maximum.y(), box1.maximum.y()),
		           fmax(box0.maximum.z(), box1.maximum.z()));

		return aabb(small, big);
	}

	void encapsulate(const point3& p)
	{
		minimum = point3(fmin(minimum.x(), p.x()),
		                 fmin(minimum.y(), p.y()),
		                 fmin(minimum.z(), p.z()));
		maximum = point3(fmax(maximum.x(), p.x()),
		                 fmax(maximum.y(), p.y()),
		                 fmax(maximum.z(), p.z()));
	}

	bool hit(const ray& r, double t_min, double t_max) const
	{
		for (int i = 0; i < 3; i++)
		{
			double t0 = (minimum[i] - r.origin[i]) * r.inv_direction[i];
			double t1 = (maximum[i] - r.origin[i]) * r.inv_direction[i];
			if (r.inv_direction[i] < 0.0)
				std::swap(t0, t1);
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min)
				return false;
		}
		return true;
	}

	vec3 size() const
	{
		return maximum - minimum;
	}

	vec3 extent() const
	{
		return size() * 0.5;
	}

	point3 minimum;
	point3 maximum;
};

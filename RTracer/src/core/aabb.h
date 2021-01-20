#pragma once

#include "direction3.h"
#include "point3.h"
#include "ray.h"

class aabb
{
public:
	aabb()
	{
	}

	aabb(const point3& min, const point3& max)
		: minimum(min),
		  maximum(max)
	{
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
		const point3 origin = r.origin();
		const direction3 direction = r.direction();
		for (int i = 0; i < 3; i++)
		{
			double inv_d = 1.0 / direction[i];
			double t0 = (minimum[i] - origin[i]) * inv_d;
			double t1 = (maximum[i] - origin[i]) * inv_d;
			if (inv_d < 0.0)
				std::swap(t0, t1);
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min)
				return false;
		}
		return true;
	}

	point3 minimum;
	point3 maximum;
};

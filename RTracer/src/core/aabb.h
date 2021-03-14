#pragma once

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

	static aabb surrounding(aabb box0, aabb box1)
	{
		point3 small(fmin(box0.minimum.x, box1.minimum.x),
		             fmin(box0.minimum.y, box1.minimum.y),
		             fmin(box0.minimum.z, box1.minimum.z));

		point3 big(fmax(box0.maximum.x, box1.maximum.x),
		           fmax(box0.maximum.y, box1.maximum.y),
		           fmax(box0.maximum.z, box1.maximum.z));

		return aabb(small, big);
	}

	void encapsulate(const point3& p)
	{
		minimum = point3(fmin(minimum.x, p.x),
		                 fmin(minimum.y, p.y),
		                 fmin(minimum.z, p.z));
		maximum = point3(fmax(maximum.x, p.x),
		                 fmax(maximum.y, p.y),
		                 fmax(maximum.z, p.z));
	}

	void transform(const glm::mat4& m)
	{
		const auto center = (minimum + maximum) * 0.5f;
		const auto extent = this->extent();
		minimum = glm::vec3(constants::infinity);
		maximum = glm::vec3(-constants::infinity);
		for (int i = -1; i < 2; i += 2)
		{
			for (int j = -1; j < 2; j += 2)
			{
				for (int k = -1; k < 2; k += 2)
				{
					vec3 position = m * vec4{center.x + i * extent.x,center.y + j * extent.y,center.z + k * extent.z, 1.0f};
					for (int l = 0; l < 3; l++)
					{
						minimum[l] = fmin(minimum[l], position[l]);
						maximum[l] = fmax(maximum[l], position[l]);
					}
				}
			}
		}
	}

	bool hit(const ray& r, float t_min, float t_max) const
	{
		for (int i = 0; i < 3; i++)
		{
			float t0 = (minimum[i] - r.origin[i]) * r.inv_direction[i];
			float t1 = (maximum[i] - r.origin[i]) * r.inv_direction[i];
			if (r.inv_direction[i] < 0.0f)
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
		return size() * 0.5f;
	}

	point3 minimum;
	point3 maximum;
};

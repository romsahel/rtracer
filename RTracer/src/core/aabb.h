#pragma once

#include "ray.h"
#include "vec3.h"

/// <summary>
/// represents an axis-aligned bounding box
/// it is defined as two points containing the minimum coordinates and maximum coordinates on each axis
/// </summary>
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

	aabb(const point3& center)
		: minimum(center),
		  maximum(center)
	{
	}

	/// <summary>
	/// construct an aabb that surrounds two other aabb
	/// </summary>
	/// <returns></returns>
	static aabb surrounding(aabb box0, aabb box1)
	{
		const point3 small(fmin(box0.minimum.x, box1.minimum.x),
		                   fmin(box0.minimum.y, box1.minimum.y),
		                   fmin(box0.minimum.z, box1.minimum.z));

		const point3 big(fmax(box0.maximum.x, box1.maximum.x),
		                 fmax(box0.maximum.y, box1.maximum.y),
		                 fmax(box0.maximum.z, box1.maximum.z));

		return aabb(small, big);
	}

	/// <summary>
	/// modify the aabb so that it contains the given point
	/// </summary>
	void encapsulate(const point3& p)
	{
		minimum = point3(fmin(minimum.x, p.x),
		                 fmin(minimum.y, p.y),
		                 fmin(minimum.z, p.z));
		maximum = point3(fmax(maximum.x, p.x),
		                 fmax(maximum.y, p.y),
		                 fmax(maximum.z, p.z));
	}

	/// <summary>
	/// transform the aabb by the given transformation matrix
	/// </summary>
	void transform(const glm::mat4& tsf)
	{
		const auto center = (minimum + maximum) * 0.5f;
		const auto extent = this->extent();
		minimum = glm::vec3(constants::infinity);
		maximum = glm::vec3(-constants::infinity);
		for (float i = -1; i < 2.0f; i += 2.0f)
		{
			for (float j = -1; j < 2.0f; j += 2.0f)
			{
				for (float k = -1; k < 2.0f; k += 2.0f)
				{
					const auto position = multiply_point_fast(tsf,
					                                          vec3{
						                                          center.x + i * extent.x,
						                                          center.y + j * extent.y,
						                                          center.z + k * extent.z
					                                          });
					for (int l = 0; l < 3; l++)
					{
						minimum[l] = fmin(minimum[l], position[l]);
						maximum[l] = fmax(maximum[l], position[l]);
					}
				}
			}
		}
	}
	
	/// <summary>
	/// returns true if the given ray hits the aabb at a distance comprised between t_min and t_max
	/// </summary>
	[[nodiscard]] bool hit(const ray& r, float t_min, float t_max) const
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
	
	/// <summary>
	/// returns the size of the aabb
	/// </summary>
	[[nodiscard]] vec3 size() const
	{
		return maximum - minimum;
	}
	
	/// <summary>
	/// returns the extent (half-width) of the aabb
	/// </summary>
	[[nodiscard]] vec3 extent() const
	{
		return size() * 0.5f;
	}

	point3 minimum;
	point3 maximum;
};

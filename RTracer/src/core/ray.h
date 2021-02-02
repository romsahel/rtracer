#pragma once

#include "vec3_utility.h"

/// <summary>
/// represent a ray cast from an origin point in world space towards a direction
/// </summary>
class ray
{
public:
	ray() = default;

	ray(const point3& origin, const direction3& direction)
		: origin(origin),
		  direction(normalize(direction)),
		  inv_direction(1.0 / this->direction.x(), 1.0 / this->direction.y(), 1.0 / this->direction.z())
	{
	}

	/// <summary>
	/// returns the point positioned on the ray, at the given distance from its origin 
	/// </summary>
	point3 at(double t) const
	{
		return point3(origin + direction * t);
	}

	point3 origin;
	direction3 direction;
	direction3 inv_direction;
};

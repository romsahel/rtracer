#pragma once

/// <summary>
/// represent a ray cast from an origin point in world space towards a direction
/// </summary>
class ray
{
public:
	ray() = default;

	/// <summary>
	/// construct a ray from the given origin and direction
	/// </summary>
	ray(const point3& origin, const direction3& direction)
		: origin(origin),
		  direction(normalize(direction)),
		  inv_direction(glm::one<vec3>() / direction)
	{
	}

	/// <summary>
	/// construct a ray from the given origin and direction and pre-computed inverse of its direction
	/// </summary>
	ray(const point3& origin, const direction3& direction, const direction3& inv_direction)
		: origin(origin),
		  direction(direction),
		  inv_direction(inv_direction)
	{
	}

	/// <summary>
	/// returns the point positioned on the ray, at the given distance from its origin 
	/// </summary>
	point3 at(float t) const
	{
		return point3(origin + direction * t);
	}

	point3 origin;
	direction3 direction;
	direction3 inv_direction;
};

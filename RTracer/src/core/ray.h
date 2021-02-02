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
		: m_origin(origin),
		  m_direction(normalize(direction))
	{
	}

	/// <summary>
	/// returns the point positioned on the ray, at the given distance from its origin 
	/// </summary>
	point3 at(double t) const
	{
		return point3(m_origin + m_direction * t);
	}

	/// <summary>
	/// the origin of the ray
	/// </summary>
	const point3& origin() const
	{
		return m_origin;
	}
	
	/// <summary>
	/// the direction of the ray
	/// </summary>
	const direction3& direction() const
	{
		return m_direction;
	}

private:
	point3 m_origin;
	direction3 m_direction;
};

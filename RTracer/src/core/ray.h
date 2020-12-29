#pragma once
#include "vec3.h"

class ray
{
public:
	ray() = default;

	ray(const point3& origin, const direction3& direction)
		: m_origin(origin),
		  m_direction(normalize(direction))
	{
	}

	point3 at(double t) const
	{
		return point3(m_origin + m_direction * t);
	}

	const point3& origin() const
	{
		return m_origin;
	}

	const direction3& direction() const
	{
		return m_direction;
	}

private:
	point3 m_origin;
	direction3 m_direction;
};

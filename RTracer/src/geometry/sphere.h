#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class sphere : public hittable
{
public:
	sphere(const char* name, const point3& center, double radius)
		: hittable(name), center(center), radius(radius)
	{
	}

	point3 center;
	double radius;

private:
	static void set_uv_at(const vec3& p, vec3& uv_coordinates)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		const double theta = acos(-p.y());
		const double phi = atan2(-p.z(), p.x()) + constants::pi;
		uv_coordinates.x() = phi / (2 * constants::pi);
		uv_coordinates.y() = theta * constants::inv_pi;
	}
};

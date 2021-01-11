#include "vec3.h"
#include "vec3_utility.h"

vec3 vec3::random_in_unit_sphere()
{
	while (true)  // TODO: use a nicer algorithm than rejection
	{
		vec3 result = random(-1.0, 1.0);
		if (result.length_squared() >= 1.0) continue;
		return result;
	}
}

vec3 vec3::random_in_unit_disk()
{
	while (true)  // TODO: use a nicer algorithm than rejection
	{
		vec3 result(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0.0);
		if (result.length_squared() >= 1.0) continue;
		return result;
	}
}

vec3 vec3::random_in_hemisphere(const vec3& normal)
{
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

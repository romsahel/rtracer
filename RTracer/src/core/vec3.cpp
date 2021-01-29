#include "vec3.h"
#include "vec3_utility.h"

vec3 vec3::random_in_unit_sphere(double sphere_angle)
{
	const auto u = random::get<double>();
	const auto v = random::get<double>();
	const auto theta = u * sphere_angle;
	const auto phi = std::acos(2.0 * v - 1.0);
	const auto r = std::cbrt(random::get<double>());
	const auto sinTheta = std::sin(theta);
	const auto cosTheta = std::cos(theta);
	const auto sinPhi = std::sin(phi);
	const auto cosPhi = std::cos(phi);
	const auto x = r * sinPhi * cosTheta;
	const auto y = r * sinPhi * sinTheta;
	const auto z = r * cosPhi;
	return vec3(x, y, z);
}

vec3 vec3::random_in_unit_disk()
{
	return random_in_unit_sphere(constants::pi);
}

vec3 vec3::random_in_hemisphere(const vec3& normal)
{
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

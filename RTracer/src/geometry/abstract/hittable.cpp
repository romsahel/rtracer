#include "hittable.h"

#include "materials/lambertian_material.h"

hittable::hittable(const char* name)
	: name(name), material(&lambertian_material::default_material())
{
}

bool hittable::base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info)
{
	const auto origin = vector3::multiply_point_fast(base_ray.origin, inv_transform);
	const auto direction = glm::mat3(inv_transform) * base_ray.direction;
	const auto inv_direction = glm::one<vec3>() / direction;
	const auto transformed_ray = ::ray(origin, direction, inv_direction);

	if (!hit(transformed_ray, t_min, t_max, info))
		return false;

	info.normal = glm::mat3(transform) * info.normal;
	info.point = vector3::multiply_point_fast(info.point, transform);

	return true;
}

void hittable::update()
{
	inv_transform = inverse(transform);
	internal_update();
	bbox.transform(transform);
}

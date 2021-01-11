#include "hittable.h"

#include "materials/lambertian_material.h"

hittable::hittable(const char* name)
	: name(name), material(&lambertian_material::default_material())
{
}

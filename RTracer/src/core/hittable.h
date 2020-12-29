#pragma once

#include "materials/material.h"
#include "core/ray.h"
#include "core/vec3.h"

struct hit_info
{
	point3 point;
	direction3 normal;
	material* material;
	double distance;
	bool front_face = false;

	explicit hit_info(::material* material)
		: material(material)
	{
	}

	inline void set_face_normal(const ray& r, const direction3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : direction3(-outward_normal);
	}
};

class hittable
{
public:
	virtual bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const = 0;
};
#pragma once

#include "direction3.h"
#include "point3.h"
#include "ray.h"
#include "vec3_utility.h"
#include "materials/material.h"

class aabb;
/// <summary>
/// contains information of how the light hits an hittable object
/// </summary>
struct hit_info
{
	// point in world space where the hit occurred
	point3 point;
	// direction for the bouncing ray
	direction3 normal;
	// distance from the origin of the raycast to the hit point
	double distance = -1.0;
	// true if the raycast hit the object from its frontside ; false if it hit from the backside
	bool front_face = false;
	
	// material of the hit object
	material* material;
	// uv coordinate of the material at the hitpoint
	vec3 uv_coordinates;

	explicit hit_info(::material* material)
		: material(material)
	{
	}

	// set both front_face and normal property using the raycast and the outward normal to compute
	inline void set_face_normal(const ray& r, const direction3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : direction3(-outward_normal);
	}
};

/// <summary>
/// represents objects that can be hit by light (e.g. geometry)
/// </summary>
class hittable
{
public:
	explicit hittable(const char* name);
	virtual ~hittable() = default;

	virtual bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const = 0;
	virtual bool bounding_box(aabb& output_aabb) const = 0;

	// name is used for ui and debug purposes
	std::string name;

	// pointer to material used by object for render (raw pointer, lifetime not managed by the object. see world for management)
	// material has a default value to the default lambertian material
	material* material;
};
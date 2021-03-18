#pragma once

#include "vec3.h"

class material;
class hittable;

/// <summary>
/// contains information of how the light hits an hittable object
/// </summary>
struct hit_info
{
	// point in world space where the hit occurred
	point3 point = point3(0.0f);
	// direction for the bouncing ray
	direction3 normal = direction3(0.0f);
	// distance from the origin of the raycast to the hit point
	float distance = constants::infinity;
	// true if the raycast hit the object from its frontside ; false if it hit from the backside
	bool front_face = false;
	
	// uv coordinate of the material at the hitpoint
	vec2 uv_coordinates = vec2(0.0f);
	
	// material of the hit object
	material* material = nullptr;
	// the hit object
	hittable* object = nullptr;

	explicit hit_info(::material* material)
		: material(material)
	{
	}

	// set both front_face and normal property using the raycast and the outward normal to compute
	void set_face_normal(const direction3& ray_direction, const direction3& outward_normal)
	{
		front_face = dot(ray_direction, outward_normal) < 0;
		normal = (static_cast<float>(front_face) * 2.0f - 1.0f) * outward_normal;
	}
};

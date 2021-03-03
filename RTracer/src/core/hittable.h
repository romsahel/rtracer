#pragma once

#include "direction3.h"
#include "point3.h"
#include "ray.h"
#include "vec3_utility.h"
#include "materials/material.h"

class hittable;
class aabb;

inline bool USE_TRANSFORM = true;

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
	float distance = constants::infinity;
	// true if the raycast hit the object from its frontside ; false if it hit from the backside
	bool front_face = false;

	// material of the hit object
	material* material;
	// uv coordinate of the material at the hitpoint
	vec2 uv_coordinates;

	hittable* object;

	explicit hit_info(::material* material)
		: material(material)
	{
	}

	// set both front_face and normal property using the raycast and the outward normal to compute
	inline void set_face_normal(const ray& r, const direction3& outward_normal)
	{
		front_face = dot(r.direction, outward_normal) < 0;
		//normal = outward_normal;
		normal = front_face ? outward_normal : -outward_normal;
		//normal = (static_cast<float>(front_face) * 2.0f - 1.0f) * outward_normal;
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

	__forceinline vec3 multiply_point_fast(const glm::mat4& m, const glm::vec3& v)
	{
		return vec3{
			(m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z) + m[3][0],
			(m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z) + m[3][1],
			(m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z) + m[3][2]
		};
	}

	bool base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info)
	{
		if (!USE_TRANSFORM)
			return hit(base_ray, t_min, t_max, info);

		auto ray = ::ray(
			multiply_point_fast(inv_transform, base_ray.origin), 
			glm::mat3(inv_transform) * base_ray.direction
		);
		if (!hit(ray, t_min, t_max, info))
			return false;

		info.point = multiply_point_fast(transform, info.point);
		info.set_face_normal(ray, glm::mat3(transform) * info.normal);
		return true;
	}

	virtual bool hit(const ray& ray, float t_min, float t_max, hit_info& info) = 0;
	virtual bool bounding_box(aabb& output_aabb) const = 0;

	virtual void update()
	{
	}

	// name is used for ui and debug purposes
	std::string name;

	// pointer to material used by object for render (raw pointer, lifetime not managed by the object. see world for management)
	// material has a default value to the default lambertian material
	material* material;
	glm::mat4 transform = glm::identity<glm::mat4>();
	glm::mat4 inv_transform = glm::identity<glm::mat4>();
};

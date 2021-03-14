#pragma once

#include <glm/gtx/matrix_decompose.hpp>

#include "vec3.h"
#include "aabb.h"
#include "hit_info.h"
#include "ray.h"
#include "serializable.h"
#include "serializable_node.h"

/// <summary>
/// represents objects that can be hit by light (e.g. geometry)
/// </summary>
class hittable : public serializable
{
protected:
	virtual void internal_update() = 0;
	
public:
	explicit hittable(const char* name);

	virtual bool base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info)
	{
		const auto origin = multiply_point_fast(inv_transform, base_ray.origin);
		const auto direction = glm::mat3(inv_transform) * base_ray.direction;
		const auto transformed_ray = ::ray(origin, direction);

		if (!hit(transformed_ray, t_min, t_max, info))
			return false;

		info.normal = glm::mat3(transform) * info.normal;
		info.point = multiply_point_fast(transform, info.point);

		return true;
	}

	virtual bool hit(const ray& ray, float t_min, float t_max, hit_info& info) = 0;
	
	void update()
	{
		inv_transform = inverse(transform);
		internal_update();
		bbox.transform(transform);
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		return std::make_shared<serializable_node_base>(
			name, serializable_list{
				std::make_shared<serializable_transform>(transform)
			}
		);
	}

	// name is used for ui and debug purposes
	std::string name;

	// pointer to material used by object for render (raw pointer, lifetime not managed by the object. see world for management)
	// material has a default value to the default lambertian material
	material* material;
	glm::mat4 transform = glm::identity<glm::mat4>();
	glm::mat4 inv_transform = glm::identity<glm::mat4>();
	aabb bbox;
};

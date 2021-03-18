#pragma once

#include <glm/gtx/matrix_decompose.hpp>

#include "core/vec3.h"
#include "core/aabb.h"
#include "core/hit_info.h"
#include "core/ray.h"
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

	virtual bool base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info);

	virtual bool hit(const ray& ray, float t_min, float t_max, hit_info& info) = 0;

	void update();

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

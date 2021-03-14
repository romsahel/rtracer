#pragma once
#include "object_store.h"
#include "serializable.h"
#include "texture.h"
#include "core/color.h"

struct hit_info;
class ray;

class material : public serializable
{
public:
	explicit material(const char* name)
		: name(name)
	{
	}

	virtual bool scatter(const ray& raycast, const hit_info& rec, color& attenuation, ray& scattered) const = 0;

	virtual color emitted(const vec2& coordinates, const point3& point)
	{
		return color(emission_strength * emission->value_at(coordinates, point));
	}

	std::shared_ptr<serializable_node_base> serialize() override
	{
		return std::make_shared<serializable_node_base>(
			"Emission", serializable_list{
				emission->serialize("Emission"),
				std::make_shared<serializable_node<float>>("Strength", &emission_strength)
			}
		);
	}

	const char* name;
	texture* emission = solid_color::black();
	float emission_strength = 0.0;
};


inline object_store<material>& material_store()
{
	static auto instance = object_store<material>();
	return instance;
}

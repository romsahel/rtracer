#pragma once
#include <glm/gtx/matrix_decompose.hpp>

#include "serializable_node.h"

struct serializable_node_base;
template <typename TValue>
struct serializable_node;

class serializable
{
public:
	virtual ~serializable() = default;

	virtual std::shared_ptr<serializable_node_base> serialize()
	{
		return nullptr;
	}

	std::shared_ptr<serializable_node_base> serialize(const std::string& name)
	{
		auto r = serialize();
		r->name = name;
		return r;
	}
};

class serializable_transform : public serializable_node_base
{
public:
	serializable_transform(glm::mat4& transform): serializable_node_base("Transform"), m_transform(transform)
	{
		static vec3 scale, skew;
		static vec4 perspective;
		static glm::quat orientation;
		decompose(m_transform, scale, orientation, translation, skew, perspective);
		euler = degrees(glm::eulerAngles(glm::conjugate(orientation)));

		s = std::make_shared<serializable_node_base>(
			"Transform", serializable_list{
				std::make_shared<serializable_node<point3>>("Position", &translation),
				std::make_shared<serializable_node<point3>>("Rotation", &euler)
			}
		);
	}

	bool visit(serializer* serializer) override
	{
		if (serializer->serialize_root(s.get()))
		{
			auto q = glm::quat(radians(euler));
			m_transform = translate(translation) * mat4_cast(q);
			return true;
		}
		return false;
	}

	std::shared_ptr<serializable_node_base> s;
	glm::mat4& m_transform;
	glm::vec3 euler{0.0f};
	vec3 translation{0.0f};
};

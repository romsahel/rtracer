#pragma once
#include <string>
#include <vector>

struct serializable_node_root;
template <typename TValue>
struct serializable_node;

class serializer
{
public:
	virtual bool serialize_root(serializable_node_root* root) = 0;
	virtual bool serialize(const std::string& name, double* value) = 0;
};


struct serializable_node_root
{
	serializable_node_root(const std::string& name, const std::initializer_list<serializable_node_root*>& children)
		: name(name),
		  children(children)
	{
	}

	virtual bool visit(serializer& serializer)
	{
		return false;
	}

public:
	std::string name;
public:
	std::vector<serializable_node_root*> children{};
};

template <typename TValue>
struct serializable_node : serializable_node_root
{
	serializable_node(const std::string& name, TValue* value,
	                  const std::initializer_list<serializable_node_root*>& children)
		: serializable_node_root(name, children)
		, value(value)
	{
	}

	bool visit(serializer& serializer) override
	{
		bool changed = false;
		serializer.serialize(name, value);
		for (serializable_node_root* child : children)
		{
			changed |= child->visit(serializer);
		}

		return changed;
	}

	TValue* value;
};
#pragma once
#include <string>
#include <vector>
#include "core/color.h"

class gui_image_view;
struct serializable_node_base;
template <typename TValue>
struct serializable_node;

class serializer
{
public:
	virtual ~serializer() = default;
	virtual bool serialize_root(serializable_node_base* root) = 0;
	virtual bool serialize(const std::string& name, float* value) = 0;
	virtual bool serialize(const std::string& name, vec3* value) = 0;
	virtual bool serialize(const std::string& name, vec2* value) = 0;
	virtual bool serialize(const std::string& name, color* value) = 0;
	virtual bool serialize(const std::string& name, gui_image_view* value) = 0;
};


using serializable_list = std::initializer_list<std::shared_ptr<serializable_node_base>>;

class serializable_change_listener
{
public:
	virtual ~serializable_change_listener() = default;

	virtual void on_change()
	{
	}
};


struct serializable_node_base
{
	serializable_node_base(const std::string& name, const serializable_list& children)
		: name(name),
		  children(children)
	{
	}

	serializable_node_base(const std::string& name)
		: name(name)
	{
	}

	virtual bool visit(serializer* serializer)
	{
		return serializer->serialize_root(this);
	}

	void notify_change()
	{
		for (auto && listener : change_listeners)
		{
			listener->on_change();
		}
	}

public:
	virtual ~serializable_node_base() = default;
	std::string name;
	std::vector<serializable_change_listener*> change_listeners;
public:
	std::vector<std::shared_ptr<serializable_node_base>> children{};
};

template <typename TValue>
struct serializable_node : serializable_node_base
{
	serializable_node(const std::string& name, TValue* value,
	                  const serializable_list& children)
		: serializable_node_base(name, children)
		  , value(value)
	{
	}

	serializable_node(const std::string& name, TValue* value)
		: serializable_node_base(name)
		  , value(value)
	{
	}
	template <class... Args>
	static std::shared_ptr<serializable_node<TValue>> make_node(const std::string& name, Args&&... args)
	{
		auto ptr = new TValue(std::forward<Args>(args)...);
		auto node = std::make_shared<serializable_node<TValue>>(name, ptr);
		node->manage = true;
		return node;
	}

	~serializable_node()
	{
		if (manage)
			free(value);
	}
	
	bool visit(serializer* serializer) override
	{
		bool changed = serializer->serialize(name, value);
		for (std::shared_ptr<serializable_node_base> child : children)
		{
			changed |= child->visit(serializer);
		}

		return changed;
	}

	TValue* value;
	bool manage = false;

};

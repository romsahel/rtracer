#pragma once

#include <vector>

#include "core/hittable.h"
#include "materials/material.h"

class world : public hittable
{
public:
	world() : hittable("World") {}
	
	~world()
	{
		for (hittable* obj : m_list)
		{
			delete obj;
		}

		for (::material* mat : m_materials)
		{
			delete mat;
		}
	}
	
	template<typename T, class... Args>
	T& add(Args&&... args)
	{
		auto* added = new T(std::forward<Args>(args)...);
		m_list.push_back(added);
		return *added;
	}

	template<typename T, class... Args>
	T& add_material(Args&&... args)
	{
		auto* added = new T(std::forward<Args>(args)...);
		m_materials.push_back(added);
		return *added;
	}

	void shallow_add(hittable* hittable)
	{
		m_list.push_back(hittable);
	}

	void shallow_add_material(::material* material)
	{
		m_materials.push_back(material);
	}

	void shallow_clear()
	{
		m_list.clear();
		m_materials.clear();
	}

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const override
	{
		bool has_hit = false;
		info.distance = t_max;
		for (const auto& hittable : m_list)
		{
			if (hittable->hit(ray, t_min, info.distance, info))
			{
				has_hit = true;
			}
		}

		return has_hit;
	}

	const std::vector<hittable*>& hittables() const
	{
		return m_list;
	}

private:
	std::vector<hittable*> m_list;
	std::vector<::material*> m_materials;
};

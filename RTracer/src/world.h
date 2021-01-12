﻿#pragma once

#include <vector>


#include "bvh.h"
#include "core/hittable.h"
#include "materials/material.h"

class world
{
public:
	world()
	{
	}

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

	template <typename T, class... Args>
	T& add(Args&&... args)
	{
		auto* added = new T(std::forward<Args>(args)...);
		m_list.push_back(added);
		return *added;
	}

	template <typename T, class... Args>
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

	bool hit(const ray& ray, double t_min, double t_max, hit_info& info) const
	{
		info.distance = t_max;

		if (use_bvh)
		{
			return m_bvh->hit(ray, t_min, t_max, info);
		}
		else
		{
			bool has_hit = false;
			for (const auto& hittable : m_list)
			{
				if (hittable->hit(ray, t_min, info.distance, info))
				{
					has_hit = true;
				}
			}

			return has_hit;
		}
	}

	const std::vector<hittable*>& hittables() const
	{
		return m_list;
	}

	const std::vector<material*>& materials() const
	{
		return m_materials;
	}

	void signal_scene_change()
	{
		m_bvh = new bvh_node(m_list, 0, m_list.size());
	}

	bool use_bvh{true};

private:
	hittable* m_bvh;
	std::vector<hittable*> m_list;
	std::vector<::material*> m_materials;
};

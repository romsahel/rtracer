#pragma once

#include <functional>
#include <iostream>

#include "core/aabb.h"
#include "core/random.h"

#include "hittable.h"

inline bool box_compare(const hittable* a, const hittable* b, int axis)
{
	return a->bbox.minimum[axis] < b->bbox.maximum[axis] && a->bbox.minimum[axis] < b->bbox.minimum[axis];
}

bool box_x_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 0); }
bool box_y_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 1); }
bool box_z_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 2); }

class bvh_empty_leaf : public hittable
{
protected:
	void internal_update() override
	{
	}

	explicit bvh_empty_leaf()
		: hittable("")
	{
	}

public:
	bool base_hit(const ray&, float, float, hit_info&) override
	{
		return false;
	}

	bool hit(const ray&, float, float, hit_info&) override
	{
		return false;
	}

	static bvh_empty_leaf* instance()
	{
		static bvh_empty_leaf node;
		return &node;
	}
};

/// <summary>
/// bounding volume hierarchy:
/// Used to speed up rendering
/// We recursively divide the objects into a hierarchy of group objects - intermediary nodes are bvh_nodes while leaves are single hittable objects
/// For each bvh_node we compute its bounding box:
/// This allows us to raycast only a sub-group of the objects
/// </summary>
class bvh_node final : public hittable
{
public:
	bvh_node(std::vector<hittable*> objects, size_t start, size_t end) : hittable("BVH Node")
	{
		// choose a comparison axis
		const int axis = random::get<int>(0, 2);

		// if the list only has two elements, put one in each subtree and stop there
		const size_t count = end - start;
		if (count == 1)
		{
			m_left = objects[start];
			m_right = bvh_empty_leaf::instance();
		}
		else if (count == 2)
		{
			if (box_compare(objects[start], objects[start + 1], axis))
			{
				m_left = objects[start];
				m_right = objects[start + 1];
			}
			else
			{
				m_left = objects[start + 1];
				m_right = objects[start];
			}
		}
		else // sort the primitives along the axis
		{
			std::sort(objects.begin() + start, objects.begin() + end, [axis](const hittable* a, const hittable* b)
			{
				return box_compare(a, b, axis);
			});

			if (count == 3)
			{
				// put the first two in m_left and the last one as a leaf
				// this avoid an uncessary bvh_node
				m_left = new bvh_node(objects, start, start + 2);
				m_right = objects[end - 1];
			}
			else
			{
				// put half in each subtree
				const size_t half = start + count / 2;
				m_left = new bvh_node(objects, start, half);
				m_right = new bvh_node(objects, half, end);
			}
		}

		bbox = aabb::surrounding(m_left->bbox, m_right->bbox);
	}

	void internal_update() override
	{
	}

	bool base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info) override
	{
		if (bbox.hit(base_ray, t_min, t_max))
		{
			const bool hit_m_left = m_left->base_hit(base_ray, t_min, t_max, info);
			const bool hit_m_right = m_right->base_hit(base_ray, t_min, info.distance, info);
			return hit_m_left || hit_m_right;
		}
		return false;
	}

	bool hit(const ray&, float, float, hit_info&) override
	{
		throw new std::logic_error("Not Implemented");
	}

	~bvh_node() override
	{
		if (dynamic_cast<const bvh_node*>(m_left) != nullptr)
		{
			delete m_left;
		}

		if (dynamic_cast<const bvh_node*>(m_right) != nullptr)
		{
			delete m_right;
		}
	}

private:
	hittable* m_left;
	hittable* m_right;
};

#pragma once

#include <functional>
#include <iostream>

#include "core/aabb.h"
#include "core/hittable.h"

inline bool box_compare(const hittable* a, const hittable* b, int axis)
{
	const aabb& box_a = a->bbox;
	const aabb& box_b = b->bbox;
	return box_a.minimum[axis] < box_b.maximum[axis] && box_a.minimum[axis] < box_b.minimum[axis];
}

bool box_x_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 0); }
bool box_y_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 1); }
bool box_z_compare(const hittable* a, const hittable* b) { return box_compare(a, b, 2); }

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
		int axis = random::get<int>(0, 2);

		// if the list only has two elements, put one in each subtree and stop there
		size_t count = end - start;
		if (count == 1)
		{
			left = objects[start];
			right = objects[start];
		}
		else if (count == 2)
		{
			left = objects[start];
			right = objects[start + 1];
			if (!box_compare(left, right, axis))
				std::swap(left, right);
		}
		else // sort the primitives along the axis
		{
			using comparer_t = bool(const hittable*, const hittable*);
			static std::vector<comparer_t*> box_comparers = {box_x_compare, box_y_compare, box_z_compare};
			std::sort(objects.begin(), objects.end(), box_x_compare);
			
			if (count == 3)
			{
				// put the first two in left and the last one as a leaf
				// this avoid an uncessary bvh_node
				left = new bvh_node(objects, start, start + 2);
				right = objects[end - 1];
			}
			else
			{
				// put half in each subtree
				size_t half = start + count / 2;
				left = new bvh_node(objects, start, half);
				right = new bvh_node(objects, half, end);
			}
		}

		bbox = aabb::surrounding(left->bbox, right->bbox);
	}

	bool base_hit(const ray& base_ray, float t_min, float t_max, hit_info& info) override
	{
		return hit(base_ray, t_min, t_max, info);
	}
	
	bool hit(const ray& ray, float t_min, float t_max, hit_info& info) override
	{
		if (bbox.hit(ray, t_min, t_max))
		{
			const bool hit_left = left->base_hit(ray, t_min, t_max, info);
			const bool hit_right = right->base_hit(ray, t_min, info.distance, info);
			return hit_left || hit_right;
		}
		return false;
	}

	~bvh_node() override
	{
		if (dynamic_cast<const bvh_node*>(left) != nullptr)
			delete left;

		if (dynamic_cast<const bvh_node*>(right) != nullptr)
			delete right;
	}

	hittable* left;
	hittable* right;
};

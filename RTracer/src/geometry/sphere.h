#pragma once

#include "core/aabb.h"
#include "core/hittable.h"

/// <summary>
/// represent a sphere
/// defined by a point in world space and a radius
/// </summary>
class sphere : public hittable
{
public:
	sphere(const char* name, const point3& center, float radius)
		: hittable(name), radius(radius)
	{
		if (!USE_TRANSFORM)
			this->center = center;
		transform = translate(transform, center);
		inv_transform = inverse(transform);
	}

	aabb bbox;
	point3 center;

	void update() override
	{
		const vec3 size(radius);
		bbox = aabb(point3(-size), point3(size));
		if (USE_TRANSFORM)
			bbox.transform(transform);
	}
	

	bool hit(const ray& r, float t_min, float t_max, hit_info& rec) override
	{
		    vec3 oc = r.origin - center;
    auto a = length2(r.direction);
    auto half_b = dot(oc, r.direction);
    auto c = length2(oc) - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.distance = root;
    rec.point = r.at(rec.distance);
    vec3 outward_normal = (rec.point - center) / radius;
    rec.set_face_normal(r, outward_normal);
    set_uv_at(outward_normal, rec.uv_coordinates);
    rec.material = material;
	return true;
		
		//const vec3 oc = ray.origin - center;
		//const float half_b = dot(ray.direction, oc);
		//const float c = length2(oc) - radius * radius;
		//const float squared_discriminant = half_b * half_b - c;
		//bool is_hit = squared_discriminant >= 0;
		//if (is_hit)
		//{
		//	const float discriminant = std::sqrt(squared_discriminant);
		//	float root = (-half_b - discriminant);
		//	is_hit = root >= t_min && root <= t_max;
		//	if (!is_hit)
		//	{
		//		root = (-half_b + discriminant);
		//		is_hit = root >= t_min && root <= t_max;
		//	}

		//	if (is_hit)
		//	{
		//		info.distance = root;
		//		info.point = ray.at(info.distance);
		//		const direction3 outward_normal = direction3((info.point - center) / radius);
		//		info.set_face_normal(ray, outward_normal);
		//		info.material = material;
		//		set_uv_at(outward_normal, info.uv_coordinates);
		//		info.object = this;
		//	}
		//}

		//return is_hit;
	}

	bool bounding_box(aabb& output_aabb) const override
	{
		output_aabb = bbox;
		return true;
	}

	float radius;

private:
	static void set_uv_at(const vec3& p, vec2& uv_coordinates)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		static const float inv_2pi = 1.0f / (2 * constants::pi);
		const float theta = acos(-p.y);
		const float phi = atan2(-p.z, p.x) + constants::pi;
		uv_coordinates.x = phi * inv_2pi;
		uv_coordinates.y = theta * constants::inv_pi;
	}
};

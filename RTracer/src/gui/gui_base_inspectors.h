#pragma once
#include "camera.h"
#include "gui_utils.h"
#include "geometry/plane.h"
#include "geometry/sphere.h"
#include "materials/dieletric_material.h"
#include "materials/lambertian_material.h"
#include "materials/metal_material.h"

inline bool draw_camera_inspector(camera& camera)
{
	bool changed = false;
	if (ImGui::CollapsingHeader("Camera"))
	{
		changed |= gui::draw_vec3("Origin", camera.origin);
		changed |= gui::draw_vec3("Target", camera.target);
		changed |= gui::draw_double("Aperture", camera.aperture);
		changed |= gui::draw_double("Focus distance", camera.focus_distance);
		changed |= gui::draw_double("Vertical fov", camera.vertical_fov);

		if (changed)
		{
			camera.update();
		}
	}

	return changed;
}

inline bool draw_material_inspector(lambertian_material* material)
{
	bool changed = false;
	changed |= gui::draw_color("Albedo", material->albedo);
	return changed;
}

inline bool draw_material_inspector(metal_material* material)
{
	bool changed = false;
	changed |= gui::draw_color("Albedo", material->albedo);
	changed |= gui::draw_double("Roughness", material->roughness);
	return changed;
}

inline bool draw_material_inspector(dielectric_material* material)
{
	bool changed = false;
	double ior = material->index_of_refraction();
	if ((changed |= gui::draw_double("Index of refraction", ior)))
		material->index_of_refraction(ior);
	return changed;
}

inline bool draw_material_inspector(material* material)
{
	if (ImGui::CollapsingHeader(material->name))
	{
		if (auto cast = dynamic_cast<lambertian_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
		if (auto cast = dynamic_cast<metal_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
		if (auto cast = dynamic_cast<dielectric_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
	}

	return false;
}

inline bool draw_hittable_inspector(sphere* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Center", hittable->center);
	changed |= gui::draw_double("Radius", hittable->radius);
	changed |= draw_material_inspector(hittable->material);
	return changed;
}

inline bool draw_hittable_inspector(plane* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Point", hittable->point);
	changed |= gui::draw_vec3("Normal", hittable->normal);
	changed |= draw_material_inspector(hittable->material);
	return changed;
}

inline bool draw_hittable_inspector(hittable* hittable)
{
	if (ImGui::CollapsingHeader(hittable->name))
	{
		if (auto cast = dynamic_cast<sphere*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
		if (auto cast = dynamic_cast<plane*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
	}

	return false;
}

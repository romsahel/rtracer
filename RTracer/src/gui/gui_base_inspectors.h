#pragma once
#include "camera.h"
#include "gui_utils.h"
#include "world.h"
#include "geometry/plane.h"
#include "geometry/sphere.h"
#include "materials/dieletric_material.h"
#include "materials/lambertian_material.h"
#include "materials/metal_material.h"

inline bool draw_camera_inspector(camera& camera)
{
	bool changed = false;
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
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
	changed |= gui::draw_double("Roughness", material->roughness, 0.05f, 0.0f, 1.0f);
	return changed;
}

inline bool draw_material_inspector(dielectric_material* material)
{
	bool changed = false;
	double ior = material->index_of_refraction();
	if ((changed |= gui::draw_double("Index of refraction", ior, 0.01f, 1.0f)))
		material->index_of_refraction(ior);
	return changed;
}

inline bool draw_material_inspector(material* material)
{
	if (ImGui::CollapsingHeader(material->name, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (const auto cast = dynamic_cast<lambertian_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
		if (const auto cast = dynamic_cast<metal_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
		if (const auto cast = dynamic_cast<dielectric_material*>(material); cast != nullptr)
			return draw_material_inspector(cast);
	}

	return false;
}

inline bool draw_hittable_inspector(sphere* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Center", hittable->center);
	changed |= gui::draw_double("Radius", hittable->radius, 0.1f, 0.01f);
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
	if (ImGui::CollapsingHeader(hittable->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (auto cast = dynamic_cast<sphere*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
		if (auto cast = dynamic_cast<plane*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
	}

	return false;
}

inline bool draw_inspector(camera& camera, const world& world, void** selection)
{
	if (*selection == nullptr) return false;

	if (*selection == &camera)
	{
		return draw_camera_inspector(camera);
	}
	else
	{
		return draw_hittable_inspector(static_cast<hittable*>(*selection));
	}
}

template <typename T>
inline void add_to_hierarchy(T* obj, const char* label, void** selection)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_FramePadding;
	node_flags |= ImGuiTreeNodeFlags_SpanFullWidth;
	node_flags |= ImGuiTreeNodeFlags_Leaf;
	node_flags |= static_cast<int>(*selection == obj) * ImGuiTreeNodeFlags_Selected;

	ImGui::PushID(obj);
	ImGui::TreeNodeEx(label, node_flags);
	ImGui::PopID();

	if (ImGui::IsItemClicked())
	{
		*selection = *selection == obj ? nullptr : static_cast<void*>(obj);
	}
	ImGui::TreePop();
}

inline void draw_hierarchy(camera& camera, const world& world, void** selection)
{
	add_to_hierarchy(&camera, "Camera", selection);

	for (hittable* obj : world.hittables())
	{
		add_to_hierarchy(obj, obj->name.c_str(), selection);
	}
}

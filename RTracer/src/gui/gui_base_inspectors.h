﻿#pragma once
#include "camera.h"
#include "gui_image.h"
#include "gui_utils.h"
#include "world.h"
#include "core/rotate_y.h"
#include "geometry/box.h"
#include "geometry/sphere.h"
#include "materials/dieletric_material.h"
#include "materials/image_texture.h"
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

inline bool draw_texture_inspector(const char* name, texture* texture)
{
	bool changed = false;
	if (auto* const color = dynamic_cast<solid_color*>(texture); color != nullptr)
	{
		changed |= gui::draw_color(name, color->color_value);
	}
	else if (auto* const checker = dynamic_cast<checker_texture*>(texture); checker != nullptr)
	{
		ImGui::Text(name);
		changed |= draw_texture_inspector("Odd", checker->odd);
		changed |= draw_texture_inspector("Even", checker->even);
	}
	else if (auto* const image = dynamic_cast<image_texture*>(texture); image != nullptr)
	{
		static gui_image img(false);
		static image_texture* previous = nullptr;
		static float ratio = 0.0f;
		if (previous != image)
		{
			img.update(image->width, image->height, image->data);
			ratio = static_cast<float>(image->height) / static_cast<float>(image->width);
		}

		ImGui::Text("%s (%dx%d)", name, image->width, image->height);

		const float width = ImGui::GetWindowSize().x - 5.0f;
		const ImVec2 size = ImVec2(width, width * ratio);
		ImGui::Image(img.texture_id(), size);
	}
	return changed;
}

inline bool draw_material_inspector(lambertian_material* material)
{
	bool changed = false;
	ImGui::Text("Albedo");
	changed |= draw_texture_inspector("Texture", material->albedo);
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
	if ((changed |= gui::draw_double("Index of refraction", ior, 0.01f, 1.0f)), changed)
		material->index_of_refraction(ior);
	return changed;
}

inline bool draw_material_inspector(material* material)
{
	bool changed = false;
	if (ImGui::CollapsingHeader(material->name, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (auto* const lambertian = dynamic_cast<lambertian_material*>(material); lambertian != nullptr)
			changed |= draw_material_inspector(lambertian);
		else if (auto* const metal = dynamic_cast<metal_material*>(material); metal != nullptr)
			changed |= draw_material_inspector(metal);
		else if (auto* const dielectric = dynamic_cast<dielectric_material*>(material); dielectric != nullptr)
			changed |= draw_material_inspector(dielectric);

		ImGui::Text("Emission");
		changed |= draw_texture_inspector("Texture", material->emission);
		changed |= gui::draw_double("Strength", material->emission_strength);
	}

	return changed;
}

inline material** get_selection_material(void* selection)
{
	auto hittable = static_cast<::hittable*>(selection);
	if (auto* cast = dynamic_cast<transform_root*>(hittable); cast != nullptr)
		return &cast->object->material;
	return &hittable->material;
}

bool draw_hittable_inspector(hittable* hittable, bool with_header);

inline bool draw_hittable_inspector(sphere* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Center", hittable->center);
	changed |= gui::draw_double("Radius", hittable->radius, 0.1f, 0.01f);
	return changed;
}

inline bool draw_hittable_inspector(rectangle* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Offset", hittable->position);
	changed |= gui::draw_double("Width", hittable->width);
	changed |= gui::draw_double("Height", hittable->height);

	if (ImGui::Button("Change orientation"))
	{
		hittable->right_axis((hittable->right_axis() + 1) % 3);
		changed = true;
	}
	ImGui::SameLine();
	bool flip = hittable->flip_normal < 0.0;
	if (ImGui::Checkbox("Flip normal", &flip))
	{
		hittable->flip_normal = flip ? -1.0 : 1.0;
		changed = true;
	}

	return changed;
}

inline bool draw_hittable_inspector(box* hittable)
{
	bool changed = false;
	changed |= gui::draw_vec3("Width", hittable->size);

	return changed;
}

template <int axis>
inline bool draw_hittable_inspector(rotate_base<axis>* hittable, const char* label)
{
	bool changed = false;
	changed |= gui::draw_double(label, hittable->angle);
	return changed;
}

inline bool draw_hittable_inspector(transform_root* hittable)
{
	bool changed = false;
	if (auto* translation = dynamic_cast<translator*>(hittable); translation != nullptr)
	{
		changed |= gui::draw_vec3("Position", translation->position);
	}
	else
	{
		if (auto* cast = dynamic_cast<rotate_x*>(hittable); cast != nullptr)
			changed |= draw_hittable_inspector(cast, "x-rotation");
		if (auto* cast = dynamic_cast<rotate_y*>(hittable); cast != nullptr)
			changed |= draw_hittable_inspector(cast, "y-rotation");
		if (auto* cast = dynamic_cast<rotate_z*>(hittable); cast != nullptr)
			changed |= draw_hittable_inspector(cast, "z-rotation");
	}

	changed |= draw_hittable_inspector(hittable->object, false);

	return changed;
}

inline bool draw_hittable_inspector(hittable* hittable, bool with_header)
{
	if (!with_header || ImGui::CollapsingHeader(hittable->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (auto* cast = dynamic_cast<sphere*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
		if (auto* cast = dynamic_cast<rectangle*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
		if (auto* cast = dynamic_cast<box*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
		if (auto* cast = dynamic_cast<transform_root*>(hittable); cast != nullptr)
			return draw_hittable_inspector(cast);
	}

	return false;
}

inline bool draw_inspector(camera& camera, void** selection)
{
	if (*selection == nullptr) return false;

	if (*selection == &camera)
	{
		return draw_camera_inspector(camera);
	}
	else
	{
		auto hittable = static_cast<::hittable*>(*selection);
		bool changed = draw_hittable_inspector(hittable, true);

		if (changed)
		{
			hittable->update();
		}

		ImGui::Separator();
		aabb bbox;
		hittable->bounding_box(bbox);
		gui::display_vec3("Minimum", bbox.minimum);
		gui::display_vec3("Maximum", bbox.maximum);
		gui::display_vec3("Size", bbox.size());

		return changed;
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

inline void draw_hierarchy(camera& camera, world& world, void** selection)
{
	add_to_hierarchy(&camera, "Camera", selection);

	const std::vector<hittable*>& objects = world.hittables();
	auto it = objects.begin();
	auto to_remove = objects.end();
	for (hittable* obj : objects)
	{
		add_to_hierarchy(obj, obj->name.c_str(), selection);
		if (ImGui::BeginPopupContextItem())
		{
			static char buf[256];
			sprintf_s(buf, "Remove %s", obj->name.c_str());
			if (ImGui::MenuItem(buf))
			{
				to_remove = it;
			}
			ImGui::EndPopup();
		}
		++it;
	}

	if (to_remove != objects.end())
	{
		if (*to_remove == *selection)
		{
			*selection = nullptr;
		}
		world.remove(to_remove);
		world.signal_scene_change();
	}
}

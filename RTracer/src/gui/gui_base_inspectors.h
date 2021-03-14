#pragma once
#include "camera.h"
#include "gui_utils.h"
#include "world.h"
#include "materials/image_texture.h"
#include "inspector_serializer.h"

inline inspector_serializer inspector;
inline std::shared_ptr<serializable_node_base> selected_serializable;

inline bool draw_inspector(camera& camera, serializable** selection)
{
	if (*selection == nullptr) return false;


	static serializable* previous_selection = nullptr;
	if (previous_selection != *selection)
	{
		selected_serializable = (*selection)->serialize();
		previous_selection = (*selection);
	}

	bool changed = inspector.serialize_root(selected_serializable.get());

	if ((*selection) == &camera)
	{
		if (changed)
			camera.update();
	}
	else
	{
		auto hittable = dynamic_cast<::hittable*>(*selection);
		if (changed)
		{
			hittable->update();
		}

		ImGui::Separator();
		gui::display_vec3("Minimum", hittable->bbox.minimum);
		gui::display_vec3("Maximum", hittable->bbox.maximum);
		gui::display_vec3("Size", hittable->bbox.size());
	}

	return changed;
}

template <typename T>
inline void add_to_hierarchy(T* obj, const char* label, serializable** selection)
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
		*selection = *selection == obj ? nullptr : static_cast<serializable*>(obj);
	}
	ImGui::TreePop();
}

inline void draw_hierarchy(camera& camera, world& world, serializable** selection)
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

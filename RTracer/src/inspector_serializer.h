#pragma once
#include "serializable_node.h"
#include "gui/gui_image.h"
#include "gui/gui_utils.h"

class inspector_serializer : public serializer
{
	int m_indent;
public:
	bool serialize_root(serializable_node_base* root) override
	{
		ImGui::Indent(2.0f);
		m_indent++;
		bool changed = false;
		if (ImGui::CollapsingHeader(root->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (std::shared_ptr<serializable_node_base> child : root->children)
			{
				changed |= child->visit(this);
			}
		}

		if (changed)
		{
			root->notify_change();
		}
		m_indent--;
		ImGui::Unindent(2.0f);
		return changed;
	}

	bool serialize(const std::string& name, float* value) override
	{
		return gui::draw_float(name.c_str(), *value);
	}

	bool serialize(const std::string& name, vec3* value) override
	{
		return gui::draw_vec3(name.c_str(), *value);
	}

	bool serialize(const std::string& name, vec2* value) override
	{
		return gui::draw_vec2(name.c_str(), *value);
	}

	bool serialize(const std::string& name, color* value) override
	{
		return gui::draw_color(name.c_str(), *value);
	}

	bool serialize(const std::string& name, gui_image_view* image) override
	{
		static gui_image img(false);
		img.update(image->width, image->height, image->data);
		float ratio = static_cast<float>(image->height) / static_cast<float>(image->width);

		ImGui::Text("%s (%dx%d)", name.c_str(), image->width, image->height);

		const float width = ImGui::GetWindowSize().x - 5.0f;
		const ImVec2 size = ImVec2(width, width * ratio);
		ImGui::Image(img.texture_id(), size);
		return false;
	}
};

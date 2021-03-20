#pragma once

#include "core/vec3.h"
#include "gui_initializer.h"
#include "core/color.h"

namespace gui
{
	inline void display_vec3(const char* label, const vec3& value)
	{
		ImGui::Text("%s: (%f, %f, %f)", label, value.x, value.y, value.z);
	}

	inline bool draw_vec3(const char* label, vec3& value, float speed = 0.1f)
	{
		ImGui::PushID(&value);
		const bool changed = ImGui::DragFloat3(label, glm::value_ptr(value), speed);
		ImGui::PopID();

		return changed;
	}

	inline bool draw_vec2(const char* label, vec2& value, float speed = 0.1f)
	{
		ImGui::PushID(&value);
		const bool changed = ImGui::DragFloat2(label, value_ptr(value), speed);
		ImGui::PopID();

		return changed;
	}

	inline bool draw_color(const char* label, color& value)
	{
		ImGui::PushID(&value);
		const bool changed = ImGui::ColorEdit3(label, glm::value_ptr(static_cast<glm::vec3&>(value)));
		ImGui::PopID();
		return changed;
	}

	inline bool draw_float(const char* label, float& value, float speed = 0.1f, float v_min = 0.0f, float v_max = 0.0f)
	{
		ImGui::PushID(&value);
		const bool changed = ImGui::DragFloat(label, &value, speed, v_min, v_max);
		ImGui::PopID();
		return changed;
	}

	inline void push_disabled(bool disabled)
	{
		if (disabled)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
	}

	inline void pop_disabled(bool disabled)
	{
		if (disabled)
		{
			ImGui::PopStyleVar();
		}
	}

	inline void help_marker(const char* description)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(450.0f);
			ImGui::TextUnformatted(description);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}

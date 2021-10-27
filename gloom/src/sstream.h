#pragma once

#include <imgui_internal.h>

#include <vector>
#include <string>
#include "core/gui_initializer.h"


class sstream
{
	std::vector<std::string> m_labels;
	std::vector<void*> m_scalars;
	ImGuiDataType m_scalarType = ImGuiDataType_COUNT;

public:
	struct endl_s
	{
		float v_speed = 1.0f;
		float v_min = 0.0f;
		float v_max = 0.0f;
	};

	const inline static endl_s endl = endl_s{};

	void clear()
	{
		m_labels.clear();
		m_scalars.clear();
		m_scalarType = ImGuiDataType_COUNT;
	}

	sstream& operator<<(const char* str)
	{
		m_labels.emplace_back(str);
		return *this;
	}

	sstream& operator<<(float& value)
	{
		m_scalarType = ImGuiDataType_Float;
		m_scalars.emplace_back(&value);
		return *this;
	}

	bool operator<<(const endl_s& value)
	{
		bool has_changed = false;

		const char* label = !m_labels.empty() ? m_labels[0].c_str() : "Value";
		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGui::BeginGroup();
		ImGui::PushID(label);
		const auto components = static_cast<int>(m_scalars.size());
		ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());
		for (int i = 0; i < components; i++)
		{
			ImGui::PushID(i);
			if (i > 0)
				ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			has_changed |= ImGui::DragScalar("", m_scalarType, m_scalars[i], value.v_speed, &value.v_min,
			                                 &value.v_max);
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		ImGui::PopID();

		const char* label_end = ImGui::FindRenderedTextEnd(label);
		if (label != label_end)
		{
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			ImGui::Text(label, label_end);
		}

    ImGui::EndGroup();

		clear();
		return has_changed;
	}
};

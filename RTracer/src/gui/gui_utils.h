#pragma once

#include "gui_initializer.h"
#include "core/color.h"
#include "core/point3.h"
#include "core/vec3.h"

namespace gui
{
	void start_frame()
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		enable_dockspace();
	}

	void end_frame()
	{
		static ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	inline float* vec3_to_temporary_float3(vec3 value)
	{
		static float xyz[3];
		xyz[0] = static_cast<float>(value.x);
		xyz[1] = static_cast<float>(value.y);
		xyz[2] = static_cast<float>(value.z);
		return xyz;
	}

	inline vec3 float3_to_vec3(float* value)
	{
		return vec3(static_cast<float>(value[0]), static_cast<float>(value[1]), static_cast<float>(value[2]));
	}

	void display_vec3(const char* label, const vec3& value)
	{
		ImGui::Text("%s: (%f, %f, %f)", label, value.x, value.y, value.z);
	}

	bool draw_vec3(const char* label, vec3& value, float speed = 0.1f)
	{
		ImGui::PushID(&value);
		//float* tmp = gui::vec3_to_temporary_float3(value);
		const bool changed = ImGui::DragFloat3(label, glm::value_ptr(value), speed);
		ImGui::PopID();

		//if (changed)
		//	value = point3(gui::float3_to_vec3(tmp));
		return changed;
	}

	bool draw_color(const char* label, color& value)
	{
		ImGui::PushID(&value);
		float* tmp = gui::vec3_to_temporary_float3(value);
		const bool changed = ImGui::ColorEdit3(label, tmp);
		ImGui::PopID();

		if (changed)
			value = color(gui::float3_to_vec3(tmp));
		return changed;
	}

	bool draw_double(const char* label, float& value, float speed = 0.1f, float v_min = 0.0f, float v_max = 0.0f)
	{
		auto tmp = static_cast<float>(value);
		ImGui::PushID(&value);
		const bool changed = ImGui::DragFloat(label, &tmp, speed, v_min, v_max);
		ImGui::PopID();

		if (changed)
			value = static_cast<float>(tmp);
		return changed;
	}

	bool draw_doubles(const char* label, float& value1, float& value2, float speed = 0.1f, float v_min = 0.0f,
	                  float v_max = 0.0f)
	{
		static float values[3];
		values[0] = static_cast<float>(value1);
		values[1] = static_cast<float>(value2);

		ImGui::PushID(&value1);
		const bool changed = ImGui::DragFloat2(label, values, speed, v_min, v_max);
		ImGui::PopID();

		if (changed)
		{
			value1 = static_cast<float>(values[0]);
			value2 = static_cast<float>(values[1]);
		}

		return changed;
	}

	void push_disabled(bool disabled)
	{
		if (disabled)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
	}

	void pop_disabled(bool disabled)
	{
		if (disabled)
		{
			ImGui::PopStyleVar();
		}
	}

	void help_marker(const char* description)
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

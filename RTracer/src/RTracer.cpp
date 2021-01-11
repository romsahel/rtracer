#include <execution>
#include <chrono>

#include "core/direction3.h"
#include "core/point3.h"
#include "core/color.h"

#include "materials/lambertian_material.h"
#include "materials/metal_material.h"

#include "geometry/plane.h"
#include "geometry/sphere.h"

#include "camera.h"
#include "gui/gui_base_inspectors.h"
#include "gui/gui_utils.h"
#include "gui/gui_image.h"
#include "gui/selection_overlay.h"
#include "raytrace_renderer.h"
#include "world.h"
#include "materials/dieletric_material.h"

int main()
{
	// camera settings
	camera camera(16.0 / 9.0);

	// output settings
	const int image_width = 512;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());

	world world;

	material& selection_material = world.add_material<lambertian_material>("Selection material", color(1.0, 0.0, 0.0));
	material& glassMat = world.add_material<dielectric_material>("Glass", 1.5);
	material& shinyMetal = world.add_material<metal_material>("Shiny metal", color(0.8, 0.8, 0.8), 0.05);
	material& roughMetal = world.add_material<metal_material>("Rough metal", color(0.8, 0.6, 0.2), 0.6);

	//auto& ground = world.add<sphere>("Ground", point3(0.0, -1.25 - 100, 0.0), 100);
	auto& ground = world.add<plane>("Ground", point3(0.0, -1.25, 0.0), direction3(0.0, 1.0, 0.0));
	auto& leftSphere = world.add<sphere>("Left sphere", point3(-2.5, 0, 3.5), 1.25);
	auto& rightSphere = world.add<sphere>("Right sphere", point3(1.5, 0.5, 3), 1.25);
	auto& smallSphere = world.add<sphere>("Small sphere", point3(0, 0, 0), 0.3);

	smallSphere.material = &glassMat;
	rightSphere.material = &shinyMetal;
	leftSphere.material = &roughMetal;

	gui::initialize_opengl();

	raytrace_renderer raytrace_renderer{image_width, image_height};
	selection_overlay selection_overlay{raytrace_renderer.current_render};

	void* selection = nullptr;
	gui_image render_image{false};
	bool pause_rendering = false;
	long long last_render_time = 0;
	int max_render_iteration = 1000;

	while (!glfwWindowShouldClose(gui::window))
	{
		bool is_rendering = !pause_rendering && raytrace_renderer.current_render.iteration < max_render_iteration;
		gui::start_frame();

		ImGui::SetNextWindowDockID(0, ImGuiCond_FirstUseEver);

		bool scene_changed = false;
		if (ImGui::Begin("Render"))
		{
			ImGui::Text("Render: %d / %d (%llums)",
			            static_cast<int>(raytrace_renderer.current_render.iteration),
			            max_render_iteration, last_render_time);
			ImGui::SameLine();
			if (pause_rendering ? ImGui::Button("Resume rendering") : ImGui::Button("Pause rendering"))
			{
				pause_rendering = !pause_rendering;
			}

			scene_changed |= ImGui::DragInt("Max render iteration", &max_render_iteration);
			scene_changed |= ImGui::DragInt("Max depth", &raytrace_renderer.current_render.bounce_depth);
		}
		ImGui::End();

		if (ImGui::Begin("Hierarchy"))
		{
			void* previous_selection = selection;
			draw_hierarchy(camera, world, &selection);
			if (previous_selection != selection)
			{
				selection_overlay.signal_change();
			}
		}
		ImGui::End();

		if (ImGui::Begin("Inspector"))
		{
			scene_changed |= draw_inspector(camera, world, &selection);
		}
		ImGui::End();


		if (scene_changed)
		{
			raytrace_renderer.signal_scene_change();
			selection_overlay.signal_change();
			pause_rendering = false;
		}

		bool has_selection = selection != nullptr && selection != &camera;
		ImGui::SetNextWindowDockID(1, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Viewer"))
		{
			auto image_position = ImGui::GetCursorPos();
			const float width = ImGui::GetWindowSize().x;
			const ImVec2 size = ImVec2(width, width / static_cast<float>(camera.aspect_ratio()));
			ImGui::Image(render_image.texture_id(), size);
			if (has_selection)
			{
				selection_overlay.draw_overlay(image_position, size);
			}
		}
		ImGui::End();

		gui::end_frame();

		if (is_rendering)
		{
			auto start = std::chrono::high_resolution_clock::now();
			{
				raytrace_renderer.render(camera, world);
				render_image.update(image_width, image_height, raytrace_renderer.current_render.colors.data());
			}
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			last_render_time = duration.count();
		}

		if (has_selection && !selection_overlay.has_valid_render())
		{
			selection_overlay.render(selection, selection_material, camera, raytrace_renderer);
		}
	}

	gui::cleanup();

	return 0;
}

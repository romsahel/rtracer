#include <execution>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "core/stb_image_write.h"

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

	material& redMat = world.add_material<lambertian_material>("Red mat", color(1.0, 0.0, 0.0));
	material& glassMat = world.add_material<dielectric_material>("Glass", 1.5);
	material& shinyMetal = world.add_material<metal_material>("Shiny metal", color(0.8, 0.8, 0.8), 0.05);
	material& roughMetal = world.add_material<metal_material>("Rough metal", color(0.8, 0.6, 0.2), 0.6);

	auto& ground = world.add<plane>("Ground", point3(0.0, -1.25, 0.0), direction3(0.0, 1.0, 0.0));
	auto& leftSphere = world.add<sphere>("Left sphere", point3(-2.5, 0, 3.5), 1.25);
	auto& rightSphere = world.add<sphere>("Right sphere", point3(1.5, 0.5, 3), 1.25);
	auto& smallSphere = world.add<sphere>("Small sphere", point3(0, 0, 0), 0.3);

	smallSphere.material = &glassMat;
	rightSphere.material = &shinyMetal;
	leftSphere.material = &roughMetal;

	gui::initialize_opengl();

	raytrace_renderer raytrace_renderer{image_width, image_height};
	gui_image render_image;
	bool pause_rendering = false;
	long long last_render_time = 0;
	double render_iteration = 1.0;
	int max_render_iteration = 1000;
	while (!glfwWindowShouldClose(gui::window))
	{
		bool is_rendering = !pause_rendering && render_iteration < max_render_iteration;
		gui::start_frame();

		ImGui::SetNextWindowDockID(0, ImGuiCond_FirstUseEver);

		bool scene_changed = false;
		if (ImGui::Begin("Render"))
		{
			ImGui::Text("Render time: %llums", last_render_time);
			if (pause_rendering ? ImGui::Button("Resume rendering") : ImGui::Button("Pause rendering"))
			{
				pause_rendering = !pause_rendering;
			}

			scene_changed |= ImGui::DragInt("Max render iteration", &max_render_iteration);
			scene_changed |= ImGui::DragInt("Max depth", &raytrace_renderer.settings.max_depth);
		}

		if (ImGui::Begin("Hierarchy"))
		{
			scene_changed |= draw_camera_inspector(camera);

			for (hittable* hittable : world.hittables())
			{
				scene_changed |= draw_hittable_inspector(hittable);
			}
		}
		ImGui::End();


		if (scene_changed)
		{
			raytrace_renderer.signal_scene_change();
			pause_rendering = false;
			render_iteration = 1.0;
		}

		ImGui::SetNextWindowDockID(1, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Viewer"))
		{
			const auto width = ImGui::GetWindowSize().x;
			ImGui::Image(render_image.texture_id(), ImVec2(width, width / static_cast<float>(camera.aspect_ratio())));
		}
		ImGui::End();

		gui::end_frame();

		if (is_rendering)
		{
			auto start = std::chrono::high_resolution_clock::now();
			{
				raytrace_renderer.render(camera, world);
				render_image.update(image_width, image_height, raytrace_renderer.pixel_colors());
				render_iteration += 1.0;
			}
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			last_render_time = duration.count();
		}
	}
	gui::cleanup();

	return 0;
}

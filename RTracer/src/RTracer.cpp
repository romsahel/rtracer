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
	camera.origin = point3(0.5, 2.0, -1.0);
	camera.target = point3(3.0, 0, 2.5);
	camera.update();

	// output settings
	const int image_width = 512;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());

	world world;

	object_store<material> materials = material_store();

	material& selection_material = materials.add<lambertian_material>("Selection material", color(1.0, 0.0, 0.0));
	materials.add<dielectric_material>("Glass", 1.5);
	materials.add<metal_material>("Shiny chrome metal", color(0.8, 0.8, 0.8), 0.05);
	materials.add<metal_material>("Gold Rough metal", color(0.8, 0.6, 0.2), 0.6);
	materials.add<metal_material>("Shiny green metal", color(0.1, 0.8, 0.1), 0.05);
	materials.add<metal_material>("Rough yellow metal", color(0.0, 0.8, 0.8), 0.3);
	materials.add<lambertian_material>("Diffuse blue", color(0.2, 1.0, 0.0));
	materials.add<lambertian_material>("Diffuse mauve", color(0.9, 0.69, 1.0));
	materials.add<lambertian_material>("Checker", texture_store().add<checker_texture>(color::black(), color::white()));

	auto& ground = world.add<sphere>("Ground", point3(0.0, -1.25 - 100, 0.0), 100);
	//auto& ground = world.add<plane>("Ground", point3(0.0, -1.25, 0.0), direction3(0.0, 1.0, 0.0));

	char name[] = "Sphere 00";
	for (int x = 0; x < 10; x++)
	{
		for (int z = 0; z < 10; z++)
		{
			name[7] = static_cast<char>('0' + x);
			name[8] = static_cast<char>('0' + z);
			auto& obj = world.add<sphere>(
				name, point3(x * 2 + random::get<double>(-0.9, 0.9), 0.2, z * 2 + random::get<double>(-0.9, 0.9)),
				random::get<double>(0.2, 0.8));
			obj.material = materials[random::get<int>(1, static_cast<int>(materials.size()) - 1)];
		}
	}

	world.signal_scene_change();

	gui::initialize_opengl();

	raytrace_renderer raytrace_renderer{image_width, image_height};
	selection_overlay selection_overlay{raytrace_renderer.current_render};

	void* selection = nullptr;
	void* material_selection = nullptr;
	gui_image render_image{false};
	bool pause_rendering = false;
	long long last_render_time = 0;
	int max_render_iteration = 1000;
	bool mouse_over_hierarchy = false;

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
			ImGui::Checkbox("Use BVH", &world.use_bvh);
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

			mouse_over_hierarchy = ImGui::IsWindowHovered();
		}
		ImGui::End();

		if (ImGui::Begin("Materials"))
		{
			for (material* mat : materials)
			{
				add_to_hierarchy(mat, mat->name, &material_selection);
			}
		}
		ImGui::End();

		if (ImGui::Begin("Inspector"))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Change material to selection", 0, false, selection != nullptr && material_selection != nullptr))
				{
					static_cast<hittable*>(selection)->material = static_cast<material*>(material_selection);
					scene_changed = true;
				}
				ImGui::EndPopup();
			}
			scene_changed |= draw_inspector(camera, world, &selection);
		}
		ImGui::End();


		if (scene_changed)
		{
			raytrace_renderer.signal_scene_change();
			selection_overlay.signal_change();
			world.signal_scene_change();
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
			if (has_selection && mouse_over_hierarchy)
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

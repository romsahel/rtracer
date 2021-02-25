#include <execution>
#include <chrono>
#include <filesystem>


#include "core/direction3.h"
#include "core/point3.h"
#include "core/color.h"

#include "materials/lambertian_material.h"
#include "materials/metal_material.h"

#include "geometry/sphere.h"
#include "geometry/rectangle.h"

#include "camera.h"
#include "gui/gui_base_inspectors.h"
#include "gui/gui_utils.h"
#include "gui/gui_image.h"
#include "gui/selection_overlay.h"

#include "raytrace_renderer.h"
#include "world.h"
#include "core/matrix4x4.h"
#include "core/rotate_y.h"
#include "geometry/box.h"

#include "materials/dieletric_material.h"
#include "materials/image_texture.h"


camera make_cornell_scene(world& world, object_store<material>& materials)
{
	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 1.0f;

	auto& cornell_red = materials.add<lambertian_material>("Cornell red", color(.65f, .05f, .05f));
	auto& cornell_white = materials.add<lambertian_material>("Cornell white", color(.73f, .73f, .73f));
	auto& cornell_green = materials.add<lambertian_material>("Cornell green", color(.12f, .45f, .15f));

	float cornell_width = 30.0f;
	float cornell_height = 30.0f;
	auto& light = world.add<rectangle>("Light", point3(0.0f, cornell_height * 0.5f - 1 + cornell_height * 0.5f, 0.0f),
	                                   10.0f,
	                                   10.0f);
	light.right_axis(2);
	light.material = &light_material;
	light.update();

	auto& plane1 = world.add<rectangle>("Right wall", point3(-cornell_width * 0.5f, cornell_height * 0.5f, 0.0f),
	                                    cornell_height,
	                                    cornell_width);
	plane1.right_axis(1);
	plane1.material = &cornell_red;
	plane1.update();
	auto& plane2 = world.add<rectangle>("Left wall", point3(cornell_width * 0.5f, cornell_height * 0.5f, 0.0),
	                                    cornell_height, cornell_width);
	plane2.right_axis(1);
	plane2.material = &cornell_green;
	plane2.update();
	auto& plane3 = world.add<rectangle>("Top wall", point3(0.0, cornell_height * 0.5f + cornell_height * 0.5f, 0.0),
	                                    cornell_width, cornell_width);
	plane3.right_axis(2);
	plane3.material = &cornell_white;
	plane3.update();
	auto& plane4 = world.add<rectangle>("Bottom wall", point3(0.0, -cornell_height * 0.5f + cornell_height * 0.5f, 0.0),
	                                    cornell_width,
	                                    cornell_width);
	plane4.right_axis(2);
	plane4.material = &cornell_white;
	plane4.update();
	auto& plane5 = world.add<rectangle>("Back wall", point3(0.0, cornell_height * 0.5f, cornell_width * 0.5f),
	                                    cornell_width, cornell_height);
	plane5.right_axis(0);
	plane5.material = &cornell_white;
	plane5.update();

	auto backbox_size = vec3(cornell_height * 0.5f * 0.5f, cornell_height * 0.5f, cornell_height * 0.5f * 0.5f);
	auto& backbox = world.add<translator>(new rotate_y(new box("Back box", backbox_size), 15.0));
	backbox.position = point3(backbox_size.x() * 0.5f, 0.0, cornell_width * 0.5f * 0.75f - backbox_size.z() * 0.5f);
	backbox.position.x() += 1.0f;
	backbox.position.y() += backbox_size.y() * 0.5f;
	backbox.update();

	auto frontbox_size = vec3(cornell_height * 0.5f * 0.5f);
	auto& frontbox = world.add<translator>(new rotate_y(new box("Front box", frontbox_size), -18.0));
	frontbox.position.x() -= frontbox_size.x() * 0.5f + 1.0f;
	frontbox.position.y() += frontbox_size.y() * 0.5f;
	frontbox.update();

	::camera camera{4.0f / 3.0f};
	camera.origin = point3(0.0f, cornell_height * 0.5f, -45);
	camera.target = point3(0.0f, cornell_height * 0.5f * 0.85f, 0.0f);
	camera.vertical_fov = 40.0f;
	return camera;
}

camera make_sphere_scene(world& world, object_store<material>& materials)
{
	materials.add<dielectric_material>("Glass", 1.5f);
	materials.add<metal_material>("Shiny chrome metal", color(0.8f, 0.8f, 0.8f), 0.05f);
	materials.add<metal_material>("Gold Rough metal", color(0.8f, 0.6f, 0.2f), 0.6f);
	materials.add<metal_material>("Shiny green metal", color(0.1f, 0.8f, 0.1f), 0.05f);
	materials.add<metal_material>("Rough yellow metal", color(0.0f, 0.8f, 0.8f), 0.3f);
	materials.add<lambertian_material>("Diffuse blue", color(0.2f, 1.0f, 0.0f));
	materials.add<lambertian_material>("Diffuse mauve", color(0.9f, 0.69f, 1.0f));
	auto& checker_tex = texture_store().add<checker_texture>(color::black(), color::white());
	materials.add<lambertian_material>("Checker", checker_tex);
	materials.add<lambertian_material>("Earth", texture_store().add<image_texture>("earthmap.jpg"));

	world.add<sphere>("Ground", point3(0.0f, -1.25f - 100.0f, 0.0f), 100.0f);

	char name[] = "Sphere 00";
	for (int z = 0; z < 10; z++)
	{
		int x_span = 6;
		for (int x = 0; x < x_span; x++)
		{
			name[7] = static_cast<char>('0' + z);
			name[8] = static_cast<char>('0' + x);
			auto& obj = world.add<sphere>(
				name, point3((x - x_span * 0.5f), 0.2f, (z + 1.0f)),
				0.3f);
			obj.material = materials[random::get<size_t>(1, materials.size()) - 1];
		}
	}

	auto& wall_left = world.add<rectangle>("wall_left", point3(5.0f, 0.0f, 0.0f), 5.0f, 30.0f);
	wall_left.right_axis(1);
	wall_left.update();

	auto& wall_right = world.add<rectangle>("wall_right", point3(-5.0f, 0.0f, 0.0f), 5.0f, 30.0f);
	wall_right.right_axis(1);
	wall_right.update();

	auto& wall_top = world.add<rectangle>("wall_top", point3(0.0f, 6.0f, 0.0f), 30.0f, 30.0f);
	wall_top.right_axis(2);
	wall_top.update();


	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 1.0f;
	auto& light = world.add<rectangle>("Light", point3(0.0f, 3.0f, 5.0f), 3.0f, 3.0f);
	light.right_axis(2);
	light.material = &light_material;
	light.update();

	::camera camera{16.0f / 9.0f};
	camera.origin = point3(-0.25f, 3.0f, -0.5f);
	camera.target = point3(-0.25f, 0.0f, 3.0f);
	camera.vertical_fov = 60.0f;
	return camera;
}

int main()
{
	world world;

	object_store<material> materials = material_store();

	//matrix4x4 m1(4, 5, 6, 2,
	//	1, 2, 4, 8,
	//	7, 1, 5, 6,
	//	4, 1, 2, 3);
	//matrix4x4 m2(4, 1, 2, 3,
	//	5, 7, 3, 7,
	//	1, 4, 5, 1,
	//	2, 4, 7, 9);

	//matrix4x4 id;
	//id.translate(vec3(10.0f, 0.0f, 0.0f));
	////id.translate(vec3(0.0f, 4.0f, 0.0f));
	//
	//auto position = vec3(1.0f, 42.0f, 3.0f);
	//auto p = matrix4x4::multiply(m1, direction3(position)); // (67.000, 92.000, 189.000)
	//auto g = matrix4x4::multiply(id, point3(position)); // 11.0000000, 42.0000000, 3.00000000, 1.00000000
	//
	//auto rotation = matrix4x4::from_euler_degrees(vec3(12.0f, 32.0f, 15.0f));

	//
	camera camera = make_sphere_scene(world, materials); // average: 179 - 184 (no bvh) || post-opti: 68 - 123 (no bvh)
	//camera camera = make_cornell_scene(world, materials); // average: 133 - 105 (no bvh) || post-opti: 58 - 57 (no bvh)
	camera.update();

	material& selection_material = materials.add<lambertian_material>("Selection material", color(1.0f, 0.0, 0.0));

	world.signal_scene_change();

	gui::initialize_opengl();

	// output settings
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());
	raytrace_renderer raytrace_renderer{image_width, image_height};
	raytrace_renderer.current_render.settings.background_strength = 0.05f;
	raytrace_renderer.render(camera, world);

	selection_overlay selection_overlay{raytrace_renderer.current_render};

	void* selection = nullptr;
	void* material_selection = nullptr;
	gui_image render_image{false};

	long long last_render_duration = 0;
	long long total_render_duration = 0;
	long long average_render_time = 0, min_render_duration = 0, max_render_duration = 0;
	float render_prev_iteration = 0;

	bool mouse_over_hierarchy = false;
	bool is_hierarchy_focused;
	vec3 viewer_mouse_pos{-1.0f, -1.0f, -1.0f};

	while (!glfwWindowShouldClose(gui::window))
	{
		bool is_rendering = raytrace_renderer.thread.is_alive;
		gui::start_frame();

		bool scene_changed = false;
		if (ImGui::Begin("Render"))
		{
			auto target_iteration = static_cast<int>(raytrace_renderer.current_render.target_iteration);
			ImGui::Text("Render: %d / %d (avg: %llums)",
			            static_cast<int>(raytrace_renderer.current_render.iteration),
			            target_iteration,
			            average_render_time);
			ImGui::SameLine();

			if (ImGui::Button(is_rendering ? "Pause rendering" : "Resume rendering"))
			{
				if (is_rendering) raytrace_renderer.thread.pause();
				else raytrace_renderer.thread.resume();
			}

			if (ImGui::DragInt("Max render iteration", &target_iteration))
			{
				raytrace_renderer.current_render.target_iteration = static_cast<float>(target_iteration);

				// do not signal scene change if we increased the maximum render iteration, only if it was decreased
				scene_changed |= static_cast<float>(target_iteration) < raytrace_renderer.current_render.iteration;
			}

			scene_changed |= ImGui::DragInt("Max depth", &raytrace_renderer.current_render.settings.bounce_depth);
			scene_changed |= gui::draw_double("Ambiant strength",
			                                  raytrace_renderer.current_render.settings.background_strength);
			scene_changed |= gui::draw_color("Background top",
			                                 raytrace_renderer.current_render.settings.background_top_color);
			scene_changed |= gui::draw_color("Background bottom",
			                                 raytrace_renderer.current_render.settings.background_bottom_color);
			ImGui::Checkbox("Use BVH", &world.use_bvh);
		}
		ImGui::End();

		if (ImGui::Begin("Materials"))
		{
			for (material* mat : materials)
			{
				add_to_hierarchy(mat, mat->name, &material_selection);
			}
		}
		else
		{
			material_selection = nullptr;
		}
		ImGui::End();

		if (is_hierarchy_focused = ImGui::Begin("Hierarchy"), is_hierarchy_focused)
		{
			mouse_over_hierarchy = ImGui::IsWindowHovered();

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
			scene_changed |= draw_inspector(camera, &selection);

			material* mat = nullptr;
			if (material_selection != nullptr && !is_hierarchy_focused)
			{
				mat = static_cast<material*>(material_selection);
			}
			else if (selection != nullptr && selection != &camera)
			{
				mat = *get_selection_material(selection);
			}

			if (mat != nullptr)
			{
				ImGui::NewLine();
				ImGui::Separator();
				ImGui::Text("Material Inspector");
				const bool has_selection = selection != nullptr && material_selection != nullptr;
				if (has_selection)
				{
					ImGui::SameLine();
					if (ImGui::Button("Assign to selection"))
					{
						*get_selection_material(selection) = static_cast<material*>(material_selection);
						scene_changed = true;
					}
				}
				scene_changed |= draw_material_inspector(mat);
			}
		}
		ImGui::End();


		if (scene_changed)
		{
			raytrace_renderer.signal_scene_change();
			raytrace_renderer.render(camera, world);
			selection_overlay.signal_change();
			world.signal_scene_change();
		}

		bool has_selection = selection != nullptr && selection != &camera;
		if (ImGui::Begin("Viewer"))
		{
			bool is_window_focused = ImGui::IsWindowFocused();
			auto image_position = ImGui::GetCursorPos();
			const auto window_size = ImGui::GetContentRegionAvail();
			const ImVec2 size = window_size.y >= window_size.x / static_cast<float>(camera.aspect_ratio())
				                    ? ImVec2(window_size.x, window_size.x / static_cast<float>(camera.aspect_ratio()))
				                    : ImVec2(window_size.y * static_cast<float>(camera.aspect_ratio()), window_size.y);
			ImGui::Image(render_image.texture_id(), size);
			auto rect_min = ImGui::GetItemRectMin();
			auto rect_max = ImGui::GetItemRectMax();
			auto mouse_pos = ImGui::GetMousePos();
			if (mouse_pos.x > rect_min.x && mouse_pos.y > rect_min.y
				&& mouse_pos.x > rect_min.x && mouse_pos.y > rect_min.y
				&& mouse_pos.x < rect_max.x && mouse_pos.y < rect_max.y
				&& mouse_pos.x < rect_max.x && mouse_pos.y < rect_max.y)
			{
				viewer_mouse_pos.x() = ((mouse_pos.x - rect_min.x)) / static_cast<float>(size.x);
				viewer_mouse_pos.y() = 1.0f - ((mouse_pos.y - rect_min.y)) / static_cast<float>(size.y);

				if (is_window_focused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					hit_info hit{&lambertian_material::default_material()};
					if (world.hit(camera.compute_ray_to(viewer_mouse_pos.x(), viewer_mouse_pos.y()), 0.001f,
					              constants::infinity, hit))
					{
						selection = hit.object;
						selection_overlay.signal_change();
					}
				}
			}
			else
			{
				viewer_mouse_pos = vec3{-1.0f};
			}

			if (has_selection && mouse_over_hierarchy)
			{
				selection_overlay.draw_overlay(image_position, size);
			}
		}
		ImGui::End();


		gui::end_frame();
		
		if (is_rendering && render_prev_iteration != raytrace_renderer.current_render.iteration)
		{
			if (scene_changed)
			{
				total_render_duration = 0;
				min_render_duration = 99999;
				max_render_duration = 0;
			}

			render_image.update(image_width, image_height, raytrace_renderer.current_render.front_buffer().data());
			last_render_duration = raytrace_renderer.current_render.last_render_duration;
			render_prev_iteration = raytrace_renderer.current_render.iteration;

			total_render_duration += last_render_duration;
			min_render_duration = std::min(min_render_duration, last_render_duration);
			max_render_duration = std::max(max_render_duration, last_render_duration);
			//average_render_time = static_cast<long long>((min_render_time + max_render_time) * 0.5f);
			average_render_time = static_cast<long long>(
				static_cast<float>(total_render_duration) / (raytrace_renderer.current_render.iteration - 1.0f)
			);
		}

		if (has_selection)
		{
			selection_overlay.render(selection, selection_material, camera, raytrace_renderer);
		}
	}

	raytrace_renderer.clear();
	gui::cleanup();

	return 0;
}

#include <execution>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <string>

#include "stb_image_write.h"
#include "stb_image.h"

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
#include "core/rotate_y.h"
#include "geometry/box.h"

#include "materials/dieletric_material.h"
#include "materials/image_texture.h"

#include "serializable_node.h"
#include "thread_pool.h"


glm::mat4 set_position_and_rotation(const vec3& position, float angle, const vec3& axis)
{
	return rotate(translate(position), glm::radians(angle), axis);
}

camera make_cornell_scene(world& world, object_store<material>& materials)
{
	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 4.0f;

	auto& cornell_red = materials.add<lambertian_material>("Cornell red", color(.65f, .05f, .05f));
	auto& cornell_white = materials.add<lambertian_material>("Cornell white", color(.73f, .73f, .73f));
	auto& cornell_green = materials.add<lambertian_material>("Cornell green", color(.12f, .45f, .15f));

	vec3 size = vec3(2.0f);

	rectangle* walls[] = {
		&world.add<rectangle>("back_wall"),
		&world.add<rectangle>("right_wall"),
		&world.add<rectangle>("left_wall"),
		&world.add<rectangle>("top_wall"),
		&world.add<rectangle>("bottom_wall"),
	};

	
	walls[1 - 1]->transform = translate(point3(0, 0, -size.z * 0.5f));
	walls[1 - 1]->size = vec2(size.x, size.y);
	walls[1 - 1]->material = &cornell_white;
	walls[1 - 1]->update();

	walls[2 - 1]->transform = rotate(translate(point3(-size.x * 0.5f, 0, 0)), glm::radians(-90.0f),
	                                 vec3{0.0f, 1.0f, 0.0f});
	walls[2 - 1]->size = vec2(size.z, size.y);
	walls[2 - 1]->material = &cornell_green;
	walls[2 - 1]->update();
	walls[3 - 1]->transform = rotate(translate(point3(size.x * 0.5f, 0, 0)), glm::radians(90.0f),
	                                 vec3{0.0f, 1.0f, 0.0f});
	walls[3 - 1]->size = vec2(size.z, size.y);
	walls[3 - 1]->update();
	walls[3 - 1]->material = &cornell_red;

	walls[4 - 1]->transform = rotate(translate(point3(0, size.y * 0.5f, 0)), glm::radians(-90.0f),
	                                 vec3{1.0f, 0.0f, 0.0f});
	walls[4 - 1]->size = vec2(size.z, size.x);
	walls[4 - 1]->material = &cornell_white;
	walls[4 - 1]->update();
	walls[5 - 1]->transform = rotate(translate(point3(0, -size.y * 0.5f, 0)), glm::radians(90.0f),
	                                 vec3{1.0f, 0.0f, 0.0f});
	walls[5 - 1]->size = vec2(size.z, size.x);
	walls[5 - 1]->material = &cornell_white;
	walls[5 - 1]->update();

	auto& light = world.add<rectangle>("light");
	light.transform = set_position_and_rotation(vec3(0, size.y * 0.45f, 0), 90.0f, vector3::right());
	light.size = vec2(0.447f);
	light.material = &light_material;
	light.update();

	auto& right_box = world.add<box>("Tall box", vec3(0.6f, 1.1f, 0.6f));
	right_box.transform = set_position_and_rotation(vec3(-0.4f, -0.45f, -0.45f), 18.0f, vector3::up());
	right_box.update();

	auto& left_box = world.add<box>("Small box", vec3(0.6f));
	left_box.transform = set_position_and_rotation(vec3(0.3f, -0.7f, 0.25f), -15.0f, vector3::up());
	left_box.update();

	//auto& sphere = world.add<::sphere>("Sphere", vec3(0.3f, -0.7f + 0.65f, 0.25f), 0.25f);
	//sphere.material = &materials.add<metal_material>("Chrome", color(0.8f, 0.8f, 0.8f), 0.05f);
	//sphere.update();

	::camera camera{1.0f};
	camera.origin = point3(0.0f, 0.0f, size.z * 2.0f);
	camera.target = point3(0.0f, 0.0f, 0.0f);
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

	world.add<sphere>("Ground", point3(0.0f, -0.25f - 100.0f, 0.0f), 100.0f);

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

	auto& wall_left = world.add<rectangle>("wall_left", point3(5.0f, 0.0f, 0.0f), 30.0f, 5.0f);
	wall_left.transform = rotate(wall_left.transform, glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
	wall_left.update();

	auto& wall_right = world.add<rectangle>("wall_right", point3(-5.0f, 0.0f, 0.0f), 30.0f, 5.0f);
	wall_right.transform = rotate(wall_right.transform, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	wall_right.update();

	auto& wall_top = world.add<rectangle>("wall_top", point3(0.0f, 6.0f, 0.0f), 30.0f, 30.0f);
	wall_top.transform = rotate(wall_top.transform, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	wall_top.update();

	//auto& wall_blocker = world.add<rectangle>("wall_blocker", point3(0.0f, 0.2f, 1.9f), 5.0f, 5.0f);
	//wall_blocker.transform = rotate(wall_blocker.transform, glm::radians(45.0f), vec3(0.0f, 0.0f, 1.0f));
	//wall_blocker.update();

	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 1.0f;
	auto& light = world.add<rectangle>("Light", point3(0.0f, 3.0f, 5.0f), 3.0f, 3.0f);
	light.transform = rotate(light.transform, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	light.material = &light_material;
	light.update();

	::camera camera{16.0f / 9.0f};
	camera.origin = point3(-0.25f, 3.0f, -0.5f);
	camera.target = point3(-0.25f, 0.0f, 3.0f);
	camera.vertical_fov = 60.0f;
	return camera;
}

camera make_simple_scene(world& world, object_store<material>& materials)
{
	auto& earth_material = materials.add<lambertian_material>(
		"Earth", texture_store().add<image_texture>("earthmap.jpg"));
	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 4.0f;

	//world.add<sphere>("Ground", point3(0.0f, -100.0f - 0.4f, 0.0f), 100.0f);
	auto& light = world.add<sphere>("Light", point3(0.0f, 0.0f, -0.8f), 0.25f);
	light.material = &light_material;
	light.update();

	world.add<sphere>("Sphere", point3(0.0f, 0.0f, -3.0f), 2.0f).material = &earth_material;

	::camera camera{16.0f / 9.0f};
	camera.origin = point3(0.0f, 0.0f, 12.0f);
	camera.target = point3(0.0f, 0.0f, 0.0f);
	camera.vertical_fov = 20.0f;
	return camera;
}

camera make_box_scene(world& world, object_store<material>& materials)
{
	//world.add<rectangle>("1");
	//world.add<rectangle>("2");
	//world.add<rectangle>("3");
	//world.add<rectangle>("4");

	world.add<box>("box");

	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 4.0f;

	auto& light = world.add<sphere>("Light", point3(0.0f, 0.0f, -0.8f), 0.25f);
	light.material = &light_material;
	light.update();

	::camera camera{16.0f / 9.0f};
	camera.origin = point3(0.0f, 0.0f, 12.0f);
	camera.target = point3(0.0f, 0.0f, 0.0f);
	camera.vertical_fov = 20.0f;
	return camera;
}

int main()
{
	world world;

	object_store<material> materials = material_store();
	
	camera camera = [&]()
	{
		switch (2)
		{
		case 0:
			return make_sphere_scene(world, materials); // average: 179 - 184 (no bvh) || post-opti: 68 - 123 (no bvh)
		case 1:
			return make_cornell_scene(world, materials); // average: 133 - 105 (no bvh) || post-opti: 58 - 57 (no bvh)
		case 2:
			return make_simple_scene(world, materials);
		case 3:
			return make_box_scene(world, materials);
		}
		return make_simple_scene(world, materials);
	}();

	camera.update();
	material& selection_material = materials.add<lambertian_material>("Selection material", color(1.0f, 0.0, 0.0));

	world.signal_scene_change();

	gui::initialize_opengl();

	// output settings
	const int image_width = 600;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());
	raytrace_renderer raytrace_renderer{image_width, image_height};
	raytrace_renderer.current_render.settings.background_bottom_color = color(0.2f);
	raytrace_renderer.current_render.settings.background_top_color = color(0.2f);
	raytrace_renderer.current_render.settings.background_strength = 0.05f;

	hit_info ahit{&lambertian_material::default_material()};
	world.hit(ray(point3(0.0f), direction3(0.0f, 0.0f, 1.0f)), 0.001f, constants::infinity, ahit);

	raytrace_renderer.render(camera, world);

	selection_overlay selection_overlay{raytrace_renderer.current_render};

	serializable* selection = nullptr;
	serializable* material_selection = nullptr;
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
			scene_changed |= gui::draw_float("Ambiant strength",
			                                  raytrace_renderer.current_render.settings.background_strength);
			scene_changed |= gui::draw_color("Background top",
			                                 raytrace_renderer.current_render.settings.background_top_color);
			scene_changed |= gui::draw_color("Background bottom",
			                                 raytrace_renderer.current_render.settings.background_bottom_color);
			ImGui::Checkbox("Use BVH", &world.use_bvh);
			if (ImGui::Button("Save to image"))
			{
				std::string filename = "rtracer_" + std::to_string(average_render_time) + "ms_" + std::to_string(
					static_cast<int>(raytrace_renderer.current_render.iteration)) + "total " + std::to_string(total_render_duration) + "_" + ".jpg";
				raytrace_renderer.save_to_image(filename);
			}
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
				mat = static_cast<::hittable*>(selection)->material;
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
						static_cast<::hittable*>(selection)->material = static_cast<material*>(material_selection);
						scene_changed = true;
					}
				}

				scene_changed |= inspector.serialize_root(mat->serialize().get());
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
				viewer_mouse_pos.x = ((mouse_pos.x - rect_min.x)) / static_cast<float>(size.x);
				viewer_mouse_pos.y = 1.0f - ((mouse_pos.y - rect_min.y)) / static_cast<float>(size.y);

				if (is_window_focused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					hit_info hit{&lambertian_material::default_material()};
					if (world.hit(camera.compute_ray_to(viewer_mouse_pos.x, viewer_mouse_pos.y), 0.001f,
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

			if (raytrace_renderer.current_render.iteration > 2.0f)
			{
				total_render_duration += last_render_duration;
				min_render_duration = std::min(min_render_duration, last_render_duration);
				max_render_duration = std::max(max_render_duration, last_render_duration);
				//average_render_time = static_cast<long long>((min_render_time + max_render_time) * 0.5f);
				average_render_time = static_cast<long long>(
					static_cast<float>(total_render_duration) / (raytrace_renderer.current_render.iteration - 2.0f)
				);
			}
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

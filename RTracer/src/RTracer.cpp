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
#include "serializable_node.h"
#include "world.h"
#include "core/rotate_y.h"
#include "geometry/box.h"

#include "materials/dieletric_material.h"
#include "materials/image_texture.h"

class inspector_serializer : public serializer
{
public:
	bool serialize_root(serializable_node_root* root) override
	{
		bool changed = false;
		if (ImGui::CollapsingHeader(root->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (serializable_node_root* child : root->children)
			{
				changed |= child->visit(*this);
			}
		}

		return changed;
	}

	bool serialize(const std::string& name, double* value) override
	{
		return gui::draw_double(name.c_str(), *value);
	}

	bool serialize(const std::string& name, vec3* value)
	{
		return gui::draw_vec3(name.c_str(), *value);
	}

	bool serialize(const std::string& name, color* value)
	{
		return gui::draw_color(name.c_str(), *value);
	}
};


int main()
{
	double angle = 1.0;
	auto n = serializable_node_root("root", {
		                                new serializable_node<double>("angle", &angle, {})
	                                });

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
	auto& checker_tex = texture_store().add<checker_texture>(color::black(), color::white());
	materials.add<lambertian_material>("Checker", checker_tex);
	materials.add<lambertian_material>("Earth", texture_store().add<image_texture>("earthmap.jpg"));

	//world.add<sphere>("Ground", point3(0.0, -1.25 - 100, 0.0), 100);

	//char name[] = "Sphere 00";
	//for (int z = 0; z < 1; z++)
	//{
	//	int x_span = 6;
	//	for (int x = 0; x < x_span; x++)
	//	{
	//		name[7] = static_cast<char>('0' + z);
	//		name[8] = static_cast<char>('0' + x);
	//		auto& obj = world.add<sphere>(
	//			name, point3((x - x_span * 0.5), 0.2, (z + 1.0)),
	//			0.3);
	//		obj.material = materials[random::get<size_t>(1, materials.size()) - 1];
	//	}
	//}

	auto& light_material = materials.add<lambertian_material>("Light", *solid_color::white());
	light_material.emission = solid_color::white();
	light_material.emission_strength = 1.0;

	auto& cornell_red = materials.add<lambertian_material>("Cornell red", color(.65, .05, .05));
	auto& cornell_white = materials.add<lambertian_material>("Cornell white", color(.73, .73, .73));
	auto& cornell_green = materials.add<lambertian_material>("Cornell green", color(.12, .45, .15));

	double cornell_width = 30.0;
	double cornell_height = 30.0;
	auto& light = world.add<rectangle>("Light", point3(0.0, cornell_height * 0.5 - 1 + cornell_height * 0.5, 0.0), 10.0,
	                                   10.0);
	light.right_axis(2);
	light.material = &light_material;
	light.update();

	auto& plane1 = world.add<rectangle>("Right wall", point3(-cornell_width * 0.5, cornell_height * 0.5, 0.0),
	                                    cornell_height,
	                                    cornell_width);
	plane1.right_axis(1);
	plane1.material = &cornell_red;
	plane1.update();
	auto& plane2 = world.add<rectangle>("Left wall", point3(cornell_width * 0.5, cornell_height * 0.5, 0.0),
	                                    cornell_height, cornell_width);
	plane2.right_axis(1);
	plane2.material = &cornell_green;
	plane2.update();
	auto& plane3 = world.add<rectangle>("Top wall", point3(0.0, cornell_height * 0.5 + cornell_height * 0.5, 0.0),
	                                    cornell_width, cornell_width);
	plane3.right_axis(2);
	plane3.material = &cornell_white;
	plane3.update();
	auto& plane4 = world.add<rectangle>("Bottom wall", point3(0.0, -cornell_height * 0.5 + cornell_height * 0.5, 0.0),
	                                    cornell_width,
	                                    cornell_width);
	plane4.right_axis(2);
	plane4.material = &cornell_white;
	plane4.update();
	auto& plane5 = world.add<rectangle>("Back wall", point3(0.0, cornell_height * 0.5, cornell_width * 0.5),
	                                    cornell_width, cornell_height);
	plane5.right_axis(0);
	plane5.material = &cornell_white;
	plane5.update();

	auto backbox_size = vec3(cornell_height * 0.5 * 0.5, cornell_height * 0.5, cornell_height * 0.5 * 0.5);
	auto& backbox = world.add<translator>(new rotate_y(new box("Back box", backbox_size), 15.0));
	backbox.position = point3(backbox_size.x() * 0.5, 0.0, cornell_width * 0.5 * 0.75 - backbox_size.z() * 0.5);
	backbox.position.x() += 1.0;
	backbox.position.y() += backbox_size.y() * 0.5;
	backbox.update();

	auto frontbox_size = vec3(cornell_height * 0.5 * 0.5);
	auto& frontbox = world.add<translator>(new rotate_y(new box("Front box", frontbox_size), -18.0));
	frontbox.position.x() -= frontbox_size.x() * 0.5 + 1.0;
	frontbox.position.y() += frontbox_size.y() * 0.5;
	frontbox.update();

	world.signal_scene_change();

	gui::initialize_opengl();

	// camera settings
	camera camera(4.0 / 3.0);
	camera.origin = point3(0.0, cornell_height * 0.5, -45);
	camera.target = point3(0.0, cornell_height * 0.5 * 0.85, 0.0);
	camera.vertical_fov = 40.0;
	camera.update();

	// output settings
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());
	raytrace_renderer raytrace_renderer{image_width, image_height};
	raytrace_renderer.current_render.background_strength = 0.05 * 10.0;

	selection_overlay selection_overlay{raytrace_renderer.current_render};

	void* selection = nullptr;
	void* material_selection = nullptr;
	gui_image render_image{false};
	bool pause_rendering = false;
	long long last_render_time = 0;
	int max_render_iteration = 1000;
#if _DEBUG
	max_render_iteration = 2;
#endif
	bool mouse_over_hierarchy = false;
	bool is_hierarchy_focused;
	vec3 viewer_mouse_pos{-1.0, -1.0, -1.0};

	while (!glfwWindowShouldClose(gui::window))
	{
		bool is_rendering = !pause_rendering && raytrace_renderer.current_render.iteration < max_render_iteration;
		gui::start_frame();

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

			int prev_max_render_iteration = max_render_iteration;
			if (ImGui::DragInt("Max render iteration", &max_render_iteration))
			{
				// do not signal scene change if we increased the maximum render iteration, only if it was decreased
				scene_changed |= prev_max_render_iteration > max_render_iteration;
			}

			scene_changed |= ImGui::DragInt("Max depth", &raytrace_renderer.current_render.bounce_depth);
			scene_changed |= gui::draw_double("Ambiant strength", raytrace_renderer.current_render.background_strength);
			scene_changed |= gui::draw_color("Background top", raytrace_renderer.current_render.background_top_color);
			scene_changed |= gui::draw_color("Background bottom",
			                                 raytrace_renderer.current_render.background_bottom_color);
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
			inspector_serializer inspector;
			inspector.serialize_root(&n);
			
			//scene_changed |= draw_inspector(camera, &selection);

			//material* mat = nullptr;
			//if (material_selection != nullptr && !is_hierarchy_focused)
			//{
			//	mat = static_cast<material*>(material_selection);
			//}
			//else if (selection != nullptr && selection != &camera)
			//{
			//	mat = *get_selection_material(selection);
			//}

			//if (mat != nullptr)
			//{
			//	ImGui::NewLine();
			//	ImGui::Separator();
			//	ImGui::Text("Material Inspector");
			//	const bool has_selection = selection != nullptr && material_selection != nullptr;
			//	if (has_selection)
			//	{
			//		ImGui::SameLine();
			//		if (ImGui::Button("Assign to selection"))
			//		{
			//			*get_selection_material(selection) = static_cast<material*>(material_selection);
			//			scene_changed = true;
			//		}
			//	}
			//	scene_changed |= draw_material_inspector(mat);
			//}
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
				viewer_mouse_pos.x() = ((mouse_pos.x - rect_min.x)) / static_cast<double>(size.x);
				viewer_mouse_pos.y() = 1.0 - ((mouse_pos.y - rect_min.y)) / static_cast<double>(size.y);

				if (is_window_focused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					hit_info hit{&lambertian_material::default_material()};
					if (world.hit(camera.compute_ray_to(viewer_mouse_pos.x(), viewer_mouse_pos.y()), 0.001,
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

		if (has_selection)
		{
			selection_overlay.render(selection, selection_material, camera, raytrace_renderer);
		}
	}

	gui::cleanup();

	return 0;
}

// RTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <conio.h> // to read single char from console>
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <chrono>
#include <chrono>

#include "core/stb_image_write.h"

#include "core/utility.h"
#include "core/vec3.h"
#include "core/direction3.h"
#include "core/point3.h"
#include "core/vec3_utility.h"
#include "core/ray.h"
#include "core/color.h"
#include "core/hittable.h"

#include "materials/lambertian_material.h"
#include "materials/metal_material.h"

#include "geometry/plane.h"
#include "geometry/sphere.h"

#include "camera.h"
#include "imgui_initializer.h"
#include "world.h"
#include "materials/dieletric_material.h"

color ray_color(const ray& raycast, const world& world, int depth)
{
	if (depth <= 0)
		return color::magenta();

	color attenuation;
	ray scattered;
	hit_info hit{&lambertian_material::default_material()};
	if (world.hit(raycast, 0.001, infinity, hit))
	{
		if (hit.material->scatter(raycast, hit, attenuation, scattered))
		{
			return color(mul(attenuation, ray_color(scattered, world, depth - 1)));
		}

		return color::black();
	}

	vec3 unit_direction = normalize(raycast.direction());
	const double t = 0.5 * (unit_direction.y() + 1.0);
	return color((1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0));
}

struct pixel
{
	int index;
	double x;
	double y;

	pixel(int index, double x, double y) : index(index), x(x), y(y)
	{
	}
};

long long Render(const camera& camera, const world& world, const std::vector<pixel>& pixels,
                 std::vector<unsigned char>& pixel_colors,
                 const int image_width, const int image_height)
{
	// render settings
	const uint samples_per_pixel = 2;
	const double inv_samples_per_pixel = 1.0 / samples_per_pixel;
	const int max_depth = 12;

	auto start = std::chrono::high_resolution_clock::now();
	std::for_each(
		std::execution::par_unseq,
		pixels.begin(),
		pixels.end(),
		[&pixel_colors, &world, &camera,
			samples_per_pixel, inv_samples_per_pixel, image_height, image_width, max_depth](pixel pixel)
		{
			color pixel_color;
			for (uint i = 0; i < samples_per_pixel; ++i)
			{
				const auto u = static_cast<double>(pixel.x + random_double()) / (image_width - 1);
				const auto v = static_cast<double>(pixel.y + random_double()) / (image_height - 1);
				pixel_color += ray_color(camera.compute_ray_to(u, v), world, max_depth);
			}

			pixel_colors[pixel.index] = to_writable_color(pixel_color.x(), inv_samples_per_pixel);
			pixel_colors[pixel.index + 1] = to_writable_color(pixel_color.y(), inv_samples_per_pixel);
			pixel_colors[pixel.index + 2] = to_writable_color(pixel_color.z(), inv_samples_per_pixel);
		});

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	return duration.count();
}

void UpdateOpenGLViewer(const int image_width, const int image_height, std::vector<unsigned char> pixel_colors, GLuint render_texture_id)
{
	glBindTexture(GL_TEXTURE_2D, render_texture_id);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
	             GL_RGB, GL_UNSIGNED_BYTE, pixel_colors.data());
}

int main()
{
	// camera settings
	camera camera(16.0 / 9.0);

	// output settings
	const int image_width = 512;
	const int image_height = static_cast<int>(image_width / camera.aspect_ratio());
	const uint channels_num = 3;

	world world;

	material& redMat = world.add_material<lambertian_material>(color(1.0, 0.0, 0.0));
	material& glassMat = world.add_material<dielectric_material>(1.5);
	material& shinyMetal = world.add_material<metal_material>(color(0.8, 0.8, 0.8), 0.05);
	material& roughMetal = world.add_material<metal_material>(color(0.8, 0.6, 0.2), 0.6);

	plane& ground = world.add<plane>(point3(0.0, -1.25, 0.0), direction3(0.0, 1.0, 0.0));
	sphere& leftSphere = world.add<sphere>(point3(-2.5, 0, 3.5), 1.25);
	sphere& rightSphere = world.add<sphere>(point3(1.5, 0.5, 3), 1.25);
	sphere& smallSphere = world.add<sphere>(point3(0, 0, 0), 0.3);

	smallSphere.material = &glassMat;
	rightSphere.material = &shinyMetal;
	leftSphere.material = &roughMetal;

	int pixel_count = image_width * image_height;

	std::vector<pixel> pixels;
	pixels.reserve(pixel_count);
	std::vector<unsigned char> pixel_colors;
	pixel_colors.reserve(pixel_count * channels_num);
	int index = 0;
	for (int y = image_height - 1; y >= 0; --y)
	{
		for (int x = 0; x < image_width; ++x)
		{
			pixels.emplace_back(index, x, y);
			for (int i = 0; i < channels_num; ++i)
			{
				pixel_colors.emplace_back();
				index++;
			}
		}
	}

	gui::initialize_opengl();
	GLuint render_texture_id;
	glGenTextures(1, &render_texture_id);

	long last_render_time = Render(camera, world, pixels, pixel_colors, image_width, image_height);
	UpdateOpenGLViewer(image_width, image_height, pixel_colors, render_texture_id);

	while (!glfwWindowShouldClose(gui::window))
	{
		bool render = false;
		gui::start_frame();

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		ImGui::SetNextWindowDockID(0, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Hierarchy"))
		{
			ImGui::Text("Render time: %llums", last_render_time);
			//vec3 direction = vec3::zero();
			//double speed = 0.04;
			//if (ImGui::Button("Forward")) direction = vec3::forward();
			//if (ImGui::Button("Backward")) direction = vec3::backward();
			//if (ImGui::Button("Right")) direction = vec3::right();
			//if (ImGui::Button("Left")) direction = vec3::left();
			//if (direction.length_squared() > epsilon)
			//{
			//	camera.origin = point3(camera.origin + direction * speed);
			//	camera.update();
			//	render = true;
			//}

			if (ImGui::CollapsingHeader("Camera"))
			{
				bool changed = false;
				float* tmp = gui::vec3_to_temporary_float3(camera.origin);
				if ((changed |= ImGui::DragFloat3("Origin", tmp)))
				{
					camera.origin = point3(gui::float3_to_vec3(tmp));
				}

				tmp = gui::vec3_to_temporary_float3(camera.target);
				if ((changed |= ImGui::DragFloat3("Target", tmp)))
				{
					camera.target = point3(gui::float3_to_vec3(tmp));
				}

				if (changed)
				{
					camera.update();
					render = true;
				}
			}
		}
		ImGui::End();
		ImGui::SetNextWindowDockID(1, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Viewer"))
		{
			auto width = ImGui::GetWindowSize().x;
			ImGui::Image((ImTextureID)render_texture_id, ImVec2(width, width / camera.aspect_ratio()));
		}
		ImGui::End();

		gui::end_frame();

		if (render)
		{
			last_render_time = Render(camera, world, pixels, pixel_colors, image_width, image_height);
			UpdateOpenGLViewer(image_width, image_height, pixel_colors, render_texture_id);
		}
	}
	gui::cleanup();

	return 0;
}

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

	char input = ' ';
	do
	{
		// render settings
		const uint samples_per_pixel = input == 'r' ? 1000 : 4;
		const double inv_samples_per_pixel = 1.0 / samples_per_pixel;
		const int max_depth = 12;

		vec3 direction = vec3::zero();
		double speed = 0.04;
		if (input == 'z') direction = vec3(0.0, 0.0, 1.0);
		else if (input == 'q') direction = vec3(-1.0, 0.0, 0.0);
		else if (input == 's') direction = vec3(0.0, 0.0, -1.0);
		else if (input == 'd') direction = vec3(1.0, 0.0, 0.0);
		else if (input == 'a') camera.vertical_fov -= 5.0;
		else if (input == 'e') camera.vertical_fov += 5.0;

		camera.origin = point3(camera.origin + direction * speed);
		camera.update();
		
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

		stbi_write_jpg("rtracer_output.jpg", image_width, image_height, channels_num, pixel_colors.data(),
		               image_width * channels_num);
		auto stop = std::chrono::high_resolution_clock::now(); 
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		std::cout << duration.count() << std::endl; 

		input = _getch();
	} while (input != '\n');

	return 0;
}

#pragma once
#include <chrono>
#include <execution>

#include "camera.h"
#include "world.h"
#include "core/color.h"
#include "materials/lambertian_material.h"

struct raytrace_settings
{
	const int image_width;
	const int image_height;

	int max_depth = 12;
};

struct raytrace_pixel
{
	long index;
	double x;
	double y;
	color color = color::black();

	raytrace_pixel(int index, double x, double y) : index(index), x(x), y(y)
	{
	}
};

class raytrace_renderer
{
public:
	raytrace_renderer(int image_width, int image_height)
		: settings{image_width, image_height}
	{
		const int channels_num = 3;
		const int pixel_count = image_width * image_height;

		m_pixels.reserve(pixel_count);
		m_default_pixels.reserve(pixel_count);
		m_pixel_colors.reserve(pixel_count * channels_num);
		int index = 0;
		for (int y = image_height - 1; y >= 0; --y)
		{
			for (int x = 0; x < image_width; ++x)
			{
				m_pixels.emplace_back(index, x, y);
				m_default_pixels.emplace_back(index, x, y);
				for (int i = 0; i < channels_num; ++i)
				{
					m_pixel_colors.emplace_back();
					index++;
				}
			}
		}
	}

	void render(const camera& camera, const world& world)
	{
		// render settings
		const double inv_samples_per_pixel = 1.0 / m_iteration;
		m_iteration++;

		auto& pixel_colors = m_pixel_colors;
		auto& settings = this->settings;
		std::for_each(
			std::execution::par_unseq,
			m_pixels.begin(),
			m_pixels.end(),
			[&pixel_colors, &world, &camera,
				inv_samples_per_pixel, settings](raytrace_pixel& pixel)
			{
				const auto u = (pixel.x + random_double()) / (static_cast<double>(settings.image_width) - 1);
				const auto v = (pixel.y + random_double()) / (static_cast<double>(settings.image_height) - 1);
				pixel.color += ray_color(camera.compute_ray_to(u, v), world, settings.max_depth);

				pixel_colors[pixel.index] = to_writable_color(pixel.color.x(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 1] = to_writable_color(pixel.color.y(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 2] = to_writable_color(pixel.color.z(), inv_samples_per_pixel);
			});
	}

	void signal_scene_change()
	{
		std::memcpy(m_pixels.data(), m_default_pixels.data(), m_pixels.size() * sizeof(raytrace_pixel));
		m_iteration = 1.0;
	}

	const unsigned char* pixel_colors() const
	{
		return m_pixel_colors.data();
	}

	raytrace_settings settings;

private:
	static color ray_color(const ray& raycast, const world& world, int depth)
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

private:
	std::vector<raytrace_pixel> m_pixels;
	std::vector<raytrace_pixel> m_default_pixels;
	std::vector<unsigned char> m_pixel_colors;

	double m_iteration = 1.0;
};

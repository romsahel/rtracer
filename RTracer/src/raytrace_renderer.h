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

	bool use_bvh;
};

/// <summary>
/// represent a raytraced pixel with its index, its coordinates in the image and its resulting color.
/// These first three parameters are constant during the lifetime of an object
/// </summary>
struct raytrace_pixel
{
	const long index;
	const double x;
	const double y;
	color color = color::black();

	raytrace_pixel(int index, double x, double y) : index(index), x(x), y(y)
	{
	}
};

/// <summary>
/// represent the result of a render
/// an instance of this class is to be given to the raytrace_renderer to produce an image
/// it contains:
/// - the pixels of the image (see render_settings for the dimension of the vector)
/// - the colors of the image (it is a conversion of all the colors from pixels
///							   into an image-format array of ascii colors, used to write into a image-file or a opengl texture buffer)
///	- the current iteration of the render (since we use progressive rendering in order to have responsive feedback)
///	- settings specific for this render: the maximum bounce depth and the color used when it is reached
/// </summary>
struct raytrace_render_data
{
	std::vector<raytrace_pixel> pixels;
	std::vector<unsigned char> colors;
	double iteration = 1.0;

	int bounce_depth = 12;
	color bounce_depth_limit_color = color::magenta();

	void set_pixels_from(const raytrace_render_data& src)
	{
		std::memcpy(pixels.data(), src.pixels.data(), pixels.size() * sizeof(raytrace_pixel));
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

		current_render.pixels.reserve(pixel_count);
		empty_render.pixels.reserve(pixel_count);

		current_render.colors.reserve(pixel_count * channels_num);

		int index = 0;
		for (int y = image_height - 1; y >= 0; --y)
		{
			for (int x = 0; x < image_width; ++x)
			{
				current_render.pixels.emplace_back(index, x, y);
				empty_render.pixels.emplace_back(index, x, y);
				for (int i = 0; i < channels_num; ++i)
				{
					current_render.colors.emplace_back();
					index++;
				}
			}
		}
	}

	/// <summary>
	/// signal the renderer that the scene has changed so that it resets its current render from scratch
	/// </summary>
	void signal_scene_change()
	{
		current_render.set_pixels_from(empty_render);
		current_render.iteration = 1.0;
	}

	/// <summary>
	/// render to the default current_render
	/// </summary>
	void render(const camera& camera, const world& world)
	{
		render(camera, world, current_render, ray_color_with_gradient_sky);
	}

	using ray_color_provider = color(*)(const ray&, const ::world&, int, color);

	/// <summary>
	/// render to a custom render_data using a custom ray_color_provider.
	/// Default one is ray_color_with_gradient_sky
	/// </summary>
	void render(const camera& camera, const world& world, raytrace_render_data& data, ray_color_provider ray_color) const
	{
		// render settings
		const double inv_samples_per_pixel = 1.0 / data.iteration;
		data.iteration++;

		auto& settings = this->settings;
		auto& pixel_colors = data.colors;
		auto bounce_depth = data.bounce_depth;
		auto bounce_depth_limit_color = data.bounce_depth_limit_color;
		std::for_each(
			std::execution::par_unseq,
			data.pixels.begin(),
			data.pixels.end(),
			[&pixel_colors, &world, &camera,
				inv_samples_per_pixel, settings, bounce_depth, bounce_depth_limit_color, ray_color](raytrace_pixel& pixel)
			{
				const auto u = (pixel.x + random::get<double>()) / (static_cast<double>(settings.image_width) - 1);
				const auto v = (pixel.y + random::get<double>()) / (static_cast<double>(settings.image_height) - 1);
				pixel.color += ray_color(camera.compute_ray_to(u, v), world, bounce_depth, bounce_depth_limit_color);

				// convert pixel.color into image-readable ascii pixel_colors
				pixel_colors[pixel.index] = to_writable_color(pixel.color.x(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 1] = to_writable_color(pixel.color.y(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 2] = to_writable_color(pixel.color.z(), inv_samples_per_pixel);
			});
	}

	/// <summary>
	/// return the color for the given raycast, using a blue-gradient sky (when the raycast returns no hit)
	/// </summary>
	static color ray_color_with_gradient_sky(const ray& raycast, const world& world, int depth, color bounce_depth_limit_color)
	{
		color value;
		if (base_ray_color(raycast, world, depth, bounce_depth_limit_color, value, ray_color_with_gradient_sky)) return value;

		const double t = 0.5 * (raycast.direction().y() + 1.0);
		return color((1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0));
	}
	
	/// <summary>
	/// return the color for the given raycast, using a black solid-color sky (when the raycast returns no hit)
	/// </summary>
	static color ray_color_for_mask(const ray& raycast, const world& world, int depth, color bounce_depth_limit_color)
	{
		color value;
		if (base_ray_color(raycast, world, depth, bounce_depth_limit_color, value, ray_color_with_gradient_sky)) return value;

		return color::black();
	}

	raytrace_settings settings;
	raytrace_render_data current_render;
	raytrace_render_data empty_render;

private:
	static bool base_ray_color(const ray& raycast, const world& world, int depth, color bounce_depth_limit_color, color& value,
	                           ray_color_provider ray_color)
	{
		if (depth <= 0)
		{
			value = bounce_depth_limit_color;
			return true;
		}

		color attenuation;
		ray scattered;
		hit_info hit{&lambertian_material::default_material()};
		if (world.hit(raycast, 0.001, infinity, hit))
		{
			if (hit.material->scatter(raycast, hit, attenuation, scattered))
			{
				value = color(mul(attenuation, ray_color(scattered, world, depth - 1, bounce_depth_limit_color)));
			}
			else
			{
				value = color::black();
			}

			return true;
		}
		return false;
	}
};

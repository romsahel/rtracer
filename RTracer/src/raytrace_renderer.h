#pragma once
#include <chrono>
#include <execution>

#include "camera.h"
#include "world.h"
#include "core/color.h"
#include "materials/lambertian_material.h"

struct raytrace_settings
{
	raytrace_settings(int image_width, int image_height)
		: image_width(image_width),
		  image_height(image_height)
	{
	}

	const int image_width;
	const int image_height;

	bool use_bvh = true;
};

/// <summary>
/// represent a raytraced pixel with its index, its coordinates in the image and its resulting color.
/// These first three parameters are constant during the lifetime of an object
/// </summary>
struct raytrace_pixel
{
	const long index;
	const float x;
	const float y;
	color color = color::black();

	raytrace_pixel(int index, float x, float y) : index(index), x(x), y(y)
	{
	}
};

struct raytrace_render_settings
{
	int bounce_depth = 12;
	color bounce_depth_limit_color = color::black();

	color background_bottom_color = color::white();
	color background_top_color = color(0.5f, 0.7f, 1.0f);
	float background_strength = 1.0f;
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
	float iteration = 1.0f;
	raytrace_render_settings settings;

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
		const size_t channels_num = 3;
		const size_t pixel_count = static_cast<size_t>(image_width) * image_height;

		current_render.pixels.reserve(pixel_count);
		empty_render.pixels.reserve(pixel_count);

		current_render.colors.reserve(pixel_count * channels_num);

		int index = 0;
		for (float y = static_cast<float>(image_height - 1); y >= 0; --y)
		{
			for (float x = 0; x < static_cast<float>(image_width); ++x)
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
		current_render.iteration = 1.0f;
	}

	/// <summary>
	/// render to the default current_render
	/// </summary>
	void render(const camera& camera, world& world)
	{
		render(camera, world, current_render);
	}

	using ray_color_provider = color(*)(const ray&, const ::world&, int, color);

	/// <summary>
	/// render to a custom render_data using a custom ray_color_provider.
	/// Default one is ray_color_with_gradient_sky
	/// </summary>
	void render(const camera& camera, world& world, raytrace_render_data& data) const
	{
		// render settings
		const float inv_samples_per_pixel = 1.0f / data.iteration;
		data.iteration++;

		const raytrace_settings& raytrace_settings = this->settings;
		const raytrace_render_settings render_settings = data.settings;
		std::vector<unsigned char>& pixel_colors = data.colors;
		float inv_width = 1.0f / (static_cast<float>(raytrace_settings.image_width) - 1);
		float inv_height = 1.0f / (static_cast<float>(raytrace_settings.image_height) - 1);
		std::for_each(
			std::execution::par,
			data.pixels.begin(),
			data.pixels.end(),
			[&pixel_colors, &world, &camera, render_settings,
				inv_samples_per_pixel, inv_width, inv_height](raytrace_pixel& pixel)
			{
				const float u = (pixel.x + random::static_double.get()) * inv_width;
				const float v = (pixel.y + random::static_double.get()) * inv_height;
				pixel.color += ray_color_with_gradient_sky_attenuated(camera.compute_ray_to(u, v), world, render_settings, color::white(), color::black());
				
				// convert pixel.color into image-readable ascii pixel_colors
				pixel_colors[pixel.index] = to_writable_color(pixel.color.x(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 1] = to_writable_color(pixel.color.y(), inv_samples_per_pixel);
				pixel_colors[pixel.index + 2] = to_writable_color(pixel.color.z(), inv_samples_per_pixel);
			});
	}

	raytrace_settings settings;
	raytrace_render_data current_render;
	raytrace_render_data empty_render;

private:
	/// <summary>
	/// return the color for the given raycast, using a blue-gradient sky (when the raycast returns no hit)
	/// </summary>
	static color ray_color_with_gradient_sky_attenuated(ray raycast, world& world,
	                                         const raytrace_render_settings& settings,
											 color acc_attenuation, color acc_emitted)
	{
		int depth = settings.bounce_depth;
		while (true)
		{
			hit_info hit{&lambertian_material::default_material()};
			if (!world.hit(raycast, 0.001f, constants::infinity, hit))
			{
				const float t = 0.5f * (raycast.direction.y() + 1.0f);
				return color(acc_emitted + mul(acc_attenuation, color(((1.0f - t) * settings.background_bottom_color + t * settings.background_top_color) * settings.background_strength)));
			}
			
			color attenuation;
			ray scattered;
			const color emitted = hit.material->emitted(hit.uv_coordinates, hit.point);
			if (hit.material->scatter(raycast, hit, attenuation, scattered))
			{
				raycast = scattered;
				acc_attenuation = color(mul(acc_attenuation, attenuation));
				acc_emitted = color(acc_emitted + mul(acc_attenuation, emitted));
				depth = depth - 1;
				if (depth < 1)
				{
					return color(acc_emitted + mul(acc_attenuation, settings.bounce_depth_limit_color));
				}
				continue;
			}

			return color(acc_emitted + mul(acc_attenuation, emitted));
		}
	}
};

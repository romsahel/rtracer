#pragma once
#include <chrono>
#include <execution>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "camera.h"
#include "thread_pool.h"
#include "world.h"
#include "core/color.h"
#include "materials/lambertian_material.h"

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

struct raytrace_settings
{
	raytrace_settings(int image_width, int image_height)
		: image_width(image_width)
	,		  image_height(image_height)
	,		  inv_image_width(1.0f / static_cast<float>(image_width - 1))
	,		  inv_image_height(1.0f / static_cast<float>(image_height - 1))
	{
	}

	int bounce_depth = 12;
	color bounce_depth_limit_color = color::black();

	color background_bottom_color = color::white();
	color background_top_color = color(0.5f, 0.7f, 1.0f);
	float background_strength = 1.0f;

	const int image_width;
	const int image_height;

	const float inv_image_width;
	const float inv_image_height;

	bool use_bvh = true;
};

/// <summary>
/// represent the result of a render
/// an instance of this class is to be given to the raytrace_renderer to produce an image
/// </summary>
struct raytrace_render_data
{
	explicit raytrace_render_data(const raytrace_settings& settings)
		: settings(settings)
	{
	}

	void reset(const raytrace_render_data& empty_render)
	{
		iteration = 1.0f;
		set_pixels_from(empty_render);
	}

	void set_pixels_from(const raytrace_render_data& src)
	{
		std::memcpy(pixels.data(), src.pixels.data(), pixels.size() * sizeof(raytrace_pixel));
	}

	void set_colors_from(const raytrace_render_data& src)
	{
		std::memcpy(colors.data(), src.colors.data(), colors.size() * sizeof(unsigned char));
	}
	
	// the pixels of the image (see raytrace_settings for the dimension of the vector)
	std::vector<raytrace_pixel> pixels;

	// the colors of the image (it is a conversion of all the colors from pixels
	//							into an image-format array of ascii colors, used to write into a image-file or a opengl texture buffer)
	std::vector<unsigned char> colors;

	// the current iteration of the render (since we use progressive rendering in order to have responsive feedback)
	float iteration = 1.0f;
	// the target iteration of the render (at which point we stop rendering)
	float target_iteration = 1000.0f;

	// if true, we render every other pixel from one render to another to be even more responsive when the scene changes
	bool extra_progressive = true;

	// settings specific for this render: see raytrace_settings
	raytrace_settings settings;

	// timing of the last render
	long long last_render_duration = 0;
};

/// <summary>
/// Describe a render command 
/// </summary>
struct raytrace_render_command
{
	raytrace_render_command(const camera& camera, const world& world, raytrace_render_data& data)
		: camera(camera),
		  world(world),
		  data(data)
	{
	}

	const camera& camera;
	const world& world;
	raytrace_render_data& data;
};

struct raytrace_render_thread
{
	static constexpr size_t thread_count = 8;
	thread_pool pool{thread_pool(thread_count)};
	bool is_alive{false};
	std::thread thread;
	std::deque<std::shared_ptr<raytrace_render_command>> commands;

	explicit raytrace_render_thread()
	{
	}

	void request_render(const camera& camera, world& world, raytrace_render_data& data)
	{
		bool already_rendering = false;
		for (auto& cmd : commands)
		{
			if (&cmd->data == &data)
			{
				already_rendering = true;
				break;
			}
		}

		if (!already_rendering)
		{
			commands.emplace_back(std::make_shared<raytrace_render_command>(camera, world, data));
			if (!is_alive)
			{
				if (thread.joinable())
					thread.join();
				thread = std::thread(&raytrace_render_thread::loop, this);
			}
		}
	}

	void loop()
	{
		is_alive = true;
		while (!commands.empty() && is_alive)
		{
			std::shared_ptr<raytrace_render_command> cmd = commands.front();
			commands.pop_front();
			if (!render(cmd->camera, cmd->world, cmd->data) && is_alive)
			{
				commands.push_back(cmd);
			}
		}
		is_alive = false;
	}

	void pause()
	{
		is_alive = false;
	}

	void interrupt()
	{
		if (is_alive)
		{
			pause();
			thread.join();
			pool.interrupt();
		}
	}

	void resume()
	{
		if (!is_alive)
		{
			thread.join();
			thread = std::thread(&raytrace_render_thread::loop, this);
		}
	}

	void clear()
	{
		is_alive = false;
		commands.clear();
		thread.join();
		pool.terminate();
	}

	/// <summary>
	/// render to a custom render_data using a custom ray_color_provider.
	/// Default one is ray_color_with_gradient_sky
	/// </summary>
	bool render(const camera& camera, const world& world, raytrace_render_data& data)
	{
		auto chrono_start = std::chrono::high_resolution_clock::now();

		int it_by_frame = 1;
		
		// render settings
		const float inv_samples_per_pixel = 1.0f / static_cast<float>(static_cast<int>(data.iteration) + it_by_frame - 1);
		const raytrace_settings& render_settings = data.settings;
		std::vector<unsigned char>& pixel_colors = data.colors;

		const size_t pixel_count = data.pixels.size();
		int increment = 1;

		const auto process_pixel = [&pixel_colors, &world, &camera, render_settings,
				inv_samples_per_pixel,
				inv_width{render_settings.inv_image_width}, inv_height{render_settings.inv_image_height},
				it_by_frame, &pixels{data.pixels}](raytrace_pixel& pixel)
		{
			for (int i = 0; i < it_by_frame; i++)
			{
				const float u = (pixel.x + random::static_float.get()) * inv_width;
				const float v = (pixel.y + random::static_float.get()) * inv_height;
				pixel.color = color(pixel.color
					+ ray_color_with_gradient_sky_attenuated(camera.compute_ray_to(u, v), world,
					                                         render_settings, color::white(),
					                                         color::black()));
			}

			// convert pixels[j].color into image-readable ascii pixel_colors
			vec3 c = sqrt(inv_samples_per_pixel * pixel.color) * 255.0f;
			for (int i = 0; i < 3; i++)
				pixel_colors[pixel.index + static_cast<long>(i)] = static_cast<unsigned char>(clamp(
					c[i], 0.0f, 255.0f));
		};
		
		if (!data.extra_progressive || (data.iteration - 10) > 0.2f)
		{
			std::for_each(std::execution::par, data.pixels.begin(), data.pixels.end(), process_pixel);
		}
		else
		{
			increment = 3;
			const int offset = static_cast<int>(data.iteration) % increment;
			const size_t nb = pixel_count / thread_count;
			auto f = [process_pixel, increment, &pixels{data.pixels}, &is_alive{is_alive}]
			(size_t start, size_t end)
			{
				for (size_t j = start; j < end && is_alive; j += increment)
				{
					process_pixel(pixels[j]);
				}
			};

			for (size_t i = 0; i < thread_count; i++)
			{
				pool.async(f, static_cast<size_t>(i * nb + offset), std::min((i + 1) * nb + offset, pixel_count));
			}
			pool.wait();
		}
		
		data.iteration += static_cast<float>(it_by_frame) / static_cast<float>(increment);

		const auto chrono_stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_stop - chrono_start);
		data.last_render_duration = duration.count();

		return data.iteration >= data.target_iteration;
	}

	/// <summary>
	/// return the color for the given raycast, using a blue-gradient sky (when the raycast returns no hit)
	/// </summary>
	static color ray_color_with_gradient_sky_attenuated(ray raycast, const world& world,
	                                                    const raytrace_settings& settings,
	                                                    color acc_attenuation, color acc_emitted)
	{
		int depth = settings.bounce_depth;
		while (true)
		{
			hit_info hit{&lambertian_material::default_material()};
			if (!world.hit(raycast, 0.001f, constants::infinity, hit))
			{
				const float t = 0.5f * (raycast.direction.y + 1.0f);
				return color(acc_emitted + (acc_attenuation *
				                               color(((1.0f - t) * settings.background_bottom_color + t * settings.
					                               background_top_color) * settings.background_strength)));
			}

			color attenuation;
			ray scattered;
			const color emitted = hit.material->emitted(hit.uv_coordinates, hit.point);
			if (hit.material->scatter(raycast, hit, attenuation, scattered))
			{
				raycast = scattered;
				acc_attenuation = color(acc_attenuation * attenuation);
				acc_emitted = color(acc_emitted + (acc_attenuation * emitted));
				depth = depth - 1;
				if (depth < 1)
				{
					return color(acc_emitted + (acc_attenuation * settings.bounce_depth_limit_color));
				}
				continue;
			}

			return color(acc_emitted + (acc_attenuation * emitted));
		}
	}
};

class raytrace_renderer
{
	const int channels_num = 3;
public:
	raytrace_renderer(int image_width, int image_height)
		: current_render(raytrace_settings{image_width, image_height})
		, empty_render(current_render.settings)
	{
		const size_t pixel_count = static_cast<size_t>(image_width) * image_height;

		current_render.pixels.reserve(pixel_count);
		empty_render.pixels.reserve(pixel_count);

		current_render.colors.reserve(pixel_count * channels_num);

		int index = 0;
		for (auto y = static_cast<float>(image_height - 1); y >= 0; y -= 1.0f)
		{
			for (float x = 0.0f; x < static_cast<float>(image_width); x += 1.0f)
			{
				current_render.pixels.emplace_back(index, x, y);
				empty_render.pixels.emplace_back(index, x, y);
				for (int i = 0; i < channels_num; ++i)
				{
					current_render.colors.emplace_back();
					empty_render.colors.emplace_back();
					index++;
				}
			}
		}
	}

	void save_to_image(const std::string& filename)
	{
		stbi_write_jpg(filename.c_str(),
		               current_render.settings.image_width, current_render.settings.image_height, channels_num,
		               current_render.colors.data(),
		               current_render.settings.image_width * channels_num);
	}

	/// <summary>
	/// signal the renderer that the scene has changed so that it resets its current render from scratch
	/// </summary>
	void signal_scene_change()
	{
		thread.interrupt();
		current_render.reset(empty_render);
	}

	/// <summary>
	/// render to the default current_render
	/// </summary>
	void render(const camera& camera, world& world)
	{
		render(camera, world, current_render);
	}

	void render(const camera& camera, world& world, raytrace_render_data& data)
	{
		thread.request_render(camera, world, data);
	}

	void clear()
	{
		thread.clear();
	}

	raytrace_render_data current_render;
	raytrace_render_data empty_render;
	raytrace_render_thread thread;
};

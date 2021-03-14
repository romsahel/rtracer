#pragma once
#include <chrono>
#include <execution>

#include "camera.h"
#include "thread_pool.h"
#include "world.h"
#include "core/color.h"
#include "materials/lambertian_material.h"

struct raytrace_settings
{
	raytrace_settings(int image_width, int image_height)
		: image_width(image_width)
	,		  image_height(image_height)
	,		  inv_image_width(1.0f / static_cast<float>(image_width - 1))
	,		  inv_image_height(1.0f / static_cast<float>(image_height - 1))
	{
	}

	const int image_width;
	const int image_height;

	const float inv_image_width;
	const float inv_image_height;

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
	float iteration = 1.0f;
	float target_iteration = 1000.0f;

	raytrace_render_settings settings;
	long long last_render_duration;

	raytrace_render_data() = default;

	raytrace_render_data(const raytrace_render_data& other)
		: pixels(other.pixels),
		  iteration(other.iteration),
		  target_iteration(other.target_iteration),
		  settings(other.settings),
		  last_render_duration(other.last_render_duration),
		  colors_front(other.colors_front),
		  colors_back(other.colors_back)
	{
	}

	void set_pixels_from(const raytrace_render_data& src)
	{
		std::memcpy(pixels.data(), src.pixels.data(), pixels.size() * sizeof(raytrace_pixel));
	}

	void swap_buffers()
	{
		//std::lock_guard<std::mutex> guard(m_mutex);
		std::swap(colors_front, colors_back);
	}

	auto& front_buffer()
	{
		//std::lock_guard<std::mutex> guard(m_mutex);
		return colors_front;
	}

	auto& back_buffer()
	{
		//std::lock_guard<std::mutex> guard(m_mutex);
		return colors_back;
	}


private:
	//std::mutex m_mutex;
	std::vector<unsigned char> colors_front;
	std::vector<unsigned char> colors_back;
};

struct raytrace_render_command
{
	raytrace_render_command(const camera& camera, world& world, raytrace_render_data& data)
		: camera(camera),
		  world(world),
		  data(data)
	{
	}

	const camera& camera;
	world& world;
	raytrace_render_data& data;
};

struct raytrace_render_thread
{
	const raytrace_settings& settings;
	bool is_alive{false};
	std::thread thread;
	std::deque<std::shared_ptr<raytrace_render_command>> commands;

	explicit raytrace_render_thread(const raytrace_settings& settings)
		: settings(settings)
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
			if (!render(cmd->camera, cmd->world, cmd->data, settings))
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

	void resume()
	{
		if (!is_alive)
		{
			thread.join();
			thread = std::thread(&raytrace_render_thread::loop, this);
		}
	}
	
	static constexpr size_t thread_count = 6;
	static inline thread_pool pool{thread_pool(thread_count)};

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
	static bool render(const camera& camera, world& world, raytrace_render_data& data,
	                   const raytrace_settings& raytrace_settings)
	{
		auto start = std::chrono::high_resolution_clock::now();

		const int it_by_frame = 1;
		
		// render settings
		const float inv_samples_per_pixel = 1.0f / static_cast<float>(static_cast<int>(data.iteration) + it_by_frame - 1);
		const raytrace_render_settings render_settings = data.settings;
		std::vector<unsigned char>& pixel_colors = data.front_buffer();

		const size_t pixel_count = data.pixels.size();
		const int offset = static_cast<int>(data.iteration) % 3;
		const int nb = pixel_count / thread_count;
		auto f = [&pixel_colors, &world, &camera, render_settings,
				inv_samples_per_pixel,
				inv_width{raytrace_settings.inv_image_width}, inv_height{raytrace_settings.inv_image_height},
				it_by_frame, &pixels{data.pixels}]
		(int start, int end)
		{
			for (int j = start; j < end; j += 3)
			{
				for (int i = 0; i < it_by_frame; i++)
				{
					const float u = (pixels[j].x + random::static_double.get()) * inv_width;
					const float v = (pixels[j].y + random::static_double.get()) * inv_height;
					pixels[j].color = color(pixels[j].color
						+ ray_color_with_gradient_sky_attenuated(camera.compute_ray_to(u, v), world,
						                                         render_settings, color::white(),
						                                         color::black()));
				}

				// convert pixels[j].color into image-readable ascii pixel_colors
				vec3 c = to_writable_color(pixels[j].color, inv_samples_per_pixel);
				for (int i = 0; i < 3; i++)
					pixel_colors[pixels[j].index + i] = static_cast<unsigned char>(clamp(c[i], 0.0f, 255.0f));
			}
		};
		for (size_t i = 0; i < thread_count; i++)
		{
			pool.async(f, i * nb + offset, std::min((i + 1) * nb + offset, pixel_count));
		}
		pool.wait();

		data.iteration += static_cast<float>(it_by_frame) / 3.0f;

		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		data.last_render_duration = duration.count();

		return data.iteration >= data.target_iteration;
	}

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
				const float t = 0.5f * (raycast.direction.y + 1.0f);
				return color(acc_emitted + mul(acc_attenuation,
				                               color(((1.0f - t) * settings.background_bottom_color + t * settings.
					                               background_top_color) * settings.background_strength)));
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

class raytrace_renderer
{
	const int channels_num = 3;
public:
	raytrace_renderer(int image_width, int image_height)
		: settings{image_width, image_height}, thread(settings)
	{
		const size_t pixel_count = static_cast<size_t>(image_width) * image_height;

		current_render.pixels.reserve(pixel_count);
		empty_render.pixels.reserve(pixel_count);

		current_render.back_buffer().reserve(pixel_count * channels_num);
		current_render.front_buffer().reserve(pixel_count * channels_num);

		int index = 0;
		for (float y = static_cast<float>(image_height - 1); y >= 0; --y)
		{
			for (float x = 0; x < static_cast<float>(image_width); ++x)
			{
				current_render.pixels.emplace_back(index, x, y);
				empty_render.pixels.emplace_back(index, x, y);
				for (int i = 0; i < channels_num; ++i)
				{
					current_render.back_buffer().emplace_back();
					current_render.front_buffer().emplace_back();
					empty_render.front_buffer().emplace_back();
					index++;
				}
			}
		}
	}

	void save_to_image(const std::string& filename)
	{
		stbi_write_jpg(filename.c_str(),
		               settings.image_width, settings.image_height, channels_num,
		               current_render.front_buffer().data(),
		               settings.image_width * channels_num);
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

	void render(const camera& camera, world& world, raytrace_render_data& data)
	{
		thread.request_render(camera, world, data);
	}

	void clear()
	{
		thread.clear();
	}

	raytrace_settings settings;
	raytrace_render_thread thread;
	raytrace_render_data current_render;
	raytrace_render_data empty_render;
};

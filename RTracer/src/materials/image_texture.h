#pragma once

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class image_texture : public texture
{
public:
	const static int bytes_per_pixel = 3;

	explicit image_texture(const char* filename)
	{
		int channels_in_file = bytes_per_pixel;
		data = stbi_load(filename, &width, &height, &channels_in_file, bytes_per_pixel);
		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture()
	{
		delete data;
	}


	color value_at(const vec3& uv_coordinates, const point3&) const override
	{
		if (data == nullptr) return color::magenta();

		float u = clamp(uv_coordinates.x(), 0, 1);
		float v = 1.0f - clamp(uv_coordinates.y(), 0, 1);

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);
		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		static const float color_scale = 1.0f / 255.0f;
		const auto pixel_data = data + j * bytes_per_scanline + i * bytes_per_pixel;
		return color(color_scale * pixel_data[0], color_scale * pixel_data[1], color_scale * pixel_data[2]);
	}

	stbi_uc* data = nullptr;
	int width = -1;
	int height = -1;
	int bytes_per_scanline = -1;
};

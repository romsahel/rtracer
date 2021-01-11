#pragma once

#include "core/gl_includer.h"

/// <summary>
/// manages an opengl texture to be renderered with imgui
/// 
/// </summary>
class gui_image
{
public:
	/// <param name="is_mask">if true, the red channel of the texture is used for the alpha</param>
	gui_image(bool is_mask)
	{
		glGenTextures(1, &m_rendererId);
		glBindTexture(GL_TEXTURE_2D, m_rendererId);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		// This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		if (is_mask)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED); // Same
		}

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// update the content of the image to display
	void update(const int image_width, const int image_height, const unsigned char* pixel_colors)
	{
		glBindTexture(GL_TEXTURE_2D, m_rendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_colors);
	}

	// TextureId to give to ImGui
	// e.g.: ImGui::Image(image.texture_id());
	ImTextureID texture_id() const
	{
#pragma warning(push)
#pragma warning(disable: 4312)
		return reinterpret_cast<void*>(m_rendererId);
#pragma warning(pop)
	}

private:
	GLuint m_rendererId = 999;
};

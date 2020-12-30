#pragma once

#include "core/gl_includer.h"

class gui_image
{
public:
	gui_image()
	{
		glGenTextures(1, &m_rendererId);
		glBindTexture(GL_TEXTURE_2D, m_rendererId);

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

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void update(const int image_width, const int image_height, const unsigned char* pixel_colors)
	{
		glBindTexture(GL_TEXTURE_2D, m_rendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
		             GL_RGB, GL_UNSIGNED_BYTE, pixel_colors);
	}

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

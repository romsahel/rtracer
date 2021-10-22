#pragma once

#include "color.hpp"
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

class Canvas
{
public:
	Canvas(GLFWwindow* window) : m_window(window)
	{
		const auto shadersDir = std::filesystem::current_path().parent_path() / "gloom" / "shaders";
		shader.makeBasicShader(shadersDir / "simple.vert", shadersDir / "simple.frag");

		static constexpr float vertices[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		updateSize();
	}

	void updateSize()
	{
		glfwGetWindowSize(m_window, &m_halfWidth, &m_halfHeight);
		m_halfWidth /= 2;
		m_halfHeight /= 2;
	}

	void start()
	{
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Then create your buffers, compile your shaders and bind your shader program and VAO.
		shader.activate();

		// seeing as we only have a single VAO there's no need to bind it every time,
		// but we'll do so to keep things a bit more organized
		glBindVertexArray(VAO);

		//
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void end()
	{
		glBindVertexArray(0);
		shader.deactivate();
	}

	void putPixel(int x, int y, const Color& color)
	{
		const GLint transformLoc = glGetUniformLocation(shader.get(), "inColor");
		glUniform4f(transformLoc, UnwrapColor(float, color));

		glEnable(GL_SCISSOR_TEST);
		glScissor(m_halfWidth + x, m_halfHeight + y, 1, 1); // position of pixel
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_SCISSOR_TEST);
	}

	GLFWwindow* m_window;
	Gloom::Shader shader;
	int m_halfWidth = -1, m_halfHeight = -1;
	unsigned int VBO = 0, VAO = 0;
};


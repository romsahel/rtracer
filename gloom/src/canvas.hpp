#pragma once

#include "color.hpp"
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"


struct Point
{
	int x, y;

	Point(int x, int y) : x(x), y(y)
	{
	}
};

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
		glBindVertexArray(VAO);
		// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//Then use:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void end()
	{
		shader.deactivate();
	}

	void putPixel(int x, int y, const Color& color)
	{
		GLint transformLoc = glGetUniformLocation(shader.get(), "inColor");
		glUniform4f(transformLoc, color.r<float>(), color.g<float>(), color.b<float>(), color.a<float>());

		glEnable(GL_SCISSOR_TEST);
		glScissor(m_halfWidth + x, m_halfHeight + y, 1, 1); // position of pixel
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_SCISSOR_TEST);
	}

	void drawLine(Point p0, Point p1, const Color& color)
	{
		if (p0.x > p1.x)
		{
			std::swap(p0, p1);
		}

		if (std::abs(p1.x - p0.x) > std::abs(p1.y - p0.y))
		{
			auto interpolator = Interpolator(p0.x, p0.y, p1.x, p1.y);
			do
			{
				putPixel(interpolator.i, static_cast<int>(interpolator.d), color);
			}
			while (!interpolator.next());
		}
		else
		{
			auto interpolator = Interpolator(p0.y, p0.x, p1.y, p1.x);
			do
			{
				putPixel(static_cast<int>(interpolator.d), interpolator.i, color);
			}
			while (!interpolator.next());
		}
	}

	struct Interpolator
	{
		Interpolator(int i0, int d0, int i1, int d1)
			: d(static_cast<float>(d0)),
			  i(i0),
			  a(static_cast<float>(d1 - d0) / static_cast<float>(i1 - i0)),
			  i1(i1)
		{
		}

		bool next()
		{
			const bool finished = i == i1;
			d += a;
			i++;
			return finished;
		}

		float d;
		int i;

	private:
		const float a;
		const int i1;
	};

	GLFWwindow* m_window;
	int m_halfWidth, m_halfHeight;
	Gloom::Shader shader;
	unsigned int VBO, VAO;
};

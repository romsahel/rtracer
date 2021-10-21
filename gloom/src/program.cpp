// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

struct Color
{
	float r;
	float g;
	float b;
	float a;


	Color(float r, float g, float b, float a)
		: r(r),
		  g(g),
		  b(b),
		  a(a)
	{
	}
};

class Canvas
{
public:
	Canvas()
	{
		shader.makeBasicShader(R"(C:\Users\Roman\source\repos\gloom\gloom\shaders\simple.vert)",
		                       R"(C:\Users\Roman\source\repos\gloom\gloom\shaders\simple.frag)");

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);
	}

	void setColor()
	{
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

	void putPixel(int x, int y, Color color)
	{
		//Your drawing code would look something like this to draw a green pixel at x = 100, y = 100:
		GLint transformLoc = glGetUniformLocation(shader.get(), "inColor");
		glUniform4f(transformLoc, color.r, color.g, color.b, color.a);

		glEnable(GL_SCISSOR_TEST);
		glScissor(x, y, 1, 1); // position of pixel
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_SCISSOR_TEST);
	}

	Gloom::Shader shader;
	unsigned int VBO, VAO;
};


void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Set default colour after clearing the colour buffer
	glClearColor(0.3f, 0.5f, 0.8f, 1.0f);

	// Set up your scene here (create Vertex Array Objects, etc.)
	Canvas canvas;

	Color colors[] = {
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.0f, 1.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f, 1.0f),
		Color(1.0f, 1.0f, 0.0f, 1.0f),
		Color(0.0f, 1.0f, 1.0f, 1.0f),
		Color(1.0f, 0.0f, 1.0f, 1.0f),
	};
	
	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		canvas.start();

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; ++j)
			{
				canvas.putPixel(0+ i, 0+ j, Color(i / 150.0f, j / 10.0f, 0.5f, 1.0f));
			}
		}
		
		canvas.end();

		glfwPollEvents();
		handleKeyboardInput(window);

		// Flip buffers
		glfwSwapBuffers(window);
	}
}


void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

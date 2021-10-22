// Local headers
#include "program.hpp"
#include "canvas.hpp"
#include "drawer.hpp"
#include "gloom/gloom.hpp"

class Scene
{
	
};

void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Set default colour after clearing the colour buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Set up your scene here (create Vertex Array Objects, etc.)
	Canvas canvas{window};
	Drawer drawer{canvas};

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		canvas.start();
		// drawing start

		drawer.drawLine(Vertex(-150, 0), Vertex(150, 0), Color::Black());
		drawer.drawLine(Vertex(0, -250), Vertex(0, 250), Color::Black());

		auto p0 = Vertex(-200, -250, 255);
		auto p1 = Vertex(200, 50, 128);
		auto p2 = Vertex(20, 250, 0);
		drawer.drawTriangle(p0, p1, p2, Color::Black(), DrawMode::Wireframe);
		drawer.drawTriangle(p0, p1, p2, Color::Red(), DrawMode::Filled);

		// drawing end
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

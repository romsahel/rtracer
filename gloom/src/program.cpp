// Local headers
#include "program.hpp"
#include "canvas.hpp"
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
	Scene;

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		canvas.start();

		canvas.drawLine(Point(-150, 0), Point(150, 0), Color(0, 0, 0, 255));
		canvas.drawLine(Point(0, -250), Point(0, 250), Color(0, 0, 0, 255));
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

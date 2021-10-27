// Local headers
#include "program.hpp"


#include "canvas.hpp"
#include "drawer.hpp"
#include "gloom/gloom.hpp"

#include "sstream.h"
#include "core/gui_initializer.h"

void operator<<(sstream& os, Camera& camera)
{
	os << "Distance" << camera.d << sstream::endl_s{0.01f};
}

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
	int canvasWidth = -1, canvasHeight = -1;
	glfwGetWindowSize(window, &canvasWidth, &canvasHeight);
	Camera camera(canvasWidth, canvasHeight, 1.0f);

	gui::initialize(window);
	sstream inspector;

	float x = 0.75f, y = 1.0f, z = 0.0f;

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		gui::start_frame();

		if (ImGui::Begin("Inspector"))
		{
			inspector << camera;
			inspector << "Position" << x << y << z << sstream::endl_s{0.01f};
		}
		ImGui::End();

		canvas.start();
		// drawing start

		drawer.drawLine(Vertex(-150, 0), Vertex(150, 0), Color::Black());
		drawer.drawLine(Vertex(0, -250), Vertex(0, 250), Color::Black());

		// The four "front" vertices
		auto vAf = Point{-2 + x, -0.5f + y, 5 + z};
		auto vBf = Point{-2 + x, 0.5f + y, 5 + z};
		auto vCf = Point{-1 + x, 0.5f + y, 5 + z};
		auto vDf = Point{-1 + x, -0.5f + y, 5 + z};
		// The four "back" vertices
		auto vAb = Point{-2 + x, -0.5f + y, 6 + z};
		auto vBb = Point{-2 + x, 0.5f + y, 6 + z};
		auto vCb = Point{-1 + x, 0.5f + y, 6 + z};
		auto vDb = Point{-1 + x, -0.5f + y, 6 + z};

		// The front face
		drawer.drawLine(camera.Project(vAf), camera.Project(vBf), Color::Blue());
		drawer.drawLine(camera.Project(vBf), camera.Project(vCf), Color::Blue());
		drawer.drawLine(camera.Project(vCf), camera.Project(vDf), Color::Blue());
		drawer.drawLine(camera.Project(vDf), camera.Project(vAf), Color::Blue());

		// The back face
		drawer.drawLine(camera.Project(vAb), camera.Project(vBb), Color::Red());
		drawer.drawLine(camera.Project(vBb), camera.Project(vCb), Color::Red());
		drawer.drawLine(camera.Project(vCb), camera.Project(vDb), Color::Red());
		drawer.drawLine(camera.Project(vDb), camera.Project(vAb), Color::Red());

		// The front-to-back edges
		drawer.drawLine(camera.Project(vAf), camera.Project(vAb), Color::Green());
		drawer.drawLine(camera.Project(vBf), camera.Project(vBb), Color::Green());
		drawer.drawLine(camera.Project(vCf), camera.Project(vCb), Color::Green());
		drawer.drawLine(camera.Project(vDf), camera.Project(vDb), Color::Green());

		//auto p0 = Vertex(-200, -250, 255);
		//auto p1 = Vertex(200, 50, 128);
		//auto p2 = Vertex(20, 250, 0);
		//drawer.drawTriangle(p0, p1, p2, Color::Black(), DrawMode::Wireframe);
		//drawer.drawTriangle(p0, p1, p2, Color::Red(), DrawMode::Filled);

		// drawing end
		canvas.end();

		handleKeyboardInput(window);

		// Flip buffers
		gui::end_frame();
		//glfwSwapBuffers(window);
	}

	gui::cleanup();
}


void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

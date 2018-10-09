#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"
#include "shaders.h"
#include "Globals.h"
#include "callbacks.h"


//Globals here
struct GlobalData Globals;

float randFloat01()
{
	return (float)rand() / (float)RAND_MAX;
}

int main(void)
{
	//Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	///glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Create a window, set context to current
	mainWindow = new Window(600, 800, "CS460 Advanced Demo");

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		assert(false);
		return -1;
	}
	glViewport(0, 0, 800, 600);
	//glDebugMessageCallback
	glDebugMessageCallback(debug_callback, nullptr);

	//Set callbacks
	glfwSetFramebufferSizeCallback(mainWindow->getGLFWwindow(), framebuffer_size_callback);
	///glfwSetKeyCallback(mainWindow, input_key_callback);

	//Create Managers
	Input = new InputManager;
	Input->Initialize();
	Input->RegisterWindow(mainWindow->getGLFWwindow());

	//make a quick shader program
	Globals.shader_program_name = create_shader_program("vert.shader", "frag.shader");

	//make a triangle
	make_triangle_VBO();

	//Main loop!
	while ( main_loop() == false );

	//Exit safely
	delete mainWindow;
	glfwTerminate();
	return 0;
}


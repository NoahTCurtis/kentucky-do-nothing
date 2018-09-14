#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"
#include "shaders.h"
#include "Globals.h"


//Globals here
struct GlobalData Globals;


float randFloat01()
{
	return (float)rand() / (float)RAND_MAX;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void debug_callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	std::cout << "Error: OpenGL reported an error from " << source << ". Type: " << type << " ID: " << id << " Severity: " << severity << std::endl;
	std::cout << message << std::endl;
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
	mainWindow = new Window(600, 800, "CS230 Advanced Demo");

	//Create Managers
	Input = new InputManager;
	Input->Initialize();
	Input->RegisterWindow(mainWindow->getGLFWwindow());

	//Set callbacks
	///glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
	///glfwSetKeyCallback(mainWindow, input_key_callback);

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);
	//glDebugMessageCallback
	glDebugMessageCallback(debug_callback, nullptr);

	//make a quick shader program
	Globals.shader_program_name = create_shader_program("vert.shader", "frag.shader");

	//make a triangle
	make_triangle_VBO();

	//Main loop!
	while ( main_loop() );

	//Exit safely
	delete mainWindow;
	glfwTerminate();
	return 0;
}


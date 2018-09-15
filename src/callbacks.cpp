#include <iostream>
#include <string>

#include "window.h"
#include "callbacks.h"


void window_size_callback(GLFWwindow* window, int width, int height)
{
	//mainWindow->set_size(width, height);
	glViewport(0, 0, width, height);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	mainWindow->set_size(width, height);
	//glViewport(0, 0, width, height);
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
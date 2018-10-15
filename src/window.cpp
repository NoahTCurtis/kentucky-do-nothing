#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"
#include "input.h"
#include "callbacks.h"

Window* mainWindow;

Window::Window(int vertical, int horizontal, const char* title)
{
	//hint what kind of window we want
	//glfwWindowHint(GLFW_DECORATED, false);

	//make new window
	window_ = glfwCreateWindow(horizontal, vertical, title, NULL, NULL);

	assert(window_ != NULL);

	glfwMakeContextCurrent(window_);

	glfwSetWindowSizeCallback(window_, window_size_callback);

	Input->RegisterWindow(window_);
}


void Window::change_title(std::string title)
{
	glfwSetWindowTitle(window_, title.c_str());
}


void Window::set_size(int width, int height)
{
	glfwSetWindowSize(window_, width, height);
	glViewport(0, 0, width, height);
}


glm::vec2 Window::get_size()
{
	int x, y;
	glfwGetWindowSize(window_, &x, &y);
	return glm::vec2(x, y);
}

glm::mat4 Window::compute_perspective_matrix(float znear, float zfar)
{
	glm::vec2 size = get_size();
	perspective_ = glm::perspective(glm::pi<float>() / 2.0f, size.x / size.y, znear, zfar);
	return perspective_;
}

glm::mat4 Window::get_perspective_matrix()
{
	return perspective_;
}
#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Window
{
public:
	Window(int vertical, int horizontal, const char* title);

	GLFWwindow* getGLFWwindow() { return window_; }

	void change_title(std::string title);

	void set_size(int width, int height);

	glm::vec2 get_size();
	glm::mat4 compute_perspective_matrix(float znear, float zfar);
	glm::mat4 get_perspective_matrix();

private:
	GLFWwindow* window_;
	glm::mat4 perspective_;
};

extern Window* mainWindow;
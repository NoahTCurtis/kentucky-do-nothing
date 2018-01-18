#pragma once

#include <GLFW\glfw3.h>

class Window
{
public:
  Window(int vertical, int horizontal, const char* title);

  GLFWwindow* GLFW() { return window_; }

private:
  GLFWwindow* window_;

  
};

extern Window* mainWindow;
///#include <cassert>

#include "window.h"
#include "input.h"

Window* mainWindow;

Window::Window(int vertical, int horizontal, const char* title)
{
  //make new window
  window_ = glfwCreateWindow(horizontal, vertical, title, NULL, NULL);

  assert(window_ != NULL);

  glfwMakeContextCurrent(window_);

  Input->RegisterWindow(window_);
}
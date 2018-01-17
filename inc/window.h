#pragma once

#include <GLFW\glfw3.h>

extern GLFWwindow* mainWindow;

class Window
{
public:
  Window(int vertical, int horizontal, const char* title);

  

private:
  GLFWwindow* window_;

  GLFWcharfun        i; void  charFunc_(GLFWwindow*, unsigned);
  GLFWcharmodsfun    i; void  charModsFunc_(GLFWwindow*, unsigned, int);
  GLFWcursorenterfun i; void  cursorEnterFunc_(GLFWwindow*, int);
  GLFWcursorposfun   i; void  cursorPosFunc_(GLFWwindow*, double, double);
  GLFWdropfun        i; void  dropFunc_(GLFWwindow*, int, const char**);
  GLFWkeyfun         i; void  keyFunc_(GLFWwindow*, int, int, int, int);
  GLFWmousebuttonfun i; void  mouseButtonFunc_(GLFWwindow*, int, int, int);
  GLFWscrollfun      i; void  scrollFunc_(GLFWwindow*, double, double);
};
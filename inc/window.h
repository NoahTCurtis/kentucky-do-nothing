#pragma once

#include <GLFW\glfw3.h>

extern GLFWwindow* mainWindow;

class Window
{
public:
  Window(int vertical, int horizontal, const char* title);

  

private:
  GLFWwindow* window_;

  /*GLFWcharfun        */  void charFunc_(GLFWwindow*, unsigned);
  /*GLFWcharmodsfun    */  void charModsFunc_(GLFWwindow*, unsigned, int);
  /*GLFWcursorenterfun */  void cursorEnterFunc_(GLFWwindow*, int);
  /*GLFWcursorposfun   */  void cursorPosFunc_(GLFWwindow*, double, double);
  /*GLFWdropfun        */  void dropFunc_(GLFWwindow*, int, const char**);
  /*GLFWkeyfun         */  void keyFunc_(GLFWwindow*, int, int, int, int);
  /*GLFWmousebuttonfun */  void mouseButtonFunc_(GLFWwindow*, int, int, int);
  /*GLFWscrollfun      */  void scrollFunc_(GLFWwindow*, double, double);
};
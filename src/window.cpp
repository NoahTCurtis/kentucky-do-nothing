#include "window.h"

GLFWwindow* mainWindow;

Window::Window(int vertical, int horizontal, const char* title)
{
  //make new window
  window_ = glfwCreateWindow(horizontal, vertical, title, NULL, NULL);

  glfwSetCharCallback        (window_, charFunc_);
  glfwSetCharModsCallback    (window_, charModsFunc_);
  glfwSetCursorEnterCallback (window_, cursorEnterFunc_);
  glfwSetCursorPosCallback   (window_, cursorPosFunc_);
  glfwSetDropCallback        (window_, dropFunc_);
  glfwSetKeyCallback         (window_, keyFunc_);
  glfwSetMouseButtonCallback (window_, mouseButtonFunc_);
  glfwSetScrollCallback      (window_, scrollFunc_);
}


//GLFWcharfun         Window::charFunc_;
//GLFWcharmodsfun     Window::charModsFunc_;
//GLFWcursorenterfun  Window::cursorEnterFunc_;
//GLFWcursorposfun    Window::cursorPosFunc_;
//GLFWdropfun         Window::dropFunc_;
//GLFWkeyfun          Window::keyFunc_;
//GLFWmousebuttonfun  Window::mouseButtonFunc_;
//GLFWscrollfun       Window::scrollFunc_;

void Window::charFunc_(GLFWwindow* win, unsigned u)
{

}
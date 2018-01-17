#include "window.h"

GLFWwindow* mainWindow;

Window::Window(int vertical, int horizontal, const char* title)
{
  //make new window
  window_ = glfwCreateWindow(horizontal, vertical, title, NULL, NULL);

  ///glfwSetCharCallback        (window_, (GLFWcharfun       ) charFunc_);
  ///glfwSetCharModsCallback    (window_, (GLFWcharmodsfun   ) charModsFunc_);
  ///glfwSetCursorEnterCallback (window_, (GLFWcursorenterfun) cursorEnterFunc_);
  ///glfwSetCursorPosCallback   (window_, (GLFWcursorposfun  ) cursorPosFunc_);
  ///glfwSetDropCallback        (window_, (GLFWdropfun       ) dropFunc_);
  ///glfwSetKeyCallback         (window_, (GLFWkeyfun        ) keyFunc_);
  ///glfwSetMouseButtonCallback (window_, (GLFWmousebuttonfun) mouseButtonFunc_);
  ///glfwSetScrollCallback      (window_, (GLFWscrollfun     ) scrollFunc_);
}


//GLFWcharfun         Window::charFunc_;
//GLFWcharmodsfun     Window::charModsFunc_;
//GLFWcursorenterfun  Window::cursorEnterFunc_;
//GLFWcursorposfun    Window::cursorPosFunc_;
//GLFWdropfun         Window::dropFunc_;
//GLFWkeyfun          Window::keyFunc_;
//GLFWmousebuttonfun  Window::mouseButtonFunc_;
//GLFWscrollfun       Window::scrollFunc_;
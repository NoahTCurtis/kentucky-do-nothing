#include <GLFW\glfw3.h>
///#include <GL\GL.h>
///#include <GL\glew.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"


///float randFloat01()
///{
///  return (float)rand() / (float)RAND_MAX;
///}


bool main_loop(unsigned shader_program)
{
  Input->Update(0);

  static int frame = 1;
  //std::cout << std::hex << frame++ << '\r';

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);



  

  glfwSwapBuffers(mainWindow->getGLFWwindow());
  return Input->IsTriggered(Keys::Escape) || !glfwWindowShouldClose(mainWindow->getGLFWwindow());
}
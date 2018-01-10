#include <GLFW\glfw3.h>
///#include <GL\GL.h>
///#include <GL\glew.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <main_loop.h>
#include <input.h>

 



///float randFloat01()
///{
///  return (float)rand() / (float)RAND_MAX;
///}



bool main_loop(GLFWwindow* window, unsigned shader_program)
{
  glfwPollEvents();

  static int frame = 1;
  std::cout << frame++ << '\r';

  glColor3f(1, 0, 0);
  glDrawBuffer(GL_TRIANGLES);
  

  glfwSwapBuffers(window);
  return !someKeyPressed && !glfwWindowShouldClose(window);
}
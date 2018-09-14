#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <cstdio>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"
#include "shaders.h"
#include "Globals.h"


//Globals here
struct GlobalData Globals;


float randFloat01()
{
  return (float)rand() / (float)RAND_MAX;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}


int main(void)
{
  //Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  ///glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  //Create a window, set context to current
  mainWindow = new Window(600, 800, "yo whats up");

  //Create Managers
  Input = new InputManager;
  Input->Initialize();
  Input->RegisterWindow(mainWindow->getGLFWwindow());

  //Set callbacks
  ///glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
  ///glfwSetKeyCallback(mainWindow, input_key_callback);

  //Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, 800, 600);

  //make a quick shader program
  Globals.shader_program_d = create_shader_program();

  //make a triangle
  make_triangle_VBO();

  //Main loop!
  while ( main_loop() );

  //Exit safely
  delete mainWindow;
  glfwTerminate();
  return 0;
}


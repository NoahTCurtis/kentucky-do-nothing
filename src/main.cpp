#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <cstdio>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"


float randFloat01()
{
  return (float)rand() / (float)RAND_MAX;
}


unsigned int create_shader_program()
{
  char* vert_shader_src = "\
    #version 330 core\
    layout(location = 0) in vec3 aPos;\
    \
    void main()\
    {\
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\
    }\
  ";

  char* frag_shader_src = "\
    #version 330 core\
    out vec4 FragColor;\
    \
    void main()\
    {\
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\
    }\
  ";

  unsigned int VBO_d;
  glGenBuffers(1, &VBO_d);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_d);

  const int tricount = 10;
  float* vertices = new float[tricount * 9];
  for (int i = 0; i < tricount * 9; i++)
    vertices[i] = 2.0f * randFloat01() - 0.5f;

  glBufferData(GL_ARRAY_BUFFER, tricount * 9, vertices, GL_STATIC_DRAW);

  unsigned int vert_shader_d;
  vert_shader_d = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader_d, 1, &vert_shader_src, NULL);
  glCompileShader(vert_shader_d);

  unsigned int frag_shader_d;
  frag_shader_d = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader_d, 1, &frag_shader_src, NULL);
  glCompileShader(frag_shader_d);

  unsigned int shader_prog;
  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, vert_shader_d);
  glAttachShader(shader_prog, frag_shader_d);
  glLinkProgram(shader_prog);

  glUseProgram(shader_prog);

  glDeleteShader(vert_shader_d);
  glDeleteShader(frag_shader_d);

  delete[] vertices;

  return shader_prog;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}


int main(void)
{
  //Create Managers
  Input = new InputManager;
  Input->Initialize();

  //Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  ///glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  //Create a window, set context to current
  mainWindow = new Window(800, 600, "yo whats up");

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
  unsigned shader_program = create_shader_program();

  //Main loop!
  while ( main_loop(shader_program) );

  //Exit safely
  delete mainWindow;
  glfwTerminate();
  return 0;
}


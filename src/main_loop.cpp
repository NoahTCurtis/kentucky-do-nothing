#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "Globals.h"
#include "window.h"


///float randFloat01()
///{
///  return (float)rand() / (float)RAND_MAX;
///}

void make_triangle_VBO()
{
	//Generate and bind VAO
	glGenVertexArrays(1, &Globals.VAO_name);
	glBindVertexArray(Globals.VAO_name);
	//Generate and bind VBO
	glGenBuffers(1, &Globals.VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, Globals.VBO_name);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	//allocate a buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

bool main_loop()
{
  Input->Update(0);

  static int frame = 1;
  //std::cout << std::hex << frame++ << '\r';

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);


  glUseProgram(Globals.shader_program_d);
  glBindVertexArray(Globals.VAO_name);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  

  glfwSwapBuffers(mainWindow->getGLFWwindow());
  return Input->IsTriggered(Keys::Escape) || !glfwWindowShouldClose(mainWindow->getGLFWwindow());
}
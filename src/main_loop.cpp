#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "main_loop.h"
#include "input.h"
#include "Globals.h"
#include "window.h"
#include "clock.h"
#include "shaders.h"


///float randFloat01()
///{
///	return (float)rand() / (float)RAND_MAX;
///}

float vertices[] = {
	0.5f,   0.5f, 0.0f,  1.0, 0.0, 0.0, // top right
	0.5f,  -0.5f, 0.0f,  0.0, 1.0, 0.0, // bottom right
	-0.5f, -0.5f, 0.0f,  0.0, 0.0, 1.0, // bottom left
	-0.5f,  0.5f, 0.0f,  0.0, 0.0, 0.0  // top left 
};
unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};


void make_triangle_VBO()
{
	//Generate and bind VAO
	glGenVertexArrays(1, &Globals.VAO_name);
	glBindVertexArray(Globals.VAO_name);
	
	//Generate and bind and allocate/fill the VBO
	glGenBuffers(1, &Globals.VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, Globals.VBO_name);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//Generate and bind and allocate/fill the EBO
	glGenBuffers(1, &Globals.EBO_name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Globals.EBO_name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Inform OpenGL of what the vert data is
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}



bool main_loop()
{
	//Reload Shaders
	if (Input->IsTriggered(Keys::F9))
	{
		glUseProgram(0);
		glDeleteProgram(Globals.shader_program_name);
		Globals.shader_program_name = create_shader_program("vert.shader", "frag.shader");
	}

	//update systems
	Input->Update(0);
	clock.recompute_delta_time();

	//reanem window
	std::stringstream ss;
	ss << clock.fps() << " (" << (int)(1.0f / clock.dt()) << ")";
	mainWindow->change_title(ss.str());

	//clear screen
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//try to move box
	for (int i = 0; i < sizeof(vertices) / sizeof(float); i++)
	{
		if (Input->IsDown(Keys::W))
			vertices[i * 3 + 1] += 0.01f;
		if (Input->IsDown(Keys::S))
			vertices[i * 3 + 1] -= 0.01f;
		if (Input->IsDown(Keys::D))
			vertices[i * 3 + 0] += 0.01f;
		if (Input->IsDown(Keys::A))
			vertices[i * 3 + 0] -= 0.01f;
	}
	glBindVertexArray(Globals.VAO_name);
	glBindBuffer(GL_ARRAY_BUFFER, Globals.VBO_name);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	//Mess with the shape color
	float greenValue = (sin((float)clock.time()) / 2.0f) + 0.5f;
	int vertexColorLocation = glGetUniformLocation(Globals.shader_program_name, "uShapeColor");
	glUseProgram(Globals.shader_program_name);
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

	///glUseProgram(Globals.shader_program_name)
	///glUniformMatrix4fv(

	//Draw
	glUseProgram(Globals.shader_program_name);
	glBindVertexArray(Globals.VAO_name);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	

	glfwSwapBuffers(mainWindow->getGLFWwindow());
	glFlush();
	return Input->IsTriggered(Keys::Escape) || !glfwWindowShouldClose(mainWindow->getGLFWwindow());
}
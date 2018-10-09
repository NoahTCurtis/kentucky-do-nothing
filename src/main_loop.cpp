#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#include "camera.h"


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
	Camera::get()->Update();

	//reanem window
	std::stringstream ss;
	ss << clock.fps() << " (" << (int)(1.0f / clock.dt()) << ")";
	mainWindow->change_title(ss.str());

	//clear screen
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//push world2cam to vert shader
	glm::mat4 world2camMat = Camera::get()->get_world_to_camera_matrix();
	glUniformMatrix4fv(glGetUniformLocation(Globals.shader_program_name, "world2cam"), 1, GL_FALSE, &world2camMat[0][0]);

	//push cam2persp to vert shader
	glm::mat4 perspectiveMat = mainWindow->compute_perspective_matrix(0.1, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(Globals.shader_program_name, "perspective"), 1, GL_FALSE, &perspectiveMat[0][0]);


	//Draw
	glUseProgram(Globals.shader_program_name);
	glBindVertexArray(Globals.VAO_name);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	

	glfwSwapBuffers(mainWindow->getGLFWwindow());
	glFlush();
	return Input->IsTriggered(Keys::Escape) || glfwWindowShouldClose(mainWindow->getGLFWwindow());
}
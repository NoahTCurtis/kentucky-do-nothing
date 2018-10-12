#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "render.h"
#include "globals.h"
#include "window.h"
#include "camera.h"
#include "shaders.h"

Renderer* Renderer::instance_;

Renderer::Renderer()
{
	//make some shaders
	Globals.mesh_shader_program_name = create_shader_program("mesh_vert.shader", "mesh_frag.shader");
	Globals.line_shader_program_name = create_shader_program("line_vert.shader", "line_frag.shader");

	glGenVertexArrays(1, &line_VAO_name);
	glBindVertexArray(line_VAO_name);
	glGenBuffers(1, &line_EBO_name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_EBO_name);
	glGenBuffers(1, &line_VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, line_VBO_name);
	//Inform OpenGL of what the vert data is
		//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
		//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Renderer* Renderer::get()
{
	if (instance_ == nullptr)
		instance_ = new Renderer();
	return instance_;
}

Renderer::~Renderer()
{
	//clear all that good stuff
	glDeleteBuffers(1, &line_VBO_name);
	glDeleteBuffers(1, &line_EBO_name);
	glDeleteVertexArrays(1, &line_VAO_name);
	glUseProgram(0);
	//fix up singleton things
	instance_ = nullptr;
}

void Renderer::render_debug_lines()
{
	//create vertex array
	int lineCount = (int)DebugLines.size();
	int vertCount = lineCount * 2;
	std::vector<GLuint> indices;
	indices.reserve(vertCount);
	for (int i = 0; i < vertCount; i++)
	{
		indices.push_back(i);
	}

	//Generate and bind VAO
	glUseProgram(Globals.line_shader_program_name);
	glBindVertexArray(line_VAO_name);

	//Generate and bind and allocate/fill the VBO
	glBindBuffer(GL_ARRAY_BUFFER, line_VBO_name);
	glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(DebugLine), &DebugLines[0], GL_STATIC_DRAW);

	//Generate and bind and allocate/fill the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_EBO_name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertCount * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//render
	glUseProgram(Globals.line_shader_program_name);
		//push world2cam to vert shader
	glm::mat4 world2camMat = Camera::get()->get_world_to_camera_matrix();
	glUniformMatrix4fv(glGetUniformLocation(Globals.line_shader_program_name, "world2cam"), 1, GL_FALSE, &world2camMat[0][0]);
		//push cam2persp to vert shader
	glm::mat4 perspectiveMat = mainWindow->compute_perspective_matrix(0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(Globals.line_shader_program_name, "perspective"), 1, GL_FALSE, &perspectiveMat[0][0]);
	
	glDrawElements(GL_LINES, vertCount, GL_UNSIGNED_INT, 0);

	//cleanup
	glUseProgram(0);
	DebugLines.clear();
}


void Renderer::add_debug_line(DebugLine dl)
{
	DebugLines.push_back(dl);
}



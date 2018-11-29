#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "vertex.h"
#include "scene_loader.h"
#include "util.h"

std::list<Mesh*> Meshes;

Mesh::Mesh()
{
	reset_transforms();
}


Mesh::Mesh(std::string filename)
{
	reset_transforms();
	create_from_obj(filename);
	name = std::string(filename.begin(), filename.end() - 4);
}


void Mesh::reset_transforms()
{
	worldTransform.v = glm::vec3(0, 0, 0);
	worldTransform.s = glm::vec3(1, 1, 1);
	worldTransform.q = glm::quat(1,0,0,0);
}


Mesh::~Mesh()
{
	glBindVertexArray(VAO_name);
	glDeleteBuffers(1, &VBO_name);
	glDeleteBuffers(1, &EBO_name);
	glDeleteVertexArrays(1, &VAO_name);
}


Vertex default_vertices[] = {
	{ {0.5f,   0.5f, 0.0f},  {1.0, 0.0, 0.0} }, // top right
	{ {0.5f,  -0.5f, 0.0f},  {0.0, 1.0, 0.0} }, // bottom right
	{ {-0.5f, -0.5f, 0.0f},  {0.0, 0.0, 1.0} }, // bottom left
	{ {-0.5f,  0.5f, 0.0f},  {0.0, 0.0, 0.0} }  // top left 
};
unsigned int default_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};


void Mesh::create_default_mesh()
{
	assert(VAO_name == -1);

	//Generate and bind VAO
	glGenVertexArrays(1, &VAO_name);
	glBindVertexArray(VAO_name);

	//Generate and bind and allocate/fill the VBO
	glGenBuffers(1, &VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_name);
	glBufferData(GL_ARRAY_BUFFER, sizeof(default_vertices), default_vertices, GL_STATIC_DRAW);

	//Generate and bind and allocate/fill the EBO
	glGenBuffers(1, &EBO_name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(default_indices), default_indices, GL_STATIC_DRAW);

	//Inform OpenGL of what the vert data is
		//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
		//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	vertCount = sizeof(default_vertices) / sizeof(default_vertices[0]);
	indexCount = sizeof(default_indices) / sizeof(default_indices[0]);
}


void Mesh::create_VAO_from_raw_data(std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
{
	assert(VAO_name == -1);

	//Generate and bind VAO
	glGenVertexArrays(1, &VAO_name);
	glBindVertexArray(VAO_name);

	//Generate and bind and allocate/fill the VBO
	glGenBuffers(1, &VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_name);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//Generate and bind and allocate/fill the EBO
	glGenBuffers(1, &EBO_name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	//Inform OpenGL of what the vert data is
		//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
		//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	vertCount = (unsigned)vertices.size();
	indexCount = (unsigned)indices.size();
}


glm::mat4 Mesh::get_model_to_world_matrix()
{
	glm::mat4 ident = glm::mat4(1);
	return glm::translate(ident, worldTransform.v) * glm::toMat4(worldTransform.q) * glm::scale(ident, worldTransform.s);
}

void Mesh::bind()
{
	glBindVertexArray(VAO_name);
}


void Mesh::randomize_vertex_colors(std::vector<Vertex>& verts)
{
	for (auto& vert : verts)
		vert.color = glm::vec3(randFloat01(), randFloat01(), randFloat01());
}

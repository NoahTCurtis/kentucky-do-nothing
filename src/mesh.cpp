#include "glm/gtc/matrix_transform.hpp"


#include "mesh.h"
#include "vertex.h"

std::list<Mesh*> meshes;

Mesh::Mesh()
{
	worldPosition = glm::vec3(0, 0, 0);
	create_default_mesh();
}


Mesh::Mesh(std::string filename)
{
}


Mesh::~Mesh()
{
	glBindVertexArray(VAO_name);
	glDeleteBuffers(1, &VBO_name);
	glDeleteBuffers(1, &EBO_name);
	glDeleteVertexArrays(1, &VAO_name);
}


Vertex vertices[] = {
	{ {0.5f,   0.5f, 0.0f},  {1.0, 0.0, 0.0} }, // top right
	{ {0.5f,  -0.5f, 0.0f},  {0.0, 1.0, 0.0} }, // bottom right
	{ {-0.5f, -0.5f, 0.0f},  {0.0, 0.0, 1.0} }, // bottom left
	{ {-0.5f,  0.5f, 0.0f},  {0.0, 0.0, 0.0} }  // top left 
};
unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};


void Mesh::create_default_mesh()
{
	//Generate and bind VAO
	glGenVertexArrays(1, &VAO_name);
	glBindVertexArray(VAO_name);

	//Generate and bind and allocate/fill the VBO
	glGenBuffers(1, &VBO_name);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_name);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Generate and bind and allocate/fill the EBO
	glGenBuffers(1, &EBO_name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Inform OpenGL of what the vert data is
		//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
		//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

glm::mat4 Mesh::get_model_to_world_matrix()
{
	glm::mat4 ident = glm::mat4(1);
	return glm::translate(ident, worldPosition);
}

void Mesh::bind()
{
	glBindVertexArray(VAO_name);
}

#pragma once

#include <string>
#include <list>

#include "glad/glad.h"
#include "glm/glm.hpp"

class Mesh
{
public:
	Mesh();
	Mesh(std::string filename);
	~Mesh();

	void create_default_mesh(); //(TEMP)
	void create_from_obj(std::string filename);
	glm::mat4 get_model_to_world_matrix();
	void bind();

	glm::vec3 worldPosition;
private:
	GLuint VBO_name;
	GLuint VAO_name;
	GLuint EBO_name;
};

extern std::list<Mesh*> meshes;
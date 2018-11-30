#pragma once

#include <string>
#include <list>
#include <vector>

///#include "glad/glad.h"
#include "glm/glm.hpp"
#include "assimp/scene.h"

#include "vertex.h"
#include "kdn_math.h"

class Mesh
{
public:
	Mesh();
	Mesh(std::string filename);
	~Mesh();

	void reset_transforms();
	void create_default_mesh(); //(TEMP)
	void create_from_obj(std::string filename);
	void create_VAO_from_raw_data(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
	void randomize_vertex_colors(std::vector<Vertex>& verts);
	glm::mat4 get_model_to_world_matrix();
	void bind();

	std::string name;
	aiMesh* aimesh = nullptr; //assimp mesh array
	unsigned vertCount = -1;
	unsigned indexCount = -1;
	bool visible = false;

	kdn::vqs worldTransform;
private:
	unsigned VBO_name = -1;
	unsigned VAO_name = -1;
	unsigned EBO_name = -1;

};
#pragma once

#include <list>
#include <vector>

///#include "glad/glad.h"
#include "glm/glm.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"

#include "skeleton.h"
#include "mesh.h"

typedef struct
{
	glm::vec3 start;
	glm::vec3 startcolor;
	glm::vec3 end;
	glm::vec3 endcolor;
} DebugLine;


class Renderer
{
public:
	Renderer();
	~Renderer();
	void Update();
	void add_mesh(Mesh* mesh);
	void render_meshes();
	void render_debug_lines();
	void add_debug_line(DebugLine dl);
	static Renderer* get();

	//dumb shit that shouldnt be here
	Skeleton skeleTemp;
	const aiScene* scene;
	Assimp::Importer importer;

	std::list<Mesh*> Meshes;
private:
	static Renderer* instance_;
	std::vector<DebugLine> DebugLines;

	unsigned int line_VBO_name = -1;
	unsigned int line_VAO_name = -1;
	unsigned int line_EBO_name = -1;
};

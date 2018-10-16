#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "assimp/scene.h"

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
	void render_debug_lines();
	void add_debug_line(DebugLine dl);
	static Renderer* get();

private:
	static Renderer* instance_;
	std::vector<DebugLine> DebugLines;

	GLuint line_VBO_name = -1;;
	GLuint line_VAO_name = -1;;
	GLuint line_EBO_name = -1;;
};

extern const aiScene* scene;
#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
//#include <GLFW/glfw3.h>


struct GlobalData
{
	GLuint mesh_shader_program_name;
	GLuint line_shader_program_name;
	glm::vec3 clear_color = { 0.2f, 0.3f, 0.3f };
};
extern struct GlobalData Globals;

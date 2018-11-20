#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
//#include <GLFW/glfw3.h>

#include "curve.h"

struct GlobalData
{
	GLuint mesh_shader_program_name;
	GLuint line_shader_program_name;
	glm::vec3 clear_color = { 0.2f, 0.3f, 0.3f };
	Curve curve;
	float quatExponent = 1.0f;
	bool animateOnCurve = false;
	glm::vec3 quatDebugVec = { 0.0f, 0.0f, 0.0f };
	float animationSpeed = 1.0f;
};
extern struct GlobalData Globals;

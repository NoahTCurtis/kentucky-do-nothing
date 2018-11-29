#pragma once

#include "glm/glm.hpp"

#include "curve.h"

struct GlobalData
{
	unsigned mesh_shader_program_name;
	unsigned line_shader_program_name;
	glm::vec3 clear_color = { 0.2f, 0.3f, 0.3f };
	Curve curve;
	float quatExponent = 1.0f;
	bool animateOnCurve = false;
	glm::vec3 quatDebugVec = { 0.0f, 0.0f, 0.0f };
	float animationSpeed = 1.0f;
	float moveAnimateSpeedRatio = 1.0f;
};
extern struct GlobalData Globals;

#pragma once

#include <glm\glm.hpp>

class Camera
{
public:
	Camera();

	glm::mat4 world_to_camera();

	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 look;

private:

};
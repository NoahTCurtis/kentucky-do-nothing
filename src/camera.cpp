#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "Globals.h"

Camera::Camera()
{
	position = glm::vec3(0, 0, -1);
	look = glm::vec3(0, 0, 1);
	up = glm::vec3(0, 1, 0);
}


glm::mat4 Camera::world_to_camera()
{
	return glm::lookAt(position, position + look, up);
}
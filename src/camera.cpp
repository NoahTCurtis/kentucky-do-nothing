#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "Globals.h"
#include "input.h"
#include "clock.h"

Camera* Camera::instance_ = nullptr;

Camera::Camera()
{
	position = glm::vec3(0, 0.5, 1);
	look = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
}

Camera::~Camera()
{
	if (instance_ != nullptr)
		delete instance_;
}

glm::mat4 Camera::get_world_to_camera_matrix()
{
	return glm::lookAt(position, position + look, up);
}

Camera* Camera::get()
{
	if (instance_ == nullptr)
		instance_ = new Camera();
	return instance_;
}

void Camera::Update()
{
	//YO THIS SUCKS. MAKE A BETTER CAMERA CONTROLLER

	glm::vec3 movement(Input->IsDown(Keys::D) - Input->IsDown(Keys::A),
		Input->IsDown(Keys::W) - Input->IsDown(Keys::S),
		Input->IsDown(Keys::E) - Input->IsDown(Keys::Q));

	glm::vec3 localRight = glm::normalize(glm::cross(look, up));
	glm::vec3 localUp = glm::normalize(glm::cross(localRight, look));
	glm::vec3 localLook = glm::normalize(look);

	position += movement * glm::vec3(0.5,0.5,0) * clock.dt();
	position.z *= 1.0f +  movement.z * 0.2f * clock.dt();
}

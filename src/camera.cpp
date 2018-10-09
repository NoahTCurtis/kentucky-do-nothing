#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "Globals.h"
#include "input.h"
#include "clock.h"

Camera* Camera::instance_ = nullptr;

Camera::Camera()
{
	position = glm::vec3(0, 0, 1);
	look = glm::vec3(0, 0, -1);
	up = glm::vec3(0,1, 0);
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
	glm::vec3 movement(Input->IsDown(Keys::D) - Input->IsDown(Keys::A),
		Input->IsDown(Keys::W) - Input->IsDown(Keys::S),
		Input->IsDown(Keys::E) - Input->IsDown(Keys::Q));
	position += movement * clock.dt();
	look = -position;
}

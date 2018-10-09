#pragma once

#include <glm\glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	void Update();

	static Camera* get();

	glm::mat4 get_world_to_camera_matrix();

	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 look;

private:
    static Camera* instance_;
};


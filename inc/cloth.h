#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "kdn_math.h"

class Cloth
{
public:
	Cloth(int w, int h);
	void Reset();
	void DebugDraw();
	void Update(float dt);
	void Integrate(float dt);
	inline void Constrain(int w1, int h1, int w2, int h2, float rest, int type);
	void Constrain_Structural();
	void Constrain_Shear();
	void Constrain_Bend();
	void Apply_Constraints(float strength);
	void Wind(glm::vec3 direction);
	void Collide();

	kdn::vqs modelToWorld;
	const int width;
	const int height;
	float restVertical;
	float restHorizontal;
	float restDiagonal;

	float velocityDamping = 0.01f;
	float constraintStrength = 0.0f; //DEPRECATED
	glm::vec3 gravity = glm::vec3(0, -0.1, 0);
	int substeps = 1;
	glm::vec3 springStiff = glm::vec3(10, 10, 10);
	glm::vec3 springDamp = glm::vec3(0.3f, 0.3f, 0.5f);
	glm::vec3 windDirection = glm::vec3(0, 0, -5);
	glm::vec4 sphere = glm::vec4(0.5, 0.5, 1.0, 0.2);

	std::vector<std::vector<glm::vec3>> position;
	std::vector<std::vector<glm::vec3>> velocity;
	std::vector<std::vector<glm::vec3>> acceler8;
	std::vector<std::vector<glm::vec3>> constrain;
	std::vector<std::vector<bool>> movable;
};
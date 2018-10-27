#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "assimp/quaternion.h"

namespace kdn
{
	class quat : public glm::highp_quat
	{
	public:
		quat();
		quat(const aiQuaternion& aiQ);
		glm::mat4 mat4();
	};

	class basis
	{
	public:
		glm::vec3 x;
		glm::vec3 y;
		glm::vec3 z;

		basis();
		basis& transform(glm::mat4& mat);
	private:
	};
} //end of kdn namespace
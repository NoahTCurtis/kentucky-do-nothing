#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace kdn
{
	class quat : public glm::highp_quat
	{
	};
} //end of kdn namespace
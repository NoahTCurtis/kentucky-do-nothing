#include <cassert>
#include "kdn_math.h"

kdn::quat::quat()
{
	w = 1;
	x = y = z = 0;
}

kdn::quat::quat(const aiQuaternion& aiQ)
{
	w = aiQ.w;
	x = aiQ.x;
	y = aiQ.y;
	z = aiQ.z;
}

glm::mat4 kdn::quat::mat4()
{
	return glm::toMat4(*this);
}

kdn::basis::basis()
{
	x = glm::vec3(1, 0, 0);
	y = glm::vec3(0, 1, 0);
	z = glm::vec3(0, 0, 1);
}

kdn::basis& kdn::basis::transform(glm::mat4 & mat)
{
	x = glm::vec3(mat * glm::vec4(x, 0.0));
	y = glm::vec3(mat * glm::vec4(y, 0.0));
	z = glm::vec3(mat * glm::vec4(z, 0.0));
	return *this;
}

#include <cassert>

//#include "assimp/matrix3x3.h"

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

glm::mat4 kdn::to_mat4(const aiQuaternion & aiQ)
{
	//assert(sizeof(glm::mat3) == sizeof(aiMatrix3x3t<float>));
	//aiMatrix3x3t<float> aiMat3 = aiQ.GetMatrix();
	//glm::mat3 mat3;
	//std::memcpy(&mat3[0][0], &aiMat3[0][0], sizeof(glm::mat3));
	//glm::transpose(mat3);
	//return glm::mat4(mat3);
	return glm::mat4(1);
}

#include <cassert>

//#include "assimp/matrix3x3.h"
#include "glm/gtc/matrix_transform.hpp"

#include "kdn_math.h"
#include "globals.h"
#include "input.h"

/*kdn::quat::quat()
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

kdn::quat::quat(const glm::quat & q)
{
	w = q.w;
	x = q.x;
	y = q.y;
	z = q.z;
}

glm::mat4 kdn::quat::mat4()
{
	return glm::toMat4(*this);
}*/



glm::mat4 kdn::to_mat4(const aiQuaternion & aiQ)
{
	assert(false);
	//assert(sizeof(glm::mat3) == sizeof(aiMatrix3x3t<float>));
	//aiMatrix3x3t<float> aiMat3 = aiQ.GetMatrix();
	//glm::mat3 mat3;
	//std::memcpy(&mat3[0][0], &aiMat3[0][0], sizeof(glm::mat3));
	//glm::transpose(mat3);
	//return glm::mat4(mat3);
	return glm::mat4(1);
}

kdn::vqs::vqs()
{
	v = glm::vec3(0, 0, 0);
	q.w = 1;
	q.x = q.y = q.z = 0;
	s = glm::vec3(1, 1, 1);
}

kdn::vqs::vqs(glm::vec3& v_, glm::quat& q_, glm::vec3& s_)
	: v(v_), q(q_), s(s_)
{
}

glm::mat4 kdn::vqs::operator*(const glm::mat4& m)
{
	return toMat4() * m;
}

glm::mat4 kdn::vqs::toMat4()
{
	glm::mat4 i(1);
	return glm::translate(i, v) * glm::toMat4(q) * glm::scale(i, s);
}

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "assimp/quaternion.h"

namespace kdn
{
	glm::mat4 to_mat4(const aiQuaternion& aiQ);


	class quat : public glm::highp_quat
	{
	public:
		quat();
		quat(const aiQuaternion& aiQ);
		quat(const glm::quat& q);
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


	class vqs
	{
	public:
		glm::vec3 v;
		glm::quat q;
		glm::vec3 s;
		vqs();
		vqs(glm::vec3& v_, glm::quat& q_, glm::vec3& s_);
		glm::mat4 operator*(const glm::mat4& m);
		glm::mat4 toMat4();
	};


	template<typename T>
	class Bezier
	{
	public:
		Bezier() {}
		Bezier(T a, T b, T c, T d) : p0(a), p1(b), p2(c), p3(d) {}

		T Get(float t)
		{
			float r = 1.0f - t;
			return
				p0 * r * r * r +
				p1 * 3.0f * r * r * t +
				p2 * 3.0f * r * t * t +
				p3 * t * t * t;
		}

		T Tangent(float t)
		{
			float omt = 1.0f - t;
			float omt2 = omt * omt;
			float t2 = t * t;
			return
				p0 * (-omt2) +
				p1 * (3.0f * omt2 - 2.0f * omt) +
				p2 * (-3.0f * t2 + 2.0f * t) +
				p3 * (t2);
		}

		T Normal(float t, glm::vec3 up = { 0,1,0 })
		{
			T tan = Tangent(t);
			T binormal = glm::cross(up, tan);
			return glm::cross(tan, binormal);
		}

		T p0, p1, p2, p3;
	};
} //end of kdn namespace
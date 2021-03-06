#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "assimp/quaternion.h"

namespace kdn
{
	glm::mat4 to_mat4(const aiQuaternion& aiQ);
	glm::vec3 normalize(const glm::vec3& v);

	/*class quat : public glm::highp_quat
	{
	public:
		quat();
		quat(const aiQuaternion& aiQ);
		quat(const glm::quat& q);
		glm::mat4 mat4();
	};//*/


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

	template <typename T>
	T unterpolate(const T& a, const T& b, const T& c)
	{
		return (c - a) / (b - a);
	}

	template <typename T>
	T wrap(const T& min, const T& max, T val)
	{
		if (val > max)
			val -= max - min;
		else if (val < min)
			val += max - min;
		return val;
	}

	template <typename T>
	T cap(const T& min, const T& max, const T& val)
	{
		if (val > max)
			val = max;
		else if (val < min)
			val = min;
		return val;
	}
} //end of kdn namespace
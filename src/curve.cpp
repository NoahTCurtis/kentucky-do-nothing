#include <cassert>

#include "util.h"
#include "input.h"
#include "curve.h"
#include "render.h"

Curve::Curve()
{
	for (int i = 0; i < 4; i++)
		push();
	points[0] = glm::vec3( 10, 1,  10);
	points[1] = glm::vec3(-10, 1,  10);
	points[2] = glm::vec3(-10, 1, -10);
	points[3] = glm::vec3( 10, 1, -10);
}

glm::vec3 Curve::operator()(float t)
{
	if(Input->IsDown(Keys::F))
		return glm::vec3(t, fade(t), 0);
	if(Input->IsDown(Keys::P))
		return glm::vec3(t, sinEase01(sinEase01(t)), 0);

	int ptCount = (int)points.size();
	auto wrap = [&](int i) { return (i + ptCount) % ptCount; };

	if(ptCount == 0)
		return glm::vec3();
	if (ptCount == 1)
		return points[0];
	else
	{
		float tScaled = t * (float)ptCount;
		int tP = (int)tScaled;

		int tm2 = wrap(tP - 2);
		int tm1 = wrap(tP - 1);
		int tpp = wrap(tP);
		int tp1 = wrap(tP + 1);
		int tp2 = wrap(tP + 2);
		int tp3 = wrap(tP + 3);

		float tDiff = fmod(tScaled, 1.0f);
		float tLocalA = (2.0f + tDiff) / 3.0f;
		float tLocalB = (1.0f + tDiff) / 3.0f;
		float tLocalC = (0.0f + tDiff) / 3.0f;

		assert(tLocalA >= 0.0f && tLocalA <= 1.0f);
		assert(tLocalB >= 0.0f && tLocalB <= 1.0f);
		assert(tLocalC >= 0.0f && tLocalC <= 1.0f);

		glm::vec3 scalars(fade(tLocalA), fade(tLocalB), fade(tLocalC));
		scalars /= scalars.x + scalars.y + scalars.z;

		glm::vec3 A = compute(tm2, tm1, tpp, tp1, tLocalA);
		glm::vec3 B = compute(tm1, tpp, tp1, tp2, tLocalB);
		glm::vec3 C = compute(tpp, tp1, tp2, tp3, tLocalC);

		return A * scalars.x + B * scalars.y + C * scalars.z;
	}
}

glm::vec3 Curve::tangent(float t)
{
	return glm::normalize(this->operator()(t + 0.0001f) - this->operator()(t));
}

glm::vec3 Curve::move_along(float t, float dist)
{
	return glm::vec3();
}

kdn::Bezier<glm::vec3> Curve::operator[](int i)
{
	int ptCount = (int)points.size();
	auto wrap = [&](int i) { return (i + ptCount) % ptCount; };
	return kdn::Bezier<glm::vec3>(points[wrap(i)], points[wrap(i+1)], points[wrap(i+2)], points[wrap(i+3)]);
}

glm::vec3 Curve::compute(int a, int b, int c, int d, float t)
{
		float r = 1.0f - t;
		int ps = (int)points.size();
		return
			points[a % ps] * r * r * r +
			points[b % ps] * 3.0f * r * r * t +
			points[c % ps] * 3.0f * r * t * t +
			points[d % ps] * t * t * t;
}

float Curve::fade(float t)
{
	float r;
	if (t < 1.0f / 3.0f)
	{
		r = t * 3.0f;
	}
	else if (t > 2.0f / 3.0f)
	{
		r = (t - 1.0f) * -3.0f;
	}
	else r = 1.0f;

	if (useSmoothness)
		return sinEase010(t);
	return r;
}

void Curve::push()
{
	float range = 5;
	float x = randRange(-range, range);
	float y = randRange(-range, range);
	float z = randRange(-range, range);
	points.push_back(glm::vec3(x, y, z));
}

void Curve::pop()
{
	if (points.size() > 0)
	{
		points.pop_back();
	}
}

void Curve::DebugDraw()
{
	//draw the fuzzy curve
	DebugLine dbl;
	if (showOriginalCurves)
	{
		int bezCount = (int)points.size(); //- 3;
		for (int c = 0; c < bezCount; c++)
		{
			auto bez = (*this)[c];
			glm::vec3 p = bez.Get(0);
			int curveSegs = lineSegments;
			for (int i = 0; i <= curveSegs; i++)
			{
				float t = (float)i / (float)curveSegs;
				dbl.start = p;
				p = bez.Get(t);
				dbl.end = p;
				dbl.startcolor = dbl.endcolor = glm::vec3(t, (float)c / (float)bezCount, 1.0f - t);
				Renderer::get()->add_debug_line(dbl);
			}
		}
	}
	else
	{
		glm::vec3 p = (*this)(0);
		int curveSegs = lineSegments * (int)points.size();
		for (int i = 0; i <= curveSegs; i++)
		{
			float t = (float)i / (float)curveSegs;
			dbl.start = p;
			p = (*this)(t);
			dbl.end = p;
			dbl.startcolor = dbl.endcolor = glm::vec3(t, 1, 1.0f - t);
			Renderer::get()->add_debug_line(dbl);
		}
	}
}

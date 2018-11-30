#include <cassert>
#include <iostream>
#include <algorithm>

#include "util.h"
#include "input.h"
#include "curve.h"
#include "render.h"
#include "kdn_math.h"

Curve::Curve()
{
	for (int i = 0; i < 4; i++)
		push();
	points[0] = glm::vec3( 4, 1,  4);
	points[1] = glm::vec3(-4, 1,  4);
	points[2] = glm::vec3(-4, 1, -4);
	points[3] = glm::vec3( 4, 1, -4);
}

glm::vec3 Curve::operator()(float t, bool normalize)
{
	t = kdn::wrap(0.0f, 1.0f, t);

	if (normalize)
		t = normalize_t(t);

	int ptCount = (int)points.size();
	auto wrap = [&](int i) { return (i + ptCount) % ptCount; };

	if(ptCount == 0)
		return glm::vec3(0);
	else if (ptCount == 1)
		return points[0];
	else if (ptCount == 2)
		return glm::mix(points[0], points[1], (t < 0.5f) ? (t * 2.0f) : ((t - 0.5) * 2.0f));
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

float Curve::normalize_t(float t)
{
	if (t == 1.0f)
		return 1.0f;

	if (tableSize == -1)
		recompute_arc_length_table();
	
	//binary search into the table
	
	auto bound = std::lower_bound(distanceTable.begin(), distanceTable.end(), t);
	int index = (int)(bound - distanceTable.begin());

	if (index == distanceTable.size() - 1)
		return 1.0f;

	float T = kdn::unterpolate(distanceTable[index], distanceTable[index + 1], t);
	float A = float(index) / float(tableSize);
	float B = float(index + 1) / float(tableSize);
	return glm::mix(A, B, T);
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
	float range = 10;
	float x = randRange(-range, range);
	float y = 0;// randRange(-range, range);
	float z = randRange(-range, range);
	points.push_back(glm::vec3(x, y, z));
	recompute_arc_length_table();
}

void Curve::pop()
{
	if (points.size() > 0)
	{
		points.pop_back();
	}
	recompute_arc_length_table();
}

void Curve::debug_draw()
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

void Curve::recompute_arc_length_table()
{
	if (points.size() == 0) return;

	//rebuild tables
	tableSize = 999 * (int)points.size();
	sampleTable.clear();
	sampleTable.reserve((int)tableSize);
	sampleTable.assign(tableSize, glm::vec3(-1));
	distanceTable.clear();
	distanceTable.reserve((int)tableSize);
	distanceTable.assign(tableSize, -1);

	//get position samples
	for (int i = 0; i < tableSize; i++)
	{
		float t = (float)i / (float)(tableSize - 1);
		sampleTable[i] = operator()(t, false);
	}

	//get arclength totals
	distanceTable[0] = 0;
	for (int i = 1; i < tableSize; i++)
	{
		distanceTable[i] = glm::length(sampleTable[i] - sampleTable[i - 1]);
		distanceTable[i] += distanceTable[i - 1];
	}

	//reduce arclength totals to [0,1]
	curveLength = distanceTable[distanceTable.size() - 1];
	for (int i = 1; i < tableSize; i++)
		distanceTable[i] /= curveLength;
	distanceTable[distanceTable.size() - 1] = 1.0f;

	if (curveLength == 0)
		curveLength = 1.0f;
}

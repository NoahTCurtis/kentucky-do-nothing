#pragma once
#include <vector>

#include "glm/glm.hpp"

#include "kdn_math.h"

class Curve
{
public:
	std::vector<glm::vec3> points;
	bool showOriginalCurves = false;
	bool useSmoothness = true;
	int lineSegments = 25;

	Curve();
	glm::vec3 operator()(float t, bool normalize = true);
	glm::vec3 tangent(float t);
	glm::vec3 move_along(float t, float dist);
	float normalize_t(float t);

	kdn::Bezier<glm::vec3> operator[](int i);
	void push();
	void pop();
	void debug_draw();
	void recompute_arc_length_table();
private:
	glm::vec3 compute(int a, int b, int c, int d, float t);
	float fade(float t);

	int tableSize = -1;
	std::vector<glm::vec3> sampleTable;
	std::vector<float> distanceTable;

};
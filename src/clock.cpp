#include <iostream>
#include <cmath>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "clock.h"

Clock clock;

void Clock::recompute_delta_time()
{
	double time = glfwGetTime();
	delta_time = time - previous_time;
	previous_time = time;

	avg_time_per_frame = avg_time_per_frame * 0.99 + delta_time * 0.01;
}

double Clock::time()
{
	return glfwGetTime();
}

float Clock::fps()
{
	return (float)(1.0 / avg_time_per_frame);
}

float Clock::dt()
{
	return static_cast<float>(glm::min(delta_time, 1.0 / 30.0));
}

double Clock::ddt()
{
	return delta_time;
}
#pragma once


class Clock
{
public:
	void recompute_delta_time();
	double time();
	float fps();
	float dt();
	double ddt();
private:
	double delta_time = 0;
	double previous_time = 0;
	double avg_time_per_frame = 0;
};

extern Clock clock;
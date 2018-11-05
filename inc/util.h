#pragma once

#include <string>

std::string get_current_working_directory(void);

std::string get_full_file_path(std::string filename, std::string offset);

float randFloat01();
float randRange(float min, float max);
float sinEase01(float t);
float sinEase010(float t);
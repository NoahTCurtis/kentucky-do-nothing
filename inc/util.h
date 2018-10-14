#pragma once

#include <string>

std::string get_current_working_directory(void);

std::string get_full_file_path(std::string filename, std::string offset);

float randFloat01();
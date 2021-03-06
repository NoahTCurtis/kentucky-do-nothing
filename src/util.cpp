#include <util.h>
#include <direct.h>
#include <fstream>
#include <ctime>
#include <cmath>

std::string get_current_working_directory(void)
{
	char buff[512];
	_getcwd(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

std::string strip_slashes(std::string str)
{
	if (str.length() <= 2)
		return str;
	auto front = str.begin();
	auto back = str.end() - 1;
	if (*front == '/' || *front == '\\')
		front += 1;
	if (*back == '/' || *back == '\\')
		back -= 1;
	return std::string(front, back + 1);
}

std::string get_full_file_path(std::string filename, std::string directory)
{
	std::string cwd = get_current_working_directory();
	std::string fullpath = cwd + "\\..\\..\\" + strip_slashes(directory) + '\\' + strip_slashes(filename);
	return fullpath;
}

float randFloat01()
{
	return (float)rand() / (float)RAND_MAX;
}

float randRange(float min, float max)
{
	return randFloat01() * (max - min) + min;
}

float sinEase01(float t)
{
	float PI = 3.1415926535f;
	return (sin(PI*(t-0.5f))+1.0f) / 2.0f;
}

float sinEase010(float t)
{
	float PI = 3.1415926535f;
	return (sin(2.0f * PI * t - PI / 2.0f) + 1.0f) / 2.0f;
}
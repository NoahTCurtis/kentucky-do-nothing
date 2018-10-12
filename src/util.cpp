#include <util.h>
#include <direct.h>
#include <fstream>


std::string get_current_working_directory(void)
{
	char buff[512];
	_getcwd(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

std::string get_full_file_path(std::string filename, std::string offset)
{
	std::string cwd = get_current_working_directory();
	std::string fullpath = cwd + "\\..\\..\\res\\" + filename;
	return fullpath;
}
#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint create_shader_program(std::string vert_shader_filename, std::string frag_shader_filename);
std::string deserialize_shader(std::string filename);
std::string get_current_working_directory();

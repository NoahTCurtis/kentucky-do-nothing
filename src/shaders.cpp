#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shaders.h>


std::string get_current_working_directory(void)
{
	char buff[512];
	_getcwd(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}


std::string deserialize_shader(std::string filename)
{
	std::string cwd = get_current_working_directory();
	std::ifstream ifs(cwd + "\\..\\..\\res\\" + filename, std::ifstream::in);
	std::string output;
	
	char c = ifs.get();
	assert(ifs.good());
	
	while (ifs.good()) {
		output += c;
		c = ifs.get();
	}
	
	ifs.close();
	return output;
}


GLuint create_shader_program(std::string vert_shader_filename, std::string frag_shader_filename)
{
	//Deserialize shaders
	std::string vert_shader_src = deserialize_shader(vert_shader_filename);
	const GLchar* vert_str = vert_shader_src.c_str();
	std::string frag_shader_src = deserialize_shader(frag_shader_filename);
	const GLchar* frag_str = frag_shader_src.c_str();

	//Create Vertex Shader
	unsigned int vert_shader_d;
	vert_shader_d = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader_d, 1, &vert_str, NULL);
	glCompileShader(vert_shader_d);

	//Create Fragment Shader
	unsigned int frag_shader_d;
	frag_shader_d = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader_d, 1, &frag_str, NULL);
	glCompileShader(frag_shader_d);

	//Create Shader Program
	unsigned int shader_prog_d;
	shader_prog_d = glCreateProgram();
	glAttachShader(shader_prog_d, vert_shader_d);
	glAttachShader(shader_prog_d, frag_shader_d);
	glLinkProgram(shader_prog_d);

	//Test for success
	GLint success;
	glGetProgramiv(shader_prog_d, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(shader_prog_d, 512, NULL, infoLog);
		std::cout << "Error: Shader program failed to link.\n" << infoLog << std::endl;
	}


	glUseProgram(shader_prog_d);

	glDeleteShader(vert_shader_d);
	glDeleteShader(frag_shader_d);

	return shader_prog_d;
}
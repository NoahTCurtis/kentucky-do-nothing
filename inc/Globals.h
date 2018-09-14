#pragma once

#include <glad/glad.h>
//#include <GLFW/glfw3.h>


struct GlobalData
{
	GLuint shader_program_d;
	GLuint VBO_name;
	GLuint VAO_name;

};
extern struct GlobalData Globals;

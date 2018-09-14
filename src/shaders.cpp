#include <iostream>

#include <glad/glad.h>
#include <GLFW\glfw3.h>

unsigned int create_shader_program()
{
	char* vert_shader_src = "\
    #version 330 core\n\
	\n\
    layout(location = 0) in vec3 aPos;\n\
    \n\
    void main()\n\
    {\n\
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
    }\n\
  ";

	char* frag_shader_src = "\
    #version 330 core\n\
    out vec4 FragColor;\n\
    \n\
    void main()\n\
    {\n\
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n\
    }\n\
  ";
	
	//Create Vertex Shader
	unsigned int vert_shader_d;
	vert_shader_d = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader_d, 1, &vert_shader_src, NULL);
	glCompileShader(vert_shader_d);

	//Create Fragment Shader
	unsigned int frag_shader_d;
	frag_shader_d = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader_d, 1, &frag_shader_src, NULL);
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
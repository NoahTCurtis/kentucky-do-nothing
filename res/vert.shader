#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vertexColor;

uniform mat4 perspective;
uniform mat4 world2cam;

void main()
{
    gl_Position = perspective * world2cam * vec4(aPos, 1.0);
    vertexColor = aColor;
}
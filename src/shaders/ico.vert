#version 330 core

layout (location = 0) in vec3 aPos;
  
out vec4 vertexColor;
out vec3 vertexPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position =  vec4(aPos, 1.0);
    vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
	vertexPos = aPos;
}
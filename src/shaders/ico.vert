#version 330 core

layout (location = 0) in vec3 aPos;
  
out vec4 vertexColor;
out vec3 vertexPos;
flat out int lodOut;
flat out float vertexCount;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform float count;
uniform vec3 color;
uniform int lod;

void main()
{
    gl_Position =  proj * view * model * vec4(aPos, 1.0);
    vertexColor = vec4(color, 1.0);
	vertexPos = aPos;
    vertexCount = count;
    lodOut = lod;
}
#version 330 core

out vec4 FragColor;
  
in vec4 vertexColor;
in vec3 vertexPos;
flat in float vertexCount;


void main()
{
	FragColor = vertexColor;
} 
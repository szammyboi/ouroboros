#version 330 core

out vec4 FragColor;
  
in vec4 vertexColor;
in vec3 vertexPos;
flat in float vertexCount;

void main()
{
	FragColor = vec4(1.0, vertexCount, 0.0, 1.0);
} 
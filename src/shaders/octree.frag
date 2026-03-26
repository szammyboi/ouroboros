#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

out vec4 FragColor;
  
in vec4 vertexColor;
in vec3 vertexPos;

void main()
{
	FragColor = vertexColor;
} 
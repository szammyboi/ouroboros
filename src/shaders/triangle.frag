#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec3 vertexPos;

void main()
{
	vec3 norm = normalize(vertexPos);
	vec3 base = vertexPos * 0.5 + 0.5;

	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // light coming from top-front-right
	float diff = max(dot(normalize(norm), lightDir), 0.0);
	vec3 color = base * diff;
	color += base * 0.2;

    FragColor = vec4(color, 1.0);
} 
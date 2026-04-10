#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
  
in vec4 vertexColor;
in vec3 vertexPos;
in vec3 vertexNormal;

flat in float vertexCount;
flat in int lodOut;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	vec3 norm = normalize(vertexNormal);

	//vec3 lightDir = normalize(vec3(0.0, 0.0, 0.0) - vertexPos);

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
	//vec3 lightDir = normalize(lightPos - vertexPos);
	//float diff = max(dot(norm, lightDir), 0.0);

	vec3 lightDir = normalize(lightPos - vertexPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 res = (ambient + diffuse) * vertexColor.xyz;

	vec3 c;
	if (lodOut == 0) c = vec3(1.0, 0.0, 0.0);
	else if (lodOut == 1) c = vec3(1.0, 0.5, 0.0);
	else if (lodOut == 2) c = vec3(1.0, 1.0, 0.0);
	else if (lodOut == 3) c = vec3(0.0, 1.0, 0.0);
	else if (lodOut == 4) c = vec3(0.0, 0.0, 1.0);
	else c = vec3(0.5, 0.0, 0.5);
    //FragColor = vec4(c, 1.0);
	FragColor = vec4(vertexColor) * 5.0;
	BrightColor = vec4(vertexColor);
	//FragColor = vec4(normalize(vertexNormal) * 0.5 + 0.5, 1.0);
	//FragColor = vec4(vertexPos * 0.5 + 0.5, 1.0);
	//FragColor = vec4(normalize(vertexNormal) * 0.5 + 0.5, 1.0);
} 
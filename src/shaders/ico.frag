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
flat in int instanceID;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform int lightCount;

struct Light
{
	mat4 transform;
	vec4 position;
	vec4 color;
};

layout(binding = 1, std430) readonly buffer lightInstanceSSBO {
	Light light_instances[];
};

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos)
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color.xyz;
	vec3 lightDir = normalize(light.position.xyz - vertexPos);
	float range = light.position.w;
	float dist = length(light.position.xyz - vertexPos);
	float attenuation = 1.0 / (dist * dist);

	float rangeFactor = clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0);
    rangeFactor *= rangeFactor;
    attenuation *= rangeFactor;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.color.xyz;

	ambient *= attenuation;
	diffuse *= attenuation;

	return (diffuse);
}

void main()
{
	vec3 norm = normalize(vertexNormal);

	// encoding light enable into vertexColor.a
	// so if vertexColor.a == 0.0, the lighting will not be applied
	
	vec3 res = vec3(1.0 - vertexColor.a, 1.0 - vertexColor.a, 1.0 - vertexColor.a);
	for(int i = 0; i < lightCount * vertexColor.a; i++)
		res += CalcPointLight(light_instances[i], norm, vertexPos);
	res *= vertexColor.xyz;
    //FragColor = vec4(c, 1.0);
	FragColor = vec4(res, 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
	//FragColor = vec4(1.0);
	//FragColor = vec4(normalize(vertexNormal) * 0.5 + 0.5, 1.0);
	//FragColor = vec4(vertexPos * 0.5 + 0.5, 1.0);
	//FragColor = vec4(normalize(vertexNormal) * 0.5 + 0.5, 1.0);
} 
#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

layout (location = 0) in vec3 aPos;
  
out vec4 vertexColor;
out vec3 vertexPos;
out vec3 vertexNormal;

flat out int lodOut;
flat out float vertexCount;

uniform mat4 proj;
uniform mat4 view;

uniform float count;
uniform vec3 color;
uniform int lod;

uniform vec3 lightPos;
uniform vec3 lightColor;

struct Light
{
	mat4 transform;
	vec4 position;
	vec4 color;
};

layout(binding = 1, std430) readonly buffer instanceSSBO {
	Light instances[];
};

void main()
{
    Light instance = instances[gl_BaseInstance + gl_InstanceID];

    gl_Position =  proj * view * instance.transform * vec4(aPos, 1.0);

    vec4 world = instance.transform * vec4(aPos, 1.0);
    vertexPos = world.xyz;

    vertexNormal = normalize(aPos);
    vertexCount = count;
    vertexColor = instance.color;
    lodOut = 0;
}
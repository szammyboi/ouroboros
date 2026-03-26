#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

layout (location = 0) in vec3 aPos;
  
out vec4 vertexColor;
out vec3 vertexPos;

uniform mat4 proj;
uniform mat4 view;

struct InstanceData
{
	mat4 transform;
	vec4 color;
};

layout(binding = 0, std430) readonly buffer instanceSSBO {
	InstanceData instances[];
};

void main()
{
    InstanceData instance = instances[gl_BaseInstance + gl_InstanceID];

    gl_Position =  proj * view * instance.transform * vec4(aPos, 1.0);
    vertexColor = instance.color;
	vertexPos = aPos;
}
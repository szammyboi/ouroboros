#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
  
out vec4 vertexColor; // specify a color output to the fragment shader
out vec3 vertexPos;

struct InstanceData
{
	mat4 transform;
    float position[3];
    float lod;
};

layout(binding = 0, std430) readonly buffer ssbo1 {
	InstanceData data[];
};


uniform mat4 perspective;

void main()
{
    InstanceData ss = data[gl_InstanceID + gl_BaseInstance];
    gl_Position = perspective * ss.transform * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
	vertexPos = aPos;
}
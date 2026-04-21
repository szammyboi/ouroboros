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

const float h = 6.626e-34;
const float c = 2.998e8;
const float k = 1.381e-23;
const float hc = h * c;
const float c2 = 2.0 * h * c * c;


float gauss(float x, float mu, float sigma1, float sigma2)
{
    float s = (x < mu) ? sigma1 : sigma2;
    float t = (x - mu) * s;
    return exp(-0.5 * t * t);
}

vec3 xyz_1931(float l)
{
    float x =  0.362 * gauss(l, 442.0, 0.0624, 0.0374)
             + 1.056 * gauss(l, 599.8, 0.0264, 0.0323)
             - 0.065 * gauss(l, 501.1, 0.0490, 0.0382);

    float y =  0.821 * gauss(l, 568.8, 0.0213, 0.0247)
             + 0.286 * gauss(l, 530.9, 0.0613, 0.0322);

    float z =  1.217 * gauss(l, 437.0, 0.0845, 0.0278)
             + 0.681 * gauss(l, 459.0, 0.0385, 0.0725);

    return vec3(x, y, z);
}

vec3 xyzToRGB(vec3 xyz)
{
    return mat3(
         3.2406, -0.9689,  0.0557,
        -1.5372,  1.8758, -0.2040,
        -0.4986,  0.0415,  1.0570
    ) * xyz;
}


float planck(float lambda_nm, float T)
{
    float l = lambda_nm * 1e-9;
    float l5 = l*l*l*l*l;
    return c2 / (l5 * (exp(hc / (l * k * T)) - 1.0));
}

vec3 blackbody(float T)
{
    vec3 xyz = vec3(0.0);
    float norm = 0.0;
    
    for (float lambda = 380.0; lambda <= 780.0; lambda += 10.0)
    {
        float B = planck(lambda, T);
        vec3 cmf = xyz_1931(lambda);
        xyz += B * cmf;
        norm += cmf.y;
    }
    xyz /= norm;
    
    return xyzToRGB(xyz);
}

vec3 tonemapStar(vec3 col, float exposure)
{
    col *= exposure;

    float lum = dot(col, vec3(0.2126, 0.7152, 0.0722));
    col *= 1.0 / (1.0 + lum);

    return pow(max(col, 0.0), vec3(1.0 / 2.2));
}

float starAttenuation(float dist_au, float luminosity)
{
    return luminosity / (dist_au * dist_au + 1e-6);
}
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos)
{
    vec3 toLight = light.position.xyz - fragPos;
    float dist = length(toLight);
    vec3 L = toLight / dist;

    float diff = max(dot(normal, L), 0.0);

    vec3 raw = blackbody(light.color.x);
    vec3 col = tonemapStar(raw, 1.2e-12);

    float attenuation = light.position.w / (dist * dist + 1e-6);

    return diff * col * attenuation;
}

void main()
{
	vec3 norm = normalize(vertexNormal);

	vec3 res = vec3(1.0 - vertexColor.a, 1.0 - vertexColor.a, 1.0 - vertexColor.a);
	for(int i = 0; i < lightCount * vertexColor.a; i++)
		res += CalcPointLight(light_instances[i], norm, vertexPos);
	res *= vertexColor.xyz;
	FragColor = vec4(res, 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
} 
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

vec3 xyzFit_1931(float wavelength_nm)
{
    float t1 = (wavelength_nm - 442.0) * ((wavelength_nm < 442.0) ? 0.0624 : 0.0374);
    float t2 = (wavelength_nm - 599.8) * ((wavelength_nm < 599.8) ? 0.0264 : 0.0323);
    float t3 = (wavelength_nm - 501.1) * ((wavelength_nm < 501.1) ? 0.0490 : 0.0382);
    float x  =  0.362 * exp(-0.5*t1*t1)
             +  1.056 * exp(-0.5*t2*t2)
             - 0.065  * exp(-0.5*t3*t3);

    float t4 = (wavelength_nm - 568.8) * ((wavelength_nm < 568.8) ? 0.0213 : 0.0247);
    float t5 = (wavelength_nm - 530.9) * ((wavelength_nm < 530.9) ? 0.0613 : 0.0322);
    float y  =  0.821 * exp(-0.5*t4*t4)
             +  0.286 * exp(-0.5*t5*t5);

    float t6 = (wavelength_nm - 437.0) * ((wavelength_nm < 437.0) ? 0.0845 : 0.0278);
    float t7 = (wavelength_nm - 459.0) * ((wavelength_nm < 459.0) ? 0.0385 : 0.0725);
    float z  =  1.217 * exp(-0.5*t6*t6)
             +  0.681 * exp(-0.5*t7*t7);

    return vec3(x, y, z);
}

// XYZ to linear sRGB matrix
vec3 xyzToRGB(vec3 xyz)
{
    return mat3(
         3.2406, -0.9689,  0.0557,
        -1.5372,  1.8758, -0.2040,
        -0.4986,  0.0415,  1.0570
    ) * xyz;
}

// Planck function — spectral radiance at wavelength (nm) and temperature (K)
float planck(float lambda_nm, float T)
{
    // Constants folded into nm units, arbitrary scale
    float l = lambda_nm * 1e-9;
    float h = 6.626e-34;
    float c = 2.998e8;
    float k = 1.381e-23;
    float l5 = l*l*l*l*l;
    return (2.0 * h * c * c) / (l5 * (exp((h*c) / (l*k*T)) - 1.0));
}

vec3 blackbodyColor(float T)
{
    vec3 xyz = vec3(0.0);
    float norm = 0.0;
    
    for (float lambda = 380.0; lambda <= 780.0; lambda += 10.0)
    {
        float B = planck(lambda, T);
        vec3 cmf = xyzFit_1931(lambda);
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
    float lumTM = lum / (1.0 + lum);
    col *= (lumTM / max(lum, 1e-6));
    return pow(max(col, 0.0), vec3(1.0 / 2.2));
}

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

	vec3 raw = blackbodyColor(light.color.x);
    float exposure = 1.2e-12; 
    vec3 col = tonemapStar(raw, exposure);
	vec3 diffuse = diff * col;

	ambient *= attenuation;
	diffuse *= attenuation;

	return (diffuse);
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
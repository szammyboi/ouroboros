// Sourced kernel from https://www.shadertoy.com/view/clBSzm

#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform float exposure;
uniform float sqrexposure;
uniform float gamma;
uniform float bloom;

uniform sampler2D previous;
uniform sampler2D original;
uniform vec2 uResolution;

vec3 expose(vec3 c)
{
    return 1.0 - exp(-exposure*(1.0+sqrexposure*c)*c);
}

vec3 saturate(vec3 c)
{
    return pow(expose(c),vec3(1.0/gamma));
}


void main()
{
    vec4 acc = texture(original, gl_FragCoord.xy/uResolution.xy);
    vec4 convol = texture(previous, 0.5*gl_FragCoord.xy/uResolution.xy);

    vec3 col = 1.0*acc.xyz/acc.w + bloom*convol.xyz;

    FragColor = vec4(saturate(col), 1.0);
}

#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D previous;
uniform float time;
uniform vec2 uResolution;

#define PI 3.14159265358979323846

uint hash(uint x) {
    x ^= x >> 16;
    x *= 0x21F0AAADu;
    x ^= x >> 15;
    x *= 0xD35A2D97u;
    x ^= x >> 15;
    return x;
}

uint rng_state;

uint rand_next() {
    rng_state = hash(rng_state);
    return rng_state;
}

float frand() {
    return uintBitsToFloat(0x3F800000u | (rand_next() >> 9)) - 1.0;
}

float poisson_rand(float lambda) {
    float m = lambda - 0.5;
    float s = sqrt(lambda);
    float x = max(frand(), 0.0001);
    float y = frand();
    return m + s * sqrt(-2.0 * log(x)) * cos(2.0 * PI * y);
}

void main() {
    vec3 col = texture(previous, uv).rgb;

    rng_state = uint(time * 3.0) + hash(floatBitsToUint(uv.x) + hash(floatBitsToUint(uv.y)));

    vec3 linear_col = pow(col, vec3(2.2));
    float luma = dot(linear_col, vec3(1.0/6.0, 2.0/6.0, 3.0/6.0));

    float strength = 0.3;
    float scale = 120.0;
    float prnd = poisson_rand(max(luma * scale, 0.0001));
    
    float grain = prnd / max(luma * scale, 0.0001);
    float mixed_grain = mix(1.0, grain, strength);

    fragColor.rgb = pow(linear_col * mixed_grain, vec3(1.0 / 2.2));
    fragColor.a = 1.0;
}
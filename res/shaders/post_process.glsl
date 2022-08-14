<VERTEX>
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 uv_coord;

void main() {
    uv_coord    = uv;
    gl_Position = vec4(pos, 0.0f, 1.0f);
}
<FRAGMENT>
#version 330 core

out vec4 FragCol;

uniform sampler2D tex;

// controllers
uniform int gamma_correction = 1;
uniform int tone_mapping = 1; // aces filmic by default

// values
uniform float gamma = 2.2f;
uniform float exposure = 0.1f;

in vec2 uv_coord;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 Uncharted2Tonemap(vec3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 exposure_map(vec3 x) {
    return vec3(1.0) - exp(-x * exposure);
}

vec3 reinhard(vec3 x) {
    return x/(x+vec3(1.0f));
}

void main() {
    vec3 result = vec3(0.0f);
    vec3 fragment = texture(tex, uv_coord).rgb;

    if (tone_mapping == 0) {
        result = fragment; // no tone mapping
    } else if (tone_mapping == 1) {
        result = ACESFilm(fragment); // aces filmic tone mapping
    } else if (tone_mapping == 2) {
        result = reinhard(fragment); // reinhard tone mapping
    } else if (tone_mapping == 3) {
        result = exposure_map(fragment); // exposure tone mapping
    } else if (tone_mapping == 4) {
        result = Uncharted2Tonemap(fragment); // uncharted 2 tone mapping
    }

    if (gamma_correction == 1) {
        FragCol = vec4(pow(result, vec3(1/gamma)), 1.0);
    } else {
        FragCol = vec4(result, 1.0f);
    }
}
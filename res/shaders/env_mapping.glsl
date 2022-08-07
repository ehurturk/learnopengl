<VERTEX>
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;
uniform mat3 model_corrected;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main() {
    gl_Position = projview * model * vec4(aPos, 1.0);
    FragPos     = vec3(model * vec4(aPos, 1.0f));
    Normal      = model_corrected * aNormal;
    TexCoords   = aTexCoords;
}
<FRAGMENT>
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1; // 0
    sampler2D texture_specular1;// 1
    sampler2D texture_emission1;// 2
    float shininess;
};

uniform vec3 cam_pos;
uniform samplerCube skybox;// 3

uniform Material material;

vec4 get_reflection() {
    vec3 I = normalize(FragPos - cam_pos);
    vec3 R = reflect(I, normalize(Normal));
    return texture(skybox, R);
}

vec4 get_refraction() {
    float ratio = 1.00 / 1.33;
    vec3 I      = normalize(FragPos - cam_pos);
    vec3 R      = refract(I, normalize(Normal), ratio);
    return texture(skybox, R);
}

void main() {

    // FragColor = mix(get_reflection(), texture(material.texture_diffuse1, TexCoords), 0.15f);
    FragColor = mix(get_reflection(), get_refraction(), 0.8);
    // FragColor = mix(FragColor, texture(material.texture_diffuse1, TexCoords), 0.15f);
}
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
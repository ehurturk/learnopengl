<VERTEX>
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;

void main() {
    TexCoords   = aTexCoords;
    gl_Position = projview * model * vec4(aPos, 1.0);
}
<FRAGMENT>
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main() {
    vec4 diffuse_tex = texture(material.texture_diffuse1, TexCoords);
    if (diffuse_tex.a < 0.5f)
        discard;
    FragColor = diffuse_tex;
}

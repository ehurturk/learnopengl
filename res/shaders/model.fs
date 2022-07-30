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

<VERTEX>
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT {
	vec2 TexCoords;
} vs_out;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;
uniform mat4 model_corrected;

void main() {
   vs_out.TexCoords   = aTexCoords;
    gl_Position = projview * model * vec4(aPos, 1.0);
}

<FRAGMENT>
#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec2 TexCoords;
} fs_in; 

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main() {
    vec4 diffuse_tex = texture(material.texture_diffuse1, fs_in.TexCoords);
    FragColor = diffuse_tex;
}

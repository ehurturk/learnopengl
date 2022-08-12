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

in vec2 uv_coord;

void main() {
    float gamma = 2.2;
    vec3 color = texture(tex, uv_coord).rgb;

    // reinhard tone mapping
    vec3 mapped = color / (color + vec3(1.0f));
    mapped = pow(mapped, vec3(1/gamma));
    FragCol = vec4(mapped, 1.0);
    // FragCol = texture(tex, uv_coord);
    // FragCol = vec4(vec3(1-texture(tex, uv_coord)), 1.0f); /* invert colors */
    // FragCol = texture(tex, uv_coord); /* invert colors */
    // FragCol = vec4(uv_coord, 1.0f, 1.0f); /* invert colors */
    // FragCol = vec4(0.3f, 0.4f, 0.5f, 1.0f);
}
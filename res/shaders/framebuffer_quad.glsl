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
    FragCol = vec4(texture(tex, uv_coord).rgb, 1.0f);
    // FragCol = vec4(vec3(1-texture(tex, uv_coord)), 1.0f); /* invert colors */
}
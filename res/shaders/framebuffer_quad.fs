#version 330 core

out vec4 FragCol;

uniform sampler2D tex;

in vec2 uv_coord;

void main() {
    FragCol = vec4(texture(tex, uv_coord).rgb, 1.0f);
    // FragCol = vec4(0.4f, 0.5f, 0.2f, 1.0f);
}
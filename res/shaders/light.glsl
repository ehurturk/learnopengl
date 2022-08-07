<VERTEX>
#version 330 core
layout(location = 0) in vec3 aPos;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;

void main() {
    gl_Position = projview * model * vec4(aPos, 1.0);
}
<FRAGMENT>
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0f);
}
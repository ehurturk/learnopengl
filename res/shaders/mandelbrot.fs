#version 330 core

out vec4 FragColor;

in vec4 gl_FragCoord;

#define MAX_ITERATIONS 500

uniform int w;
uniform int h;

uniform float zoom;
uniform float x_offset;
uniform float y_offset;

uniform vec4 color_ranges;

int get_iterations() {
    float real = ((gl_FragCoord.x / w - 0.5f) * zoom + x_offset);
    float imag = ((gl_FragCoord.y / h - 0.5f) * zoom + y_offset);

    float const_real = real;
    float const_imag = imag;

    int iter_num = 0;

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        float tmp_real = real;
        real           = (real * real - imag * imag) + const_real;
        imag           = (2.0 * tmp_real * imag) + const_imag;

        float dist = real * real + imag * imag;
        if (dist > 4.0f) {
            iter_num = i;
            break;
        }
    }
    return iter_num;
}

vec4 get_color(int iterations) {
    if (iterations == MAX_ITERATIONS) {
        gl_FragDepth = 0.0f;
        return vec4(vec3(0.0f), 1.0f);
    }

    float iter = float(iterations) / MAX_ITERATIONS;
    return vec4(0.0f, iter, 0.0f, 1.0f);
}

vec4 return_color() {
    int iter = get_iterations();
    if (iter == MAX_ITERATIONS) {
        gl_FragDepth = 0.0f;
        return vec4(vec3(0.0f), 1.0f);
    }
    float iterations = float(iter) / MAX_ITERATIONS;
    gl_FragDepth     = iterations;

    vec4 color_0 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 color_1 = vec4(0.0f, 0.4f, 0.3f, 1.0f);
    vec4 color_2 = vec4(1.0f, 0.5f, 0.1f, 1.0f);
    vec4 color_3 = vec4(1.0f, 0.0f, 0.4f, 1.0f);

    float fraction = 0.0f;
    if (iterations < color_ranges[1]) {
        fraction = (iterations - color_ranges[0]) / (color_ranges[1] - color_ranges[0]);
        return mix(color_0, color_1, fraction);
    } else if (iterations < color_ranges[2]) {
        fraction = (iterations - color_ranges[1]) / (color_ranges[2] - color_ranges[1]);
        return mix(color_1, color_2, fraction);
    } else {
        fraction = (iterations - color_ranges[2]) / (color_ranges[3] - color_ranges[2]);
        return mix(color_2, color_3, fraction);
    }
}

void main() {
    FragColor = return_color();
    // FragColor = vec4(0.3f, 0.4f, 0.5f, 1.0f);
}
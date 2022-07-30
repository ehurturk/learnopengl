#pragma once

#include "../include/Application.hpp"
#include "../include/graphics/UniformBuffer.hpp"
#include "../include/Quark.hpp"
#include "../include/Engine.hpp"

class Mandelbrot : public Application {
public:
    Mandelbrot();
    ~Mandelbrot();
    void start() override;
    void update(float dt) override;
    void imgui_update() override;

    void process_input(float dt);

private:
    Shader mandelbrot_shader;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    std::vector<float> pixel_data;
    glm::vec4 ranges;

    float zoom       = 2.5f;
    float offsets[2] = { 0.0f, 0.0f };

    int scr_width, scr_height;

    float vertices[12] = {
        //    x      y      z
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    unsigned int indices[6] = {
        //  2---,1
        //  | .' |
        //  0'---3
        0, 1, 2,
        0, 3, 1
    };

    bool my_tool_active = false;
};

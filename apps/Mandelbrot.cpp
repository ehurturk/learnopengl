#include "Mandelbrot.hpp"
#include "imgui.h"
#include <memory>

glm::vec4 find_ranges(std::vector<float> &data);

Mandelbrot::Mandelbrot() : Application("Mandelbrot", 800, 800) {
    std::cout << "sa\n";
}

Mandelbrot::~Mandelbrot() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vbo);
}

void Mandelbrot::start() {
    mandelbrot_shader.create();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    mandelbrot_shader.load_shader("../res/shaders/mandelbrot.vs", Shader::ShaderType::VERTEX_SHADER);
    mandelbrot_shader.load_shader("../res/shaders/mandelbrot.fs", Shader::ShaderType::FRAGMENT_SHADER);

    scr_width  = Engine::Get().get_subsystem<Window>()->config.width;
    scr_height = Engine::Get().get_subsystem<Window>()->config.height;

    pixel_data = std::vector<float>(scr_width * scr_height, 0.0f);
    ranges     = glm::vec4(0.0001f, 0.33333f, 0.66667f, 1.00f);
}

void Mandelbrot::update(float dt) {
    process_input(dt);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scr_width  = Engine::Get().get_subsystem<Window>()->config.width;
    scr_height = Engine::Get().get_subsystem<Window>()->config.height;

    mandelbrot_shader.use();
    mandelbrot_shader.setInt("w", scr_width);
    mandelbrot_shader.setInt("h", scr_height);
    mandelbrot_shader.setFloat("x_offset", offsets[0]);
    mandelbrot_shader.setFloat("y_offset", offsets[1]);
    mandelbrot_shader.setFloat("zoom", zoom);
    mandelbrot_shader.setVec4("color_ranges", ranges);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glReadPixels(0, 0, scr_width, scr_height, GL_DEPTH_COMPONENT, GL_FLOAT, pixel_data.data());
    ranges = find_ranges(pixel_data);
}

void Mandelbrot::imgui_update() {
    static glm::vec4 my_color;
    ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), config.title);
    ImGui::Text("FPS: %.3f", ImGui::GetIO().Framerate);
    ImGui::End();
}

glm::vec4 find_ranges(std::vector<float> &data) {
    std::sort(data.begin(), data.end());
    int lowest = 0;
    while (data[lowest] == 0.0f) {
        ++lowest;
    }

    int size         = data.size();
    int length       = size - lowest;
    glm::vec4 ranges = glm::vec4(data[lowest], data[lowest + length * 3 / 4 - 1], data[lowest + length * 7 / 8 - 1], data[size - 1]);
    return ranges;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Mandelbrot::process_input(float dt) {
    GLFWwindow *window = Engine::Get().get_subsystem<Window>()->get_raw_window();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        offsets[1] += 0.005f * zoom;
        if (offsets[1] > 1.0f)
            offsets[1] = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        offsets[1] -= 0.005f * zoom;
        if (offsets[1] < -1.0f)
            offsets[1] = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        offsets[0] += 0.005f * zoom;
        if (offsets[0] > 1.0f)
            offsets[0] = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        offsets[0] -= 0.005f * zoom;
        if (offsets[0] < -1.0f)
            offsets[0] = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        zoom = zoom * 1.02f;
        if (zoom > 4.f) {
            zoom = 4.f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        zoom = zoom * 0.98f;
        if (zoom < 0.00001f) {
            zoom = 0.00001f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        my_tool_active = true;
}
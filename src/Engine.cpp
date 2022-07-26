#include <Engine.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Model.hpp"

float dt = 0.0f;
float lt = 0.0f;

bool my_tool_active = false;

Engine::Engine(const std::string &title, ui32 width, ui32 height) : cfg((EngineConfig){.title = title, .width = width, .height = height, .full_screen = false}) {}
Engine::Engine(const std::string &title) : cfg((EngineConfig){.title = title, .full_screen = true}) {}

Engine::~Engine() {}

static void processInput(GLFWwindow *window, std::unique_ptr<Camera> &cam);

void Engine::start() {
    m_Window = std::make_unique<Window>(cfg.title, cfg.width, cfg.height);
    m_Camera = std::make_unique<Camera>(m_Window);
    // m_Window->flags |= cfg.full_screen;
    m_Window->initialize();
    m_Window->set_window_user_pointer(static_cast<void *>(m_Camera.get()));
    GLFWwindow *window = m_Window->get_raw_window();

    //     // set up vertex data (and buffer(s)) and configure vertex attributes
    //     // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

#pragma region LIGHT 1
    unsigned int lightVAO;
    unsigned int VBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
#pragma endregion

    Shader backpack_shader;
    backpack_shader.load_shader("../res/shaders/cube.vs", Shader::ShaderType::VERTEX_SHADER);
    backpack_shader.load_shader("../res/shaders/cube.fs", Shader::ShaderType::FRAGMENT_SHADER);

    Model backpack("../res/models/backpack/backpack.obj");

    Shader sponza_shader;
    sponza_shader.load_shader("../res/shaders/cube.vs", Shader::ShaderType::VERTEX_SHADER);
    sponza_shader.load_shader("../res/shaders/cube.fs", Shader::ShaderType::FRAGMENT_SHADER);

    stbi_set_flip_vertically_on_load(false);
    Model sponza("../res/models/sponza/sponza.obj");
    stbi_set_flip_vertically_on_load(true);

    Shader lightShader;
    lightShader.load_shader("../res/shaders/light.vs", Shader::ShaderType::VERTEX_SHADER);
    lightShader.load_shader("../res/shaders/light.fs", Shader::ShaderType::FRAGMENT_SHADER);

    glm::vec3 colorRed    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 colorGreen  = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 colorBlue   = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 colorYellow = glm::vec3(1.0f, 1.0f, 0.0f);

    glm::vec3 pointLightColors[] = {colorRed, colorGreen, colorBlue, colorYellow, colorYellow, colorGreen, colorBlue};

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f),
            glm::vec3(13.0f, 0.0f, 20.0f),
            glm::vec3(20.0f, 0.0f, 20.0f),
            glm::vec3(27.0f, 0.0f, 20.0f)};

    glm::vec3 backpack_positions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)};


    while (m_Window->is_window_open()) {
        // input
        // -----
        m_Window->update();
        m_Camera->update();

        processInput(window, m_Camera);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // std::cout << "pos: (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")" << std::endl;

        float cf = glfwGetTime();

        dt = cf - lt;
        lt = cf;

        {
            static glm::vec4 my_color;
            ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // Edit a color (stored as ~4 floats)
            ImGui::ColorEdit4("Color", glm::value_ptr(my_color));

            // Plot some values
            const float my_values[] = {0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f};
            ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

            // Display contents in a scrolling region
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
            ImGui::BeginChild("Scrolling");
            for (int n = 0; n < 50; n++)
                ImGui::Text("%04d: Some text", n);
            ImGui::EndChild();
            ImGui::End();
        }

        ImGui::Render();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sponza_shader.use();

        sponza_shader.setVec3("cam_pos", m_Camera->position);
        sponza_shader.setFloat("material.shininess", 32.0f);

        // Directional light settings
        sponza_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
        sponza_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
        sponza_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
        sponza_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

        // Point light settings
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            std::string pos      = "point_lights[" + std::to_string(i) + "].position";
            std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
            std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
            std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
            std::string constant = "point_lights[" + std::to_string(i) + "].constant";
            std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
            std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

            constexpr float AMBIENT_INTENSITY = 0.1f;
            sponza_shader.setVec3(pos.c_str(), pointLightPositions[i]);
            sponza_shader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
            sponza_shader.setVec3(diff.c_str(), pointLightColors[i]);
            sponza_shader.setVec3(spec.c_str(), pointLightColors[i]);
            sponza_shader.setFloat(constant.c_str(), 1.0f);
            sponza_shader.setFloat(linear.c_str(), 0.09f);
            sponza_shader.setFloat(quad.c_str(), 0.032f);
        }

        // cubeShader.setMat4("model", model); // (as already set in the loop)
        sponza_shader.setMat4("view", m_Camera->get_view_matrix());
        sponza_shader.setMat4("projection", m_Camera->get_projection_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(20, 0, 20));
        model           = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        sponza_shader.setMat4("model", model);
        sponza.draw(sponza_shader);

        backpack_shader.use();

        backpack_shader.setVec3("cam_pos", m_Camera->position);
        backpack_shader.setFloat("material.shininess", 32.0f);

        // Directional light settings
        backpack_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
        backpack_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
        backpack_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
        backpack_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

        // Point light settings
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            std::string pos      = "point_lights[" + std::to_string(i) + "].position";
            std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
            std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
            std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
            std::string constant = "point_lights[" + std::to_string(i) + "].constant";
            std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
            std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

            constexpr float AMBIENT_INTENSITY = 0.1f;
            backpack_shader.setVec3(pos.c_str(), pointLightPositions[i]);
            backpack_shader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
            backpack_shader.setVec3(diff.c_str(), pointLightColors[i]);
            backpack_shader.setVec3(spec.c_str(), pointLightColors[i]);
            backpack_shader.setFloat(constant.c_str(), 1.0f);
            backpack_shader.setFloat(linear.c_str(), 0.007f);
            backpack_shader.setFloat(quad.c_str(), 0.002f);
        }

        // cubeShader.setMat4("model", model); // (as already set in the loop)
        backpack_shader.setMat4("view", m_Camera->get_view_matrix());
        backpack_shader.setMat4("projection", m_Camera->get_projection_matrix());

        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, backpack_positions[i]);
            float angle     = 20.0f * i;
            model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model           = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            backpack_shader.setMat4("model", model);
            backpack_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));

            backpack.draw(backpack_shader);
        }

        // // 4 Point Lights
        lightShader.use();
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lightShader.setMat4("model", model);
            lightShader.setMat4("view", m_Camera->get_view_matrix());
            lightShader.setMat4("projection", m_Camera->get_projection_matrix());
            lightShader.setVec3("color", glm::vec3(pointLightColors[i]));

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        m_Window->post_update();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // TODO: Window automatically destructs itself here, so glfwTerminate() occurs here. NOTE!!
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow *window, std::unique_ptr<Camera> &cam) {
    float speed = 5.f * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed *= 1.5f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam->position += speed * cam->front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam->position -= speed * cam->front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam->position -= speed * glm::normalize(glm::cross(cam->front, cam->up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam->position += speed * glm::normalize(glm::cross(cam->front, cam->up));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        my_tool_active = true;
}

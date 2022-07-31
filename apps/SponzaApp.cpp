#include "SponzaApp.hpp"
#include <memory>

void SponzaApp::start() {
    backpack_shader.create();
    sponza_shader.create();

    backpack_shader.load_shader("../res/shaders/cube.vs", Shader::ShaderType::VERTEX_SHADER);
    backpack_shader.load_shader("../res/shaders/cube.fs", Shader::ShaderType::FRAGMENT_SHADER);

    backpack.load_model("../res/models/sphere.obj");

    sponza_shader.load_shader("../res/shaders/model.vs", Shader::ShaderType::VERTEX_SHADER);
    sponza_shader.load_shader("../res/shaders/model.fs", Shader::ShaderType::FRAGMENT_SHADER);

    stbi_set_flip_vertically_on_load(false);
    sponza.load_model("../res/models/sponza/Sponza.gltf");
    stbi_set_flip_vertically_on_load(true);

    backpack_shader.set_uniform_block_binding("Matrices", 0);
    sponza_shader.set_uniform_block_binding("Matrices", 0);
}

void SponzaApp::update(float dt) {
    process_input(dt);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sponza_shader.use();

    glm::mat4 sponza_model = glm::mat4(1.0f);
    sponza_model           = glm::translate(sponza_model, glm::vec3(0, -2.5f, 0));
    sponza_model           = glm::scale(sponza_model, glm::vec3(0.02f, 0.02f, 0.02f));
    sponza_shader.setMat4("model", sponza_model);
    sponza.draw(sponza_shader);

    backpack_shader.use();

    backpack_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    backpack_shader.setFloat("material.shininess", 32.0f);

    // Directional light settings
    backpack_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
    backpack_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
    backpack_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
    backpack_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

    // Point light settings
    for (int i = 0; i < ARR_SIZE(pointLightPositions); i++) {
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

    for (unsigned int i = 0; i < ARR_SIZE(backpack_positions); i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, backpack_positions[i]);
        float angle     = 20.0f * i;
        model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        model           = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        backpack_shader.setMat4("model", model);
        backpack_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));
        backpack.draw(backpack_shader);
    }
}

void SponzaApp::imgui_update() {
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
    ImGui::Text("FPS: %.3f", ImGui::GetIO().Framerate);

    // Plot some values
    const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
    ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

    // Display contents in a scrolling region
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
    ImGui::End();
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void SponzaApp::process_input(float dt) {
    GLFWwindow *window             = Engine::Get().get_subsystem<Window>()->get_raw_window();
    std::unique_ptr<Camera3D> &cam = Engine::Get().get_subsystem<Camera3D>();
    float speed                    = 5.f * dt;
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
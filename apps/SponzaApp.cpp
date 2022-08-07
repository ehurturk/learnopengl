#include "SponzaApp.hpp"
#include "Engine.hpp"
#include "Resource.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <memory>
#include <mutex>
#include <thread>

void SponzaApp::start() {
    skybox = new SkyBox();
    phong_shader.create();
    reflection_shader.create();
    light_shader.create();

    phong_shader.load_shader("../res/shaders/cube.glsl");
    reflection_shader.load_shader("../res/shaders/env_mapping.glsl");
    light_shader.load_shader("../res/shaders/light.glsl");

    stbi_set_flip_vertically_on_load(false);
    // earth.load_model("../res/models/globe-sphere/globe-sphere.obj");
    earth.load_model("../res/models/teapot/teapot.obj");
    stbi_set_flip_vertically_on_load(true);

    stbi_set_flip_vertically_on_load(false);
    sponza.load_model("../res/models/sponza/Sponza.gltf");
    stbi_set_flip_vertically_on_load(true);

    light_sphere.load_model("../res/models/globe-sphere/globe-sphere.obj");

    phong_shader.set_uniform_block_binding("Matrices", 0);
    light_shader.set_uniform_block_binding("Matrices", 0);

    std::array<const char *, 6> faces = {
        "../res/skyboxes/city/right.jpg",
        "../res/skyboxes/city/left.jpg",
        "../res/skyboxes/city/top.jpg",
        "../res/skyboxes/city/bottom.jpg",
        "../res/skyboxes/city/front.jpg",
        "../res/skyboxes/city/back.jpg"
    };

    stbi_set_flip_vertically_on_load(false);
    skybox->load_faces(faces);
    stbi_set_flip_vertically_on_load(true);
}

void SponzaApp::update(float dt) {
    process_input(dt);

    phong_shader.use();

    phong_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    phong_shader.setFloat("material.shininess", 32.0f);

    // Directional light settings
    phong_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
    phong_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
    phong_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
    phong_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

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
        phong_shader.setVec3(pos.c_str(), pointLightPositions[i]);
        phong_shader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
        phong_shader.setVec3(diff.c_str(), pointLightColors[i]);
        phong_shader.setVec3(spec.c_str(), pointLightColors[i]);
        phong_shader.setFloat(constant.c_str(), 1.0f);
        phong_shader.setFloat(linear.c_str(), 0.09f);
        phong_shader.setFloat(quad.c_str(), 0.032f);
    }

    glm::mat4 sponza_model = glm::mat4(1.0f);
    sponza_model           = glm::translate(sponza_model, glm::vec3(0, -2.5f, 0));
    sponza_model           = glm::scale(sponza_model, glm::vec3(0.01f, 0.01f, 0.01f));
    phong_shader.setMat4("model", sponza_model);
    phong_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(sponza_model))));
    sponza.draw(phong_shader);

    reflection_shader.use();
    reflection_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    for (unsigned int i = 0; i < ARR_SIZE(earth_positions); i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, earth_positions[i]);
        model           = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        reflection_shader.setMat4("model", model);
        reflection_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->get_id());
        reflection_shader.setInt("skybox", 1);
        earth.draw(reflection_shader);
    }

    light_shader.use();
    if (draw_lights) {
        for (unsigned int i = 0; i < ARR_SIZE(pointLightPositions); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, pointLightPositions[i]);
            model           = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            light_shader.setMat4("model", model);
            light_shader.setVec3("color", pointLightColors[i]);
            light_sphere.draw(light_shader);
        }
    }


    // NOTE: Draw the skybox last
    skybox->draw();
}

SponzaApp::~SponzaApp() {
    delete skybox;
}

void SponzaApp::imgui_update() {
#pragma region EDITOR_PANEL
    /**================================================== *
     * ==========  Editor Inspector Panel  ========== *
     * ================================================== */

    ImGui::Begin("Inspector", &my_tool_active);

    if (ImGui::TreeNode("Entities")) {
        for (int i = 0; i < ARR_SIZE(earth_positions); i++) {
            ImGui::PushID((std::string("entity_info_") + std::to_string(i)).c_str());
            ImGui::Text("Sphere %d", i + 1);
            ImGui::DragFloat3("Position", glm::value_ptr(earth_positions[i]));
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Lights")) {
        for (int i = 0; i < ARR_SIZE(pointLightPositions); i++) {
            ImGui::PushID((std::string("p_light_info_") + std::to_string(i)).c_str());
            ImGui::Text("Point Light %d", i + 1);
            ImGui::DragFloat3("Position", glm::value_ptr(pointLightPositions[i]));
            ImGui::ColorEdit4("Color", glm::value_ptr(pointLightColors[i]));
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Checkbox("Draw Lights", &draw_lights);

    ImGui::Separator();
    ImGui::Text("Controls:");
    ImGui::Text("Move with WASD");
    ImGui::Text("Rotate the camera with dragging the mouse");
    ImGui::Text("K: Disable cursor");
    ImGui::Text("J: Enable cursor");

    ImGui::End();
    /* =======  End of Editor Inspector Panel  ======= */
#pragma endregion

#pragma region STAT WINDOW
    /**================================================== *
     * ==========  Stat Window  ========== *
     * ================================================== */
    float fps = Engine::Get().get_subsystem<Window>()->get_fps(), ms = Engine::Get().get_subsystem<Window>()->get_ms_per_frame();
    static bool p_open            = true;
    static int corner             = 2;
    ImGuiIO &io                   = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1) {
        const float PAD               = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 work_pos               = viewport->WorkPos;// Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size              = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x       = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y       = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f);// Transparent background
    if (ImGui::Begin("Example: Simple overlay", &p_open, window_flags)) {
        ImGui::Text("Stats\n"
                    "(right-click to change position)");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::Text("Time per Frame (ms): %f", ms);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (&p_open && ImGui::MenuItem("Close")) p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
    /* =======  End of Stat Window  ======= */
#pragma endregion
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void SponzaApp::process_input(float dt) {
    GLFWwindow *window             = Engine::Get().get_subsystem<Window>()->get_raw_window();
    std::unique_ptr<Camera3D> &cam = Engine::Get().get_subsystem<Camera3D>();
    float speed                    = 8.f * dt;
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
}
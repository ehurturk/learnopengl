#include "NormalMap.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

NormalMap::NormalMap() : Application("NormalMap", 2560, 1600) {}
NormalMap::~NormalMap() { delete box; }

void NormalMap::start() {
    // Executed on initialization
    box = new SkyBox();
    normal_shader.create();
    light_source_shader.create();
    std::array<const char *, 6> faces = {
        "../res/skyboxes/city/right.jpg",
        "../res/skyboxes/city/left.jpg",
        "../res/skyboxes/city/top.jpg",
        "../res/skyboxes/city/bottom.jpg",
        "../res/skyboxes/city/front.jpg",
        "../res/skyboxes/city/back.jpg"
    };

    stbi_set_flip_vertically_on_load(false);
    box->load_faces(faces);
    stbi_set_flip_vertically_on_load(true);

    normal_shader.load_shader("../res/shaders/ubershader.glsl");
    normal_shader.set_uniform_block_binding("Matrices", 0);

    light_source_shader.load_shader("../res/shaders/light.glsl");
    light_source_shader.set_uniform_block_binding("Matrices", 0);

    stbi_set_flip_vertically_on_load(false);
    // car.load_model("../res/models/backpack/backpack.obj");
    car.load_model("../res/models/sponza/sponza.gltf");
    stbi_set_flip_vertically_on_load(true);

    light.load_model("../res/models/globe-sphere/globe-sphere.obj");
}

void NormalMap::update(float dt) {
    // Executed each frame
    process_input(dt);

    normal_shader.use();
    // model                     = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

    normal_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    normal_shader.setFloat("material.shininess", 32.0f);

    // Directional light settings
    normal_shader.setVec3("directional_light_dir", -0.2f, -1.0f, -0.3f);// for tangent space calculation
    normal_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
    normal_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
    normal_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

    // Point light settings
    for (int i = 0; i < ARR_SIZE(light_pos); i++) {
        std::string pos      = "light_pos[" + std::to_string(i) + "]";// for tangent space calculation
        std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
        std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
        std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
        std::string constant = "point_lights[" + std::to_string(i) + "].constant";
        std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
        std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

        constexpr float AMBIENT_INTENSITY = 0.1f;
        normal_shader.setVec3(pos.c_str(), light_pos[i]);
        normal_shader.setVec3(am.c_str(), light_color[i].r * AMBIENT_INTENSITY, light_color[i].g * AMBIENT_INTENSITY, light_color[i].b * AMBIENT_INTENSITY);
        normal_shader.setVec3(diff.c_str(), light_color[i]);
        normal_shader.setVec3(spec.c_str(), light_color[i]);
        normal_shader.setFloat(constant.c_str(), 1.0f);
        normal_shader.setFloat(linear.c_str(), 0.09f);
        normal_shader.setFloat(quad.c_str(), 0.032f);
    }

    glm::mat4 sponza_model = glm::mat4(1.0f);
    sponza_model           = glm::scale(sponza_model, glm::vec3(0.01f, 0.01f, 0.01f));
    normal_shader.setMat4("model", sponza_model);
    normal_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(sponza_model))));
    normal_shader.setBool("normal_enabled", normal_enabled);
    car.draw(normal_shader);

    if (draw_lights) {
        light_source_shader.use();
        for (int i = 0; i < ARR_SIZE(light_pos); i++) {
            glm::mat4 light_model = glm::mat4(1.0f);
            light_model           = glm::translate(light_model, light_pos[i]);
            light_model           = glm::scale(light_model, glm::vec3(0.5f, 0.5f, 0.5f));
            light_source_shader.setMat4("model", light_model);
            light_source_shader.setVec3("color", light_color[i]);
            light.draw(light_source_shader);
        }
    }
    box->draw();
}

void NormalMap::imgui_update() {
    // Executed each frame for UI
    ImGui::Begin("Inspector");
    if (ImGui::TreeNode("Lights")) {
        for (int i = 0; i < ARR_SIZE(light_pos); i++) {
            ImGui::PushID(i);
            ImGui::Text("Point Light %d", i);
            ImGui::DragFloat3("Position", glm::value_ptr(light_pos[i]));
            ImGui::ColorEdit4("Color", glm::value_ptr(light_color[i]));
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Checkbox("Normal Mapping Enabled", &normal_enabled);
    ImGui::Checkbox("Draw Lights", &draw_lights);

    ImGui::End();

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

void NormalMap::process_input(float dt) {
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

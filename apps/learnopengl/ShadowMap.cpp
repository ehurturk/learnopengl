#include "ShadowMap.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"

ShadowMap::ShadowMap() : Application("Shadow Mapping", 2560, 1600) {}
ShadowMap::~ShadowMap() { delete box; }

void ShadowMap::start() {
    // Executed on initialization
    box = new SkyBox();
    normal_shader.create();
    light_source_shader.create();
    floor_shadow_shader.create();
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
    // normal_shader.load_shader("../res/shaders/shadow.glsl");
    normal_shader.set_uniform_block_binding("Matrices", 0);

    light_source_shader.load_shader("../res/shaders/light.glsl");
    light_source_shader.set_uniform_block_binding("Matrices", 0);

    floor_shadow_shader.load_shader("../res/shaders/ubershader.glsl");
    floor_shadow_shader.set_uniform_block_binding("Matrices", 0);

    // stbi_set_flip_vertically_on_load(false);
    car.load_model("../res/models/backpack/backpack.obj");
    // car.load_model("../res/models/sponza/sponza.gltf");
    // stbi_set_flip_vertically_on_load(true);

    floor.load_model("../res/models/def_cube/scene.gltf");

    light.load_model("../res/models/globe-sphere/globe-sphere.obj");

    depth_map.init(1024, 1024);
    depth_map.add_spec(Framebuffer::FramebufferSpec::DEPTH_MAP);
    depth_map.create();

    depth_map_shader.create();
    depth_map_shader.load_shader("../res/shaders/depth_map.glsl");

    Texture wood   = ResourceManager::load_ogl_texture_from_path("../res/textures/wood.png", Texture::TextureType::DIFFUSE, true);
    Texture normal = ResourceManager::load_ogl_texture_from_path("../res/models/manhole/textures/Scene_-_Root_normal.png", Texture::TextureType::NORMAL, false);
    floor.add_texture(wood);  // diffuse
    floor.add_texture(normal);// diffuse

    car.add_texture(depth_map.get_texture(), "shadow_map");
    floor.add_texture(depth_map.get_texture(), "shadow_map");
}

void ShadowMap::update(float dt) {
    // Executed each frame
    process_input(dt);

    // first render to depth map
    glViewport(0, 0, 1024, 1024);
    depth_map.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    /* light projection matrix */
    float near_plane = 1.0f, far_plane = 15.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    /* light view matrix */
    glm::mat4 lightView = glm::lookAt(dir_light_dir,               // position
                                      glm::vec3(0.0f, 0.0f, 0.0f), // target
                                      glm::vec3(0.0f, 1.0f, 0.0f));// up

    /* light space matrix */
    lightSpaceMatrix = lightProjection * lightView;
    depth_map_shader.use();
    depth_map_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glCullFace(GL_FRONT);
    shadow_pass();
    glCullFace(GL_BACK);
    std::pair<int, int> size = Engine::Get().get_viewport_size();
    glViewport(0, 0, size.first, size.second);
    Engine::Get().get_default_framebuffer().bind();// bind back to Engine's default framebuffer

    geometry_pass();
}

void ShadowMap::geometry_pass() {
    normal_shader.use();
    normal_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    normal_shader.setFloat("material.shininess", 64.0f);

    // Directional light settings
    normal_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);// for tangent space calculation
    normal_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
    normal_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
    normal_shader.setVec3("directional_light.specular", 0.5f, 0.5f, 0.5f);

    // Point light settings
    for (int i = 0; i < ARR_SIZE(light_pos); i++) {
        std::string pos      = "point_lights[" + std::to_string(i) + "].position";// for tangent space calculation
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

    normal_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    normal_shader.setBool("soft_shadows", soft_shadows);
    normal_shader.setBool("normal_enabled", normal_enabled);
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, entity_pos[0]);
        model           = glm::scale(model, entity_scale[0]);
        model           = glm::rotate(model, glm::radians(entity_rot[0].x), glm::vec3(1.0f, 0.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[0].y), glm::vec3(0.0f, 1.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[0].z), glm::vec3(0.0f, 0.0f, 1.0f));
        normal_shader.setMat4("model", model);
        normal_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));
        car.draw(normal_shader);
    }
    floor_shadow_shader.use();
    floor_shadow_shader.setVec3("cam_pos", Engine::Get().get_subsystem<Camera3D>()->position);
    floor_shadow_shader.setFloat("material.shininess", 64.0f);

    // Directional light settings
    floor_shadow_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);// for tangent space calculation
    floor_shadow_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
    floor_shadow_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
    floor_shadow_shader.setVec3("directional_light.specular", 0.5f, 0.5f, 0.5f);

    // Point light settings
    for (int i = 0; i < ARR_SIZE(light_pos); i++) {
        std::string pos      = "point_lights[" + std::to_string(i) + "].position";// for tangent space calculation
        std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
        std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
        std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
        std::string constant = "point_lights[" + std::to_string(i) + "].constant";
        std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
        std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

        constexpr float AMBIENT_INTENSITY = 0.1f;
        floor_shadow_shader.setVec3(pos.c_str(), light_pos[i]);
        floor_shadow_shader.setVec3(am.c_str(), light_color[i].r * AMBIENT_INTENSITY, light_color[i].g * AMBIENT_INTENSITY, light_color[i].b * AMBIENT_INTENSITY);
        floor_shadow_shader.setVec3(diff.c_str(), light_color[i]);
        floor_shadow_shader.setVec3(spec.c_str(), light_color[i]);
        floor_shadow_shader.setFloat(constant.c_str(), 1.0f);
        floor_shadow_shader.setFloat(linear.c_str(), 0.09f);
        floor_shadow_shader.setFloat(quad.c_str(), 0.032f);
    }

    floor_shadow_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    floor_shadow_shader.setBool("soft_shadows", soft_shadows);
    floor_shadow_shader.setBool("normal_enabled", normal_enabled);
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, entity_pos[1]);
        model           = glm::scale(model, entity_scale[1]);
        model           = glm::rotate(model, glm::radians(entity_rot[1].x), glm::vec3(1.0f, 0.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[1].y), glm::vec3(0.0f, 1.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[1].z), glm::vec3(0.0f, 0.0f, 1.0f));
        floor_shadow_shader.setMat4("model", model);
        floor_shadow_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));
        floor.draw(floor_shadow_shader);
    }

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

void ShadowMap::shadow_pass() {
    // draw what is necessary (no skyboxes, no debug objects such as lights)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, entity_pos[0]);
        model           = glm::scale(model, entity_scale[0]);
        model           = glm::rotate(model, glm::radians(entity_rot[0].x), glm::vec3(1.0f, 0.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[0].y), glm::vec3(0.0f, 1.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[0].z), glm::vec3(0.0f, 0.0f, 1.0f));
        depth_map_shader.setMat4("model", model);
        car.draw(depth_map_shader);
    }
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, entity_pos[1]);
        model           = glm::scale(model, entity_scale[1]);
        model           = glm::rotate(model, glm::radians(entity_rot[1].x), glm::vec3(1.0f, 0.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[1].y), glm::vec3(0.0f, 1.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(entity_rot[1].z), glm::vec3(0.0f, 0.0f, 1.0f));
        depth_map_shader.setMat4("model", model);
        floor.draw(depth_map_shader);
    }
}

void ShadowMap::imgui_update() {
    // Executed each frame for UI
    ImGui::Begin("Inspector");
    if (ImGui::TreeNode("Lights")) {
        for (int i = 0; i < ARR_SIZE(light_pos); i++) {
            ImGui::PushID(i);
            ImGui::Text("Point Light %d", i + 1);
            ImGui::DragFloat3("Position", glm::value_ptr(light_pos[i]));
            ImGui::ColorEdit4("Color", glm::value_ptr(light_color[i]));
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Entities")) {
        for (int i = 0; i < ARR_SIZE(entity_pos); i++) {
            ImGui::PushID(i);
            ImGui::Text("Entity %d", i + 1);
            ImGui::DragFloat3("Position", glm::value_ptr(entity_pos[i]));
            ImGui::DragFloat3("Rotation", glm::value_ptr(entity_rot[i]));
            ImGui::DragFloat3("Scale", glm::value_ptr(entity_scale[i]));
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::DragFloat3("Directional Light Direction", glm::value_ptr(dir_light_dir));

    ImGui::Checkbox("Normal Mapping Enabled", &normal_enabled);
    ImGui::Checkbox("Draw Lights", &draw_lights);
    ImGui::Checkbox("Soft Shadows", &soft_shadows);

    ImGui::Image((ImTextureID) depth_map.get_texture(), { 512, 512 }, ImVec2(1, 0), ImVec2(0, 1));

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
        ImGui::Text("Camera Position: (%f, %f, %f)", Engine::Get().get_subsystem<Camera3D>()->position.x, Engine::Get().get_subsystem<Camera3D>()->position.y, Engine::Get().get_subsystem<Camera3D>()->position.z);
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

void ShadowMap::process_input(float dt) {
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

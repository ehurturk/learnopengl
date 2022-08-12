#pragma once

#include "Engine.hpp"
#include "Quark.hpp"
#include "glm/fwd.hpp"
#include "graphics/SkyBox.h"

class ShadowMap : public Application {
public:
    ShadowMap();
    ~ShadowMap();

    // Executed on initialization
    void start() override;
    // Executed each frame
    void update(float dt) override;
    // Executed each frame for UI
    void imgui_update() override;

    void shadow_pass();
    void geometry_pass();

    // Additional functions...
    void process_input(float dt);

private:
    // All private variables...
    Shader normal_shader;
    Shader light_source_shader;
    Shader depth_map_shader;
    Shader floor_shadow_shader;

    SkyBox *box;

    Model car;
    Model floor;

    Model light;

    Framebuffer depth_map;

    bool normal_enabled = true;
    bool draw_lights    = false;
    bool soft_shadows   = true;

#define COLOR_WHITE glm::vec3(1.0f)
#define COLOR_BLUE  glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_RED   glm::vec3(1.0f, 0.0f, 0.0f)

    glm::vec3 light_pos[4] = {
        glm::vec3(-3.0f, 0.5f, 0.0f),
        glm::vec3(-1.0f, 0.5f, 0.0f),
        glm::vec3(1.0f, 0.5f, 0.0f),
        glm::vec3(3.0f, 0.5f, 0.0f)
    };
    glm::vec3 light_color[4] = { glm::vec3(0.25),
                                 glm::vec3(0.50),
                                 glm::vec3(0.75),
                                 glm::vec3(1.00) };

    glm::vec3 entity_pos[2] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -4.0f, 0.0f),
    };
    glm::vec3 entity_scale[2] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(20.0f, 0.01f, 20.0f),
    };
    glm::vec3 entity_rot[2] = {
        glm::vec3(0.0f, -90.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };

    glm::mat4 lightSpaceMatrix;
    glm::vec3 dir_light_dir = glm::vec3(-3.0f, 3.0f, -1.0f);
};

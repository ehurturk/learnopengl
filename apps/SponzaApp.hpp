#pragma once

#include "../include/Application.hpp"
#include "../include/graphics/UniformBuffer.hpp"
#include "../include/Quark.hpp"
#include "../include/Engine.hpp"
#include "graphics/SkyBox.h"

class SponzaApp : public Application {
public:
    SponzaApp() : Application("Sponza", 1280, 1024) {
    }
    ~SponzaApp();
    void start() override;
    void update(float dt) override;
    void imgui_update() override;

    void process_input(float dt);

private:
    Shader phong_shader;
    Shader light_shader;
    Shader reflection_shader;

    Model earth;
    Model sponza;
    Model light_sphere;

    SkyBox *skybox;

    glm::vec3 colorWhite  = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 colorBlue   = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 colorGreen  = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 colorRed    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 colorYellow = glm::vec3(0.0f, 1.0f, 1.0f);

    glm::vec3 pointLightColors[4] = { colorBlue, colorGreen, colorRed, colorYellow };

    glm::vec3 pointLightPositions[4] = {
        glm::vec3(0.0f, 0.0f, -3.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.5f, -3.0f),
        glm::vec3(1.0f, -1.0f, 2.0f),
    };

    glm::vec3 earth_positions[3] = {
        glm::vec3(0.0f, -2.0f, 4.0f),
        glm::vec3(3.0f, -2.0f, 4.0f),
        glm::vec3(-5.0f, -2.0f, 4.0f),
    };

    bool my_tool_active = false;
    bool draw_lights    = false;
};

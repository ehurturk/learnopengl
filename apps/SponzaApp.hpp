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

    Model backpack;
    Model sponza;

    SkyBox *skybox;

    glm::vec3 colorWhite = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 pointLightColors[4] = { colorWhite };

    glm::vec3 pointLightPositions[1] = {
        glm::vec3(0.0f, 0.0f, -3.0f),
    };

    glm::vec3 backpack_positions[3] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -2.5f),
    };

    bool my_tool_active = false;
};

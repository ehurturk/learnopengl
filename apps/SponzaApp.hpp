#pragma once

#include "../include/Application.hpp"
#include "../include/graphics/UniformBuffer.hpp"
#include "../include/Quark.hpp"
#include "../include/Engine.hpp"

class SponzaApp : public Application {
public:
    SponzaApp() {}
    ~SponzaApp() {}
    void start() override;
    void update(float dt) override;
    void imgui_update() override;

    void process_input(float dt);

private:
    Shader backpack_shader;
    Shader sponza_shader;
    Model backpack;
    Model sponza;

    glm::vec3 colorRed    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 colorGreen  = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 colorBlue   = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 colorYellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 colorWhite  = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 pointLightColors[4] = {colorRed, colorGreen, colorBlue, colorYellow};

    glm::vec3 pointLightPositions[4] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, 0.4f, -4.0f),
            glm::vec3(1.0f, 2.0f, -3.0f),
            glm::vec3(0.0f, 0.0f, -3.0f),
    };

    glm::vec3 backpack_positions[3] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
    };

    bool my_tool_active = false;
};

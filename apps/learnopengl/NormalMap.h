#pragma once

#include "Engine.hpp"
#include "Quark.hpp"
#include "graphics/SkyBox.h"

class NormalMap : public Application {
public:
    NormalMap();
    ~NormalMap();

    // Executed on initialization
    void start() override;
    // Executed each frame
    void update(float dt) override;
    // Executed each frame for UI
    void imgui_update() override;

    // Additional functions...
    void process_input(float dt);

private:
    // All private variables...
    Shader normal_shader;
    SkyBox *box;
    Model car;
};

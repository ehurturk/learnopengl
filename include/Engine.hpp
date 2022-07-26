#pragma once

#include <iostream>
#include <memory>
#include <array>

/* OpenGL Functionality */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
/* GLM Functionality */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/* Utils */
#include "thirdparty/stb_image.h"

#include "Shader.hpp"
#include "Resource.hpp"
#include "Window.hpp"
#include "Camera.h"
#include "Utils.hpp"
#include "common.h"

struct EngineConfig {
    const std::string &title;
    ui32 width;
    ui32 height;
    bool full_screen;
};

class Engine {
public:
    Engine(const std::string &title, ui32 width, ui32 height);
    Engine(const std::string &title);
    ~Engine();

    void start();
    void update();

private:
    EngineConfig cfg;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Camera> m_Camera;
};
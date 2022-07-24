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
#include "Utils.hpp"
#include "common.h"

struct EngineConfig {
    const std::string &title;
    ui32 width;
    ui32 height;
};

class Engine {
public:
    Engine(const std::string &title, ui32 width, ui32 height);
    ~Engine();

    void test();
    void start();

private:
    EngineConfig cfg;
    std::unique_ptr<Window> m_Window;
};
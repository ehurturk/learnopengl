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
#include "Utils.hpp"

struct EngineConfig {
    const char *title;
    int width;
    int height;
};

class Engine {
public:
    Engine(const EngineConfig config);
    ~Engine();

    void start();

private:
    EngineConfig cfg;
};
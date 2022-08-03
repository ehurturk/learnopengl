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
#include "Application.hpp"
#include "graphics/UniformBuffer.hpp"

// forward declerations

class Engine {
public:
    static Engine &Get() {
        if (!instance)
            instance = std::unique_ptr<Engine>(new Engine());

        return *instance;
    }
    ~Engine();

    void register_app(Application *app);

    void create(AppConfig cfg);
    void start();
    void update();

    template<typename T>
    inline std::unique_ptr<T> &get_subsystem();

    template<>
    inline std::unique_ptr<Camera3D> &get_subsystem<Camera3D>() { return m_Camera; }

    template<>
    inline std::unique_ptr<Window> &get_subsystem<Window>() { return m_Window; }

private:
    static std::unique_ptr<Engine> instance;
    Engine();

    void framebuffer_callback_fn(GLFWwindow *, int, int);
    void mouse_input_callback_fn(GLFWwindow *, double, double);
    void key_input_callback_fn(GLFWwindow *, int, int, int, int);

    AppConfig cfg;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Camera3D> m_Camera;
    /* std::stack<std::unique_ptr<EngineSystem>> systems; */
    /* for system in systems system->start(); */
    /* for system in systems system->update(dt); */
    Application *m_App;
    UniformBuffer m_UniformBuffer;

    float quad_vertices[24] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int qvbo, qvao;
    Shader quad_shader;
};

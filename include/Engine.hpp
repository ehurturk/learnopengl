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
#include "graphics/Framebuffer.h"

// forward declerations
enum class PostProcessElements {
    GAMMA_CORRECTION,
    HDR_TONEMAPPING,
};


struct HdrToneMappingSettings {
    HdrToneMappingSettings() {}
    enum class HdrToneMapType {
        NONE = 0,
        ACES_FILMIC,
        REINHARD,
        EXPOSURE,
        UNCHARTED2
    };
    union HdrToneMapInfo {
        HdrToneMapInfo() {}
        struct {
            float exposure = 0.1f;
        };
    };
    HdrToneMapType type;
    HdrToneMapInfo info;
};

struct GammaCorrectionSettings {
    float gamma = 2.2f;
};

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
    inline std::unique_ptr<T> &get_subsystem() {}

    template<>
    inline std::unique_ptr<Camera3D> &get_subsystem<Camera3D>() { return m_Camera; }

    template<>
    inline std::unique_ptr<Window> &get_subsystem<Window>() { return m_Window; }

    inline Framebuffer &get_default_framebuffer() { return m_Framebuffer; }

    // returns the size of the raw glfw window if raw mode is enabled, else returns the size of the IMGUI viewport
    std::pair<int, int> get_viewport_size() {
        if (cfg.raw) {
            int w, h;
            glfwGetFramebufferSize(m_Window->get_raw_window(), &w, &h);
            return std::make_pair(w, h);
        }
        return std::make_pair(viewport_w, viewport_h);
    }

    void add_post_process_effect(PostProcessElements element, void *settings);

private:
    static std::unique_ptr<Engine> instance;
    Engine();

    void framebuffer_callback_fn(GLFWwindow *, int, int);
    void mouse_input_callback_fn(GLFWwindow *, double, double);
    void key_input_callback_fn(GLFWwindow *, int, int, int, int);

    void draw_viewport();

    AppConfig cfg;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Camera3D> m_Camera;

    Application *m_App;

    UniformBuffer m_UniformBuffer;
    Framebuffer m_PostProcessBuffer;
    Framebuffer m_Framebuffer;

    float quad_vertices[24] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    unsigned int vvbo, vvao;

    unsigned viewport_w, viewport_h;

    bool block_input = false;

    Shader quad_shader;
    Shader post_process_shader;

    std::unordered_map<PostProcessElements, void *> post_process_stack;
};

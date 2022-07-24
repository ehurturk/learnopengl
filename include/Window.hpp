#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include "common.h"


class Window {
public:
    typedef void (*KeyInputCallbackFunc)(GLFWwindow *window, int key, int scancode, int action, int mods);
    typedef void (*MouseInputCallbackFunc)(GLFWwindow *window, double xpos, double ypos);
    typedef void (*FramebufferSizeCallbackFunc)(GLFWwindow *window, int width, int height);

    struct WindowConfig {
        const std::string &title;
        ui32 width;
        ui32 height;
    };
    explicit Window(WindowConfig cfg);
    Window(const std::string &title, ui32 width, ui32 height);
    ~Window();

    void update();
    void post_update();

    template<typename T>
    void bind_func(T func);

    template<>
    void bind_func<KeyInputCallbackFunc>(KeyInputCallbackFunc func);

    template<>
    void bind_func<MouseInputCallbackFunc>(MouseInputCallbackFunc func);

    template<>
    void bind_func<FramebufferSizeCallbackFunc>(FramebufferSizeCallbackFunc func);
    WindowConfig config;

    GLFWwindow *get_raw_window() { return window; }
    int is_window_open() { return glfwWindowShouldClose(window); }

    void initialize();

private:
    struct GLFWCallbackData {
        KeyInputCallbackFunc K_fn;
        MouseInputCallbackFunc M_fn;
        FramebufferSizeCallbackFunc F_fn;
    };

    GLFWwindow *window;
    GLFWCallbackData callback_data;
};
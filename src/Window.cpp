#include "Window.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "thirdparty/stb_image.h"
#include <cassert>

// void framebuffer_size_callback(GLFWwindow *window, int width,
//                                int height) {
//     // make sure the viewport matches the new window dimensions; note that width
//     // and height will be significantly larger than specified on retina displays.
// TODO: Change camera projection matrix based on new width and height values.
//     // something like camera->set_projection(width, height);
//     glViewport(0, 0, width, height);
// }

Window::Window(Window::WindowConfig cfg) : config(cfg) {
}

Window::Window(const std::string &title, ui32 width, ui32 height) : config((Window::WindowConfig){ .title = title, .width = width, .height = height }) {
}

Window::~Window() {
    glfwTerminate();
}

void Window::initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation

    // window = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
    window = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (callback_data.K_fn != nullptr) glfwSetKeyCallback(window, callback_data.K_fn);
    if (callback_data.F_fn != nullptr) glfwSetFramebufferSizeCallback(window, callback_data.F_fn);
    if (callback_data.M_fn != nullptr) glfwSetCursorPosCallback(window, callback_data.M_fn);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    stbi_set_flip_vertically_on_load(true);
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    config.width  = w;
    config.height = h;

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImFont *font = io.Fonts->AddFontFromFileTTF("../res/fonts/Roboto-Medium.ttf", 16.0f);
    if (font == NULL)
        return;
}

void Window::update() {
    glfwPollEvents();
}

void Window::post_update() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
}

void Window::set_window_user_pointer(void *ptr) {
    glfwSetWindowUserPointer(window, ptr);
}

template<typename T>
void Window::bind_func(T func) {}

template<>
void Window::bind_func<Window::KeyInputCallbackFunc>(Window::KeyInputCallbackFunc func) {
    callback_data.K_fn = func;
}

template<>
void Window::bind_func<Window::MouseInputCallbackFunc>(Window::MouseInputCallbackFunc func) {
    callback_data.M_fn = func;
}

template<>
void Window::bind_func<Window::FramebufferSizeCallbackFunc>(Window::FramebufferSizeCallbackFunc func) {
    callback_data.F_fn = func;
}
#include "Window.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "thirdparty/stb_image.h"
#include <cassert>

Window::Window(Window::WindowConfig cfg) : config(cfg) {
}

Window::Window(const std::string &title, ui32 width, ui32 height) : config((Window::WindowConfig){ .title = title, .width = width, .height = height }) {
}

Window::~Window() {
    glfwTerminate();
}

double prevTime = 0.0f;
double crntTime = 0.0f;
double deltaT;
unsigned int counter = 0;

void Window::initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation

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
    crntTime = glfwGetTime();
    deltaT   = crntTime - prevTime;
    counter++;
    if (deltaT >= 1.0f / 30.0f) {
        double fps = (1.0f / deltaT) * counter;
        double ms  = (deltaT / counter) * 1000.0f;
        data.fps   = fps;
        data.ms    = ms;
        prevTime   = crntTime;
        counter    = 0.0f;
    }
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
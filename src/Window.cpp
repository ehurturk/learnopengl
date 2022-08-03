#include "Window.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "thirdparty/stb_image.h"
#include <cassert>
#include <iterator>

Window::Window(Window::WindowConfig cfg) : config(cfg) {
}

Window::Window(const std::string &title, ui32 width, ui32 height, bool fullscreen, bool raw) : config((Window::WindowConfig){ .title = title, .width = width, .height = height, .fullscreen = fullscreen, .raw = raw }) {
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// github user @janekb04
void embraceTheDarkness() {
    ImVec4 *colors                         = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]        = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle &style                     = ImGui::GetStyle();
    style.WindowPadding                   = ImVec2(8.00f, 8.00f);
    style.FramePadding                    = ImVec2(5.00f, 2.00f);
    style.CellPadding                     = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                     = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding               = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                   = 25;
    style.ScrollbarSize                   = 15;
    style.GrabMinSize                     = 10;
    style.WindowBorderSize                = 1;
    style.ChildBorderSize                 = 1;
    style.PopupBorderSize                 = 1;
    style.FrameBorderSize                 = 1;
    style.TabBorderSize                   = 1;
    style.WindowRounding                  = 7;
    style.ChildRounding                   = 4;
    style.FrameRounding                   = 3;
    style.PopupRounding                   = 4;
    style.ScrollbarRounding               = 9;
    style.GrabRounding                    = 3;
    style.LogSliderDeadzone               = 4;
    style.TabRounding                     = 4;
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
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
    if (config.fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        config.width            = mode->width;
        config.height           = mode->height;
        window                  = glfwCreateWindow(400, 300, config.title.c_str(), monitor, NULL);
        glfwSetWindowSize(window, mode->width, mode->height);
    } else {
        window = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
    }

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

    glfwGetFramebufferSize(window, (int *) &config.width, (int *) &config.height);
    std::cout << config.width << config.height << std::endl;

    glViewport(0, 0, config.width, config.height);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    if (!config.raw) {

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        (void) io;
        embraceTheDarkness();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        ImFont *font = io.Fonts->AddFontFromFileTTF("../res/fonts/Roboto-Medium.ttf", 16.0f);
        if (font == NULL)
            return;
    }
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
    if (!config.raw) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
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
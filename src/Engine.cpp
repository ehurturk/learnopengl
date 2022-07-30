#include <Engine.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Model.hpp"

float dt = 0.0f;
float lt = 0.0f;

bool my_tool_active = false;

std::unique_ptr<Engine> Engine::instance = nullptr;
Engine::Engine(const char *title, ui32 width, ui32 height) {
    cfg.title  = title;
    cfg.width  = width;
    cfg.height = height;
    std::cout << cfg.title << std::endl;
}

Engine::Engine(const char *title) {
    cfg.title       = title;
    cfg.full_screen = true;
}

Engine::~Engine() {
    std::cout << "deleting engine...\n";
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void processInput(GLFWwindow *window, std::unique_ptr<Camera> &cam);

void Engine::register_app(Application *app) { m_App = app; }

void Engine::update() {
    while (m_Window->is_window_open()) {
        m_Window->update();
        m_Camera->update();

        // set the global view matrix
        m_UniformBuffer.set_data(m_Camera->get_view_matrix(), sizeof(glm::mat4));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float cf = glfwGetTime();
        dt       = cf - lt;
        lt       = cf;

        m_App->imgui_update();
        ImGui::Render();

        m_App->update(dt);
        m_Window->post_update();
    }
}

void Engine::start() {

    std::cout << cfg.title << std::endl;

    m_Window = std::make_unique<Window>(cfg.title, cfg.width, cfg.height);
    m_Camera = std::make_unique<Camera>(m_Window);

    m_Window->initialize();
    m_Window->set_window_user_pointer(static_cast<void *>(m_Camera.get()));

    // init uniform buffer for global shader uniforms
    m_UniformBuffer = UniformBuffer();
    m_UniformBuffer.init(2 * sizeof(glm::mat4));
    m_UniformBuffer.bind(0);
    m_UniformBuffer.load_data(m_Camera->get_projection_matrix());
    m_App->start();
}

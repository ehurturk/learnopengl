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

struct GlobalUniformData {
    glm::mat4 projview;
};

std::unique_ptr<Engine> Engine::instance = nullptr;
Engine::Engine() {
}

Engine::~Engine() {
    std::cout << "deleting engine...\n";
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Engine::register_app(Application *app) { m_App = app; }

void Engine::create(const char *title, ui32 width, ui32 height) {
    cfg.title  = title;
    cfg.width  = width;
    cfg.height = height;
}

void Engine::update() {
    while (m_Window->is_window_open()) {
        m_Window->update();
        m_Camera->update();

        // set the global proj view matrix
        m_UniformBuffer.set_data(m_Camera->get_projection_matrix() * m_Camera->get_view_matrix(), offsetof(GlobalUniformData, projview));

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
    m_Camera = std::make_unique<Camera3D>(m_Window);

    m_Window->initialize();
    m_Window->set_window_user_pointer(static_cast<void *>(m_Camera.get()));

    // init uniform buffer for global shader uniforms
    m_UniformBuffer = UniformBuffer();
    m_UniformBuffer.init(1 * sizeof(glm::mat4));
    m_UniformBuffer.bind(0); /* bind binding 0 ranging all data */
    /* alternatively, one can use .bind(0, offset, size) to bind specific portion of data */
    /* binding specific region of data is especially useful when an UBO is being used as a buffer of all uniforms, and one can easily select a specific part of the buffer to load in the uniforms.*/
    m_App->start();
}

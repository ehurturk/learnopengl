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

unsigned int framebuffer;
unsigned int tex_color;
unsigned int rbo;

float viewport_x = 0.0f;
float viewport_y = 0.0f;

void resize_framebuffer(int, int);

struct GlobalUniformData {
    glm::mat4 projview;
};

std::unique_ptr<Engine> Engine::instance = nullptr;
Engine::Engine() {
}

Engine::~Engine() {
    std::cout << "deleting engine...\n";
    if (!cfg.raw) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &tex_color);
    glDeleteRenderbuffers(1, &rbo);

    glDeleteVertexArrays(1, &qvao);
    glDeleteBuffers(1, &qvbo);
}

void Engine::register_app(Application *app) { m_App = app; }

void Engine::create(AppConfig cfg_) {
    cfg = cfg_;
}

void Engine::update() {
    while (m_Window->is_window_open()) {
        m_Window->update();
        m_Camera->update();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set the global proj view matrix
        m_UniformBuffer.set_data(m_Camera->get_projection_matrix() * m_Camera->get_view_matrix(), offsetof(GlobalUniformData, projview));

        if (!cfg.raw) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
        float cf = glfwGetTime();
        dt       = cf - lt;
        lt       = cf;

        if (!cfg.raw) {
            static bool opt_fullscreen                = true;
            static bool opt_padding                   = false;
            static bool docking_enabled               = true;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags_docking = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen) {
                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags_docking |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags_docking |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            } else {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags_docking |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &docking_enabled, window_flags_docking);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO &dock_io = ImGui::GetIO();
            if (dock_io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    ImGui::MenuItem("Open...", "CTRL+O");
                    ImGui::MenuItem("Save...", "CTRL+S");
                    ImGui::MenuItem("Save as...", "CTRL+A");
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Options")) {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Close", NULL, false))
                        docking_enabled = false;
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            /**================================================== *
          * ==========  Viewport  ========== *
          * ================================================== */
            static bool viewport_open = true;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
            ImGui::Begin("Viewport", &viewport_open);
            {
                // Get the size of the child (i.e. the whole draw size of the windows).
                ImVec2 wsize = ImGui::GetContentRegionAvail();
                if (viewport_x != wsize.x || viewport_y != wsize.y) {
                    viewport_x = wsize.x;
                    viewport_y = wsize.y;
                    // TODO: New framebuffer here
                    m_Camera->adjust_viewport(viewport_x, viewport_y);
                    resize_framebuffer(viewport_x, viewport_y);
                }
                ImGui::Image((ImTextureID) tex_color, wsize, ImVec2(0, 1), ImVec2(1, 0));
            }
            ImGui::End();
            ImGui::PopStyleVar();

            /* =======  End of Viewport  ======= */
            m_App->imgui_update();
            ImGui::End(); /* end of docking */
            ImGui::Render();
        }
        m_App->update(dt);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);// disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);// set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        if (cfg.raw) {
            quad_shader.use();
            glBindVertexArray(qvao);
            glBindTexture(GL_TEXTURE_2D, tex_color);// use the color attachment texture as the texture of the quad plane
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        m_Window->post_update();
    }
}

void Engine::framebuffer_callback_fn(GLFWwindow *window, int w, int h) {
    m_Camera->adjust_viewport(w, h);
    resize_framebuffer(w, h);
}

void Engine::mouse_input_callback_fn(GLFWwindow *window, double xpos, double ypos) {
    m_Camera->mouse_callback_fn(xpos, ypos);
}

void Engine::key_input_callback_fn(GLFWwindow *window, int key, int scan, int action, int mods) {
    m_Camera->key_callback_fn(key, scan, action, mods);
}

void Engine::start() {

    std::cout << cfg.title << std::endl;

    m_Window = std::make_unique<Window>(cfg.title, cfg.width, cfg.height, cfg.fullscreen, cfg.raw);
    m_Camera = std::make_unique<Camera3D>(m_Window);

    m_Window->bind_func<Window::MouseInputCallbackFunc>([](GLFWwindow *window, double xpos, double ypos) {
        auto engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
        if (engine != nullptr)
            engine->mouse_input_callback_fn(window, xpos, ypos);
    });

    m_Window->bind_func<Window::FramebufferSizeCallbackFunc>([](GLFWwindow *window, int w, int h) {
        auto engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
        if (engine != nullptr)
            engine->framebuffer_callback_fn(window, w, h);
    });

    m_Window->bind_func<Window::KeyInputCallbackFunc>([](GLFWwindow *window, int k, int s, int a, int m) {
        auto engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
        if (engine != nullptr)
            engine->key_input_callback_fn(window, k, s, a, m);
    });

    m_Window->initialize();
    m_Window->set_window_user_pointer(static_cast<void *>(instance.get()));


    // init uniform buffer for global shader uniforms
    m_UniformBuffer = UniformBuffer();
    m_UniformBuffer.init(1 * sizeof(glm::mat4));
    m_UniformBuffer.bind(0); /* bind binding 0 ranging all data */
    /* alternatively, one can use .bind(0, offset, size) to bind specific portion of data */
    /* binding specific region of data is especially useful when an UBO is being used as a buffer of all uniforms, and one can easily select a specific part of the buffer to load in the uniforms.*/

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    glGenTextures(1, &tex_color);
    glBindTexture(GL_TEXTURE_2D, tex_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Window->config.width, m_Window->config.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Window->config.width, m_Window->config.height);// use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);                // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (cfg.raw) {
        glGenVertexArrays(1, &qvao);
        glGenBuffers(1, &qvbo);
        glBindVertexArray(qvao);
        glBindBuffer(GL_ARRAY_BUFFER, qvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    }

    quad_shader.create();
    quad_shader.load_shader("../res/shaders/framebuffer_quad.vs", Shader::ShaderType::VERTEX_SHADER);
    quad_shader.load_shader("../res/shaders/framebuffer_quad.fs", Shader::ShaderType::FRAGMENT_SHADER);

    quad_shader.use();
    quad_shader.setInt("tex", 0);

    m_App->start();
}

void resize_framebuffer(int width, int height) {
    framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &tex_color);
    glBindTexture(GL_TEXTURE_2D, tex_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
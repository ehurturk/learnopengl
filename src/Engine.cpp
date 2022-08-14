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

float viewport_x = 0.0f;
float viewport_y = 0.0f;

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

    if (vvao != 0)
        glDeleteVertexArrays(1, &vvao);
    if (vvbo != 0)
        glDeleteBuffers(1, &vvbo);
}

void Engine::register_app(Application *app) { m_App = app; }

void Engine::create(AppConfig cfg_) {
    cfg = cfg_;
}

void Engine::update() {
    while (m_Window->is_window_open()) {
        m_Window->update();
        m_Camera->update();

        m_Framebuffer.bind();
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

        m_App->update(dt);// render scene

        m_PostProcessBuffer.bind();
        glDisable(GL_DEPTH_TEST);// disable depth test so screen-space quad isn't discarded due to depth test.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        post_process_shader.use();
        // Draw to the post process buffer
        glBindVertexArray(vvao);
        glBindTexture(GL_TEXTURE_2D, m_Framebuffer.get_texture());// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetContentRegionAvail();
            viewport_w   = wsize.x;
            viewport_h   = wsize.y;

            if (viewport_x != wsize.x || viewport_y != wsize.y) {
                viewport_x = wsize.x;
                viewport_y = wsize.y;
                // TODO: On window resize
                framebuffer_callback_fn(m_Window->get_raw_window(), viewport_x, viewport_h);
            }
            ImGui::Image((ImTextureID) m_PostProcessBuffer.get_texture(), wsize, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::End();
            ImGui::PopStyleVar();

            /* =======  End of Viewport  ======= */
            m_App->imgui_update();
            ImGui::End(); /* end of docking */
            ImGui::Render();
        }

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (const auto &element : post_process_stack) {
            switch (element.first) {
                case PostProcessElements::GAMMA_CORRECTION:
                    post_process_shader.setInt("gamma_correction", 1);
                    post_process_shader.setFloat("gamma", ((GammaCorrectionSettings *) element.second)->gamma);
                    break;
                case PostProcessElements::HDR_TONEMAPPING:
                    HdrToneMappingSettings *settings = (HdrToneMappingSettings *) element.second;
                    post_process_shader.setInt("tone_mapping", static_cast<int>(settings->type));// 0 for none, 1 for aces filmic, 2 for reinhard, 3 for exposure
                    switch (settings->type) {
                        case HdrToneMappingSettings::HdrToneMapType::ACES_FILMIC:
                            break;
                        case HdrToneMappingSettings::HdrToneMapType::EXPOSURE:
                            post_process_shader.setFloat("exposure", settings->info.exposure);// 0 for none, 1 for aces filmic, 2 for reinhard, 3 for exposure
                            break;
                        case HdrToneMappingSettings::HdrToneMapType::REINHARD:
                            break;
                        case HdrToneMappingSettings::HdrToneMapType::UNCHARTED2:
                            break;
                        case HdrToneMappingSettings::HdrToneMapType::NONE:
                            break;
                    }
                    break;
            }
        }

        if (cfg.raw) {
            glDisable(GL_DEPTH_TEST);// disable depth test so screen-space quad isn't discarded due to depth test.
            // clear all relevant buffers
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);// set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT);
            quad_shader.use();
            glBindVertexArray(vvao);
            glBindTexture(GL_TEXTURE_2D, m_PostProcessBuffer.get_texture());// use the color attachment texture as the texture of the quad plane
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        m_Window->post_update();
    }
}

void Engine::add_post_process_effect(PostProcessElements element, void *settings) {
    post_process_stack[element] = settings;
}

void Engine::framebuffer_callback_fn(GLFWwindow *window, int w, int h) {
    m_Camera->adjust_viewport(w, h);// proj matrix correction + glViewport
    m_Framebuffer.resize(w, h);
    m_PostProcessBuffer.resize(w, h);
}

void Engine::mouse_input_callback_fn(GLFWwindow *window, double xpos, double ypos) {
    m_Camera->mouse_callback_fn(xpos, ypos);
}

void Engine::key_input_callback_fn(GLFWwindow *window, int key, int scan, int action, int mods) {
    m_Camera->key_callback_fn(key, scan, action, mods);
}

void Engine::start() {
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

    m_Framebuffer.init(m_Window->config.width, m_Window->config.height);
    m_Framebuffer.add_spec(Framebuffer::FramebufferSpec::COLOR);
    m_Framebuffer.add_spec(Framebuffer::FramebufferSpec::DEPTH24STENCIL8);
    m_Framebuffer.create();

    m_PostProcessBuffer.init(m_Window->config.width, m_Window->config.height);
    m_PostProcessBuffer.add_spec(Framebuffer::FramebufferSpec::COLOR);
    // m_PostProcessBuffer.add_spec(Framebuffer::FramebufferSpec::DEPTH24STENCIL8);
    m_PostProcessBuffer.create();

    glGenVertexArrays(1, &vvao);
    glGenBuffers(1, &vvbo);
    glBindVertexArray(vvao);
    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    quad_shader.create();
    quad_shader.load_shader("../res/shaders/framebuffer_quad.glsl");

    quad_shader.use();
    quad_shader.setInt("tex", 0);

    post_process_shader.create();
    post_process_shader.load_shader("../res/shaders/post_process.glsl");

    post_process_shader.use();
    post_process_shader.setInt("tex", 0);

    m_App->start();
}

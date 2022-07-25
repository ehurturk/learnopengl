#include <Engine.hpp>
#include <sstream>
#include <string>

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Model.hpp"

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 camPos       = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up           = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camFront     = glm::vec3(0.0f, 0.0f, -1.0f);

float dt = 0.0f;
float lt = 0.0f;

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

float yaw;
float pitch;

bool first_time_mouse_enter = true;
bool my_tool_active         = false;


Engine::Engine(const std::string &title, ui32 width, ui32 height) : cfg((EngineConfig){.title = title, .width = width, .height = height}) {}

Engine::~Engine() {}

static void processInput(GLFWwindow *window);
static void key_input_callback(GLFWwindow *window, int, int, int, int);
static void cursor_pos_callback(GLFWwindow *window, double x, double y);
static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height);
unsigned int load_texture(const char *path);

glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

void Engine::start() {
    m_Window = std::make_unique<Window>(cfg.title, cfg.width, cfg.height);
    m_Window->bind_func<Window::KeyInputCallbackFunc>(key_input_callback);
    m_Window->bind_func<Window::MouseInputCallbackFunc>(cursor_pos_callback);
    m_Window->bind_func<Window::FramebufferSizeCallbackFunc>(framebuffer_size_callback);
    m_Window->initialize();
    GLFWwindow *window = m_Window->get_raw_window();

    //     // set up vertex data (and buffer(s)) and configure vertex attributes
    //     // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

#pragma region LIGHT 1
    unsigned int lightVAO;
    unsigned int VBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
#pragma endregion

    Shader backpack_shader;
    backpack_shader.load_shader("../res/shaders/cube.vs", Shader::ShaderType::VERTEX_SHADER);
    backpack_shader.load_shader("../res/shaders/cube.fs", Shader::ShaderType::FRAGMENT_SHADER);

    Model backpack("../res/models/backpack/backpack.obj");

    Shader sponza_shader;
    sponza_shader.load_shader("../res/shaders/cube.vs", Shader::ShaderType::VERTEX_SHADER);
    sponza_shader.load_shader("../res/shaders/cube.fs", Shader::ShaderType::FRAGMENT_SHADER);

    stbi_set_flip_vertically_on_load(false);
    Model sponza("../res/models/sponza/sponza.obj");
    stbi_set_flip_vertically_on_load(true);

    Shader lightShader;
    lightShader.load_shader("../res/shaders/light.vs", Shader::ShaderType::VERTEX_SHADER);
    lightShader.load_shader("../res/shaders/light.fs", Shader::ShaderType::FRAGMENT_SHADER);

    glm::vec3 colorRed    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 colorGreen  = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 colorBlue   = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 colorYellow = glm::vec3(1.0f, 1.0f, 0.0f);

    glm::vec3 pointLightColors[] = {colorRed, colorGreen, colorBlue, colorYellow, colorYellow, colorGreen, colorBlue};

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f),
            glm::vec3(13.0f, 0.0f, 20.0f),
            glm::vec3(20.0f, 0.0f, 20.0f),
            glm::vec3(27.0f, 0.0f, 20.0f)};

    glm::vec3 backpack_positions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)};

    while (!glfwWindowShouldClose(m_Window->get_raw_window())) {
        // input
        // -----
        m_Window->update();
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // std::cout << "pos: (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")" << std::endl;

        float cf = glfwGetTime();

        dt = cf - lt;
        lt = cf;

        {
            static glm::vec4 my_color;
            ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // Edit a color (stored as ~4 floats)
            ImGui::ColorEdit4("Color", glm::value_ptr(my_color));

            // Plot some values
            const float my_values[] = {0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f};
            ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

            // Display contents in a scrolling region
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
            ImGui::BeginChild("Scrolling");
            for (int n = 0; n < 50; n++)
                ImGui::Text("%04d: Some text", n);
            ImGui::EndChild();
            ImGui::End();
        }

        ImGui::Render();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // CUBE
        // it is important to set them to identity matrices at the begininng of the loop since we will modify them
        // glm::mat4 model      = glm::mat4(1.0f); // the model is already defined in the loop
        glm::mat4 view = glm::mat4(1.0f);


        // cube_position.z -= 0.005f; // moves the cube 0.005 units backwards every frame

        // model = glm::translate(model, cube_position); // useful for the cube struct
        // model = glm::rotate(model, glm::radians(cube_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // useful for the cube struct (x rotation)
        // model = glm::rotate(model, glm::radians(cube_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // useful for the cube struct (y rotation)
        // model = glm::rotate(model, glm::radians(cube_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // useful for the cube struct (z rotation)
        // model = glm::scale(model, cube_scale); // useful for the cube struct (scale)

        view = glm::lookAt(camPos, camFront + camPos, up);// view matrix, applied to the camera (-3.0f unit backwards)

        sponza_shader.use();

        sponza_shader.setVec3("cam_pos", camPos);
        sponza_shader.setFloat("material.shininess", 32.0f);

        // Directional light settings
        sponza_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
        sponza_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
        sponza_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
        sponza_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

        // Point light settings
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            std::string pos      = "point_lights[" + std::to_string(i) + "].position";
            std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
            std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
            std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
            std::string constant = "point_lights[" + std::to_string(i) + "].constant";
            std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
            std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

            constexpr float AMBIENT_INTENSITY = 0.1f;
            sponza_shader.setVec3(pos.c_str(), pointLightPositions[i]);
            sponza_shader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
            sponza_shader.setVec3(diff.c_str(), pointLightColors[i]);
            sponza_shader.setVec3(spec.c_str(), pointLightColors[i]);
            sponza_shader.setFloat(constant.c_str(), 1.0f);
            sponza_shader.setFloat(linear.c_str(), 0.09f);
            sponza_shader.setFloat(quad.c_str(), 0.032f);
        }

        // cubeShader.setMat4("model", model); // (as already set in the loop)
        sponza_shader.setMat4("view", view);
        sponza_shader.setMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(20, 0, 20));
        model           = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        sponza_shader.setMat4("model", model);
        sponza.draw(sponza_shader);

        backpack_shader.use();

        backpack_shader.setVec3("cam_pos", camPos);
        backpack_shader.setFloat("material.shininess", 32.0f);

        // Directional light settings
        backpack_shader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
        backpack_shader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
        backpack_shader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
        backpack_shader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

        // Point light settings
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            std::string pos      = "point_lights[" + std::to_string(i) + "].position";
            std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
            std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
            std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
            std::string constant = "point_lights[" + std::to_string(i) + "].constant";
            std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
            std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

            constexpr float AMBIENT_INTENSITY = 0.1f;
            backpack_shader.setVec3(pos.c_str(), pointLightPositions[i]);
            backpack_shader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
            backpack_shader.setVec3(diff.c_str(), pointLightColors[i]);
            backpack_shader.setVec3(spec.c_str(), pointLightColors[i]);
            backpack_shader.setFloat(constant.c_str(), 1.0f);
            backpack_shader.setFloat(linear.c_str(), 0.007f);
            backpack_shader.setFloat(quad.c_str(), 0.002f);
        }

        // cubeShader.setMat4("model", model); // (as already set in the loop)
        backpack_shader.setMat4("view", view);
        backpack_shader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, backpack_positions[i]);
            float angle     = 20.0f * i;
            model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model           = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            backpack_shader.setMat4("model", model);
            backpack_shader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));

            backpack.draw(backpack_shader);
        }

        // // 4 Point Lights
        lightShader.use();
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lightShader.setMat4("model", model);
            lightShader.setMat4("view", view);
            lightShader.setMat4("projection", projection);
            lightShader.setVec3("color", glm::vec3(pointLightColors[i]));

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        m_Window->post_update();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // TODO: Window automatically destructs itself here, so glfwTerminate() occurs here. NOTE!!
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow *window) {
    float speed = 5.f * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed *= 1.5f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += speed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= speed * camFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= speed * glm::normalize(glm::cross(camFront, up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += speed * glm::normalize(glm::cross(camFront, up));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        my_tool_active = true;
}

static void key_input_callback(GLFWwindow *window, int key, int scan_code, int action, int mods) {
}

static void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    if (first_time_mouse_enter) {
        lastX                  = x;
        lastY                  = y;
        first_time_mouse_enter = false;
    }
    // calculate how much the mouse has moved in both the x and y axes
    float xoffset = (float) x - lastX;
    float yoffset = -1.0 * ((float) y - lastY);// center is at top left, hence y goes negative

    lastX = x;
    lastY = y;

    const float mouse_sensitivity = 0.1f;
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // create the normalized vector of mouse movement
    glm::vec3 mouse_dir;
    mouse_dir.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    mouse_dir.y = sin(glm::radians(pitch));
    mouse_dir.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    camFront = glm::normalize(mouse_dir);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}
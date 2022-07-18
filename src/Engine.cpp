#include <Engine.hpp>
#include <sstream>
#include <string>

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


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


Engine::Engine(const EngineConfig config) : cfg(config) {}

Engine::~Engine() {}

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height);
static void processInput(GLFWwindow *window);
static void cursor_pos_callback(GLFWwindow *window, double x, double y);
unsigned int load_texture(const char *path);

glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

void Engine::start() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
            glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
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

#pragma region CUBE
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // normal data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture data
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);
#pragma endregion

#pragma region LIGHT 1
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
#pragma endregion


    // render loop
    // -----------
    Shader cubeShader("../res/shaders/cube.vs", "../res/shaders/cube.fs");
    Shader lightShader("../res/shaders/light.vs", "../res/shaders/light.fs");

    glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

    unsigned int diffuse_map  = load_texture("../res/textures/diffuse_map.png");
    unsigned int specular_map = load_texture("../res/textures/specular_map.png");
    unsigned int emission_map = load_texture("../res/textures/emission_map.png");
    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setInt("material.emission", 2);

    // cube's model info
    glm::vec3 cube_position(0.0f, 0.0f, 0.0f);
    glm::vec3 cube_rotation(0.0f, 0.0f, 0.0f);
    glm::vec3 cube_scale(1.0f, 1.0f, 1.0f);

    glm::vec3 colorRed    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 colorGreen  = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 colorBlue   = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 colorYellow = glm::vec3(1.0f, 1.0f, 0.0f);

    glm::vec3 pointLightColors[] = {colorRed, colorGreen, colorBlue, colorYellow};

    glm::vec3 cubePositions[] = {
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

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)};


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


    while (!glfwWindowShouldClose(window)) {
        // input
        // -----

        glfwPollEvents();
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

        cubeShader.use();

        cubeShader.setVec3("cam_pos", camPos);
        cubeShader.setFloat("material.shininess", 32.0f);

        // Directional light settings
        cubeShader.setVec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
        cubeShader.setVec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
        cubeShader.setVec3("directional_light.specular", 1.0f, 1.0f, 1.0f);

        // Point light settings
        for (int i = 0; i < 4; i++) {
            std::string pos      = "point_lights[" + std::to_string(i) + "].position";
            std::string am       = "point_lights[" + std::to_string(i) + "].ambient";
            std::string diff     = "point_lights[" + std::to_string(i) + "].diffuse";
            std::string spec     = "point_lights[" + std::to_string(i) + "].specular";
            std::string constant = "point_lights[" + std::to_string(i) + "].constant";
            std::string linear   = "point_lights[" + std::to_string(i) + "].linear";
            std::string quad     = "point_lights[" + std::to_string(i) + "].quadratic";

            cubeShader.setVec3(pos.c_str(), pointLightPositions[i]);
#define AMBIENT_INTENSITY 0.3f
            cubeShader.setVec3(am.c_str(), pointLightColors[i].r * AMBIENT_INTENSITY, pointLightColors[i].g * AMBIENT_INTENSITY, pointLightColors[i].b * AMBIENT_INTENSITY);
#undef AMBIENT_INTENSITY
            cubeShader.setVec3(diff.c_str(), pointLightColors[i]);
            cubeShader.setVec3(spec.c_str(), pointLightColors[i]);
            cubeShader.setFloat(constant.c_str(), 1.0f);
            cubeShader.setFloat(linear.c_str(), 0.09f);
            cubeShader.setFloat(quad.c_str(), 0.032f);
        }

        // cubeShader.setMat4("model", model); // (as already set in the loop)
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emission_map);

        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, cubePositions[i]);
            float angle     = 20.0f * i;
            model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cubeShader.setMat4("model", model);
            cubeShader.setMat3("model_corrected", glm::mat3(glm::transpose(glm::inverse(model))));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 4 Point Lights
        lightShader.use();
        for (int i = 0; i < 4; i++) {
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


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteTextures(1, &diffuse_map);
    glDeleteTextures(1, &specular_map);
    glDeleteTextures(1, &emission_map);
    glDeleteTextures(1, &emission_map);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
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

unsigned int load_texture(const char *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
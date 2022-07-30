#pragma once

#include "common.h"
#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(std::unique_ptr<Window> &window);
    virtual ~Camera() {}
    virtual void update();
    virtual glm::mat4 &get_view_matrix();
    virtual glm::mat4 &get_projection_matrix();
    //TODO: For now, later move them into protected!!
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

protected:
    std::unique_ptr<Window> &window;

    float YAW         = -90.0f;
    float PITCH       = 0.0f;
    float FOV         = 45.0f;
    float SENSITIVITY = 0.1f;

    glm::mat4 view, projection;

    void update_cam_dirs();

private:
    bool firstTimeEntered = true;
    float lastX, lastY;
    void mouse_callback_fn(double x, double y);
    void framebuffer_size_callback_fn(GLFWwindow *window, int width, int height);
    void key_callback_fn(GLFWwindow *window, int key, int scan_code, int action, int mods);
};

class FlyCamera : public Camera {
public:
    FlyCamera(const std::unique_ptr<Window> &window);
    ~FlyCamera();

private:
};
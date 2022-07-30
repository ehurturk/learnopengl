#include "Camera.h"

void Camera::mouse_callback_fn(double xpos, double ypos) {
    if (firstTimeEntered) {
        lastX            = xpos;
        lastY            = ypos;
        firstTimeEntered = false;
    }

    float offsetX = xpos - lastX;
    float offsetY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    offsetX *= sensitivity;
    offsetY *= sensitivity;

    YAW += offsetX;
    PITCH += offsetY;

    if (PITCH > 89.0f)
        PITCH = 89.0f;
    if (PITCH < -89.0f)
        PITCH = -89.0f;

    update_cam_dirs();
}

void Camera::framebuffer_size_callback_fn(GLFWwindow *window, int width, int height) {
    projection = glm::perspective(glm::radians(FOV), (float) width / (float) height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

void Camera::key_callback_fn(GLFWwindow *window, int key, int scan_code, int action, int mods) {
}

Camera::Camera(std::unique_ptr<Window> &window) : window(window), position(glm::vec3(0.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), lastX(window->config.width / 2), lastY(window->config.height / 2), projection(glm::perspective(glm::radians(FOV), (float) window->config.width / window->config.height, 0.1f, 100.0f)) {
    window->bind_func<Window::MouseInputCallbackFunc>([](GLFWwindow *window, double xpos, double ypos) {
        Camera *cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        if (cam != nullptr)
            cam->mouse_callback_fn(xpos, ypos);
    });
    window->bind_func<Window::FramebufferSizeCallbackFunc>([](GLFWwindow *window, int w, int h) {
        Camera *cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        if (cam != nullptr)
            cam->framebuffer_size_callback_fn(window, w, h);
    });
    window->bind_func<Window::KeyInputCallbackFunc>([](GLFWwindow *window, int k, int s, int a, int m) {
        Camera *cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        if (cam != nullptr)
            cam->key_callback_fn(window, k, s, a, m);
    });
    update_cam_dirs();
}

void Camera::update() {
    view = glm::lookAt(position, position + front, up);
}

void Camera::update_cam_dirs() {
    glm::vec3 direction;
    direction.x = glm::cos(glm::radians(YAW)) * glm::cos(glm::radians(PITCH));
    direction.y = glm::sin(glm::radians(PITCH));
    direction.z = glm::sin(glm::radians(YAW)) * glm::cos(glm::radians(PITCH));

    front = glm::normalize(direction);
}

glm::mat4 &Camera::get_projection_matrix() { return projection; }
glm::mat4 &Camera::get_view_matrix() { return view; }

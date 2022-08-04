#include "graphics/SkyBox.h"

SkyBox::SkyBox() {
    shader.create();
    shader.load_shader("../res/shaders/skybox.vs", Shader::ShaderType::VERTEX_SHADER);
    shader.load_shader("../res/shaders/skybox.fs", Shader::ShaderType::FRAGMENT_SHADER);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
void SkyBox::load_faces(const std::array<const char *, 6> &faces) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int w, h, n_channels;
    for (int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i], &w, &h, &n_channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "ERROR: Failed to load cubemap image at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    shader.use();
    shader.setInt("skybox", 0);
}

// NOTE: Always draw the skybox last!!!
void SkyBox::draw() {
    glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content
    shader.use();
    const auto &camera = Engine::Get().get_subsystem<Camera3D>();
    auto view          = glm::mat4(glm::mat3(camera->get_view_matrix()));
    auto proj          = camera->get_projection_matrix();
    auto projview      = proj * view;
    shader.setMat4("projview", projview);

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);// set depth function back to default
}

SkyBox::~SkyBox() {
    glDeleteTextures(1, &id);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
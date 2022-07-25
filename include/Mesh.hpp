#pragma once

#include "common.h"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normals;
    glm::vec2 uv;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex> &vertices, const std::vector<ui32> &indices, const std::vector<Texture> &textures);
    ~Mesh();
    void draw(const Shader &shader) const;
    void destroy() const;

private:
    ui32 vao, vbo, ebo;
    void setup_mesh();
};
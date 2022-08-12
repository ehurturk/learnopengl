#include "Mesh.hpp"
#include <string>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<ui32> &indices, const std::vector<Texture> &textures) : vertices(vertices), indices(indices), textures(textures) {
    setup_mesh();
}

Mesh::~Mesh() {}

void Mesh::setup_mesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normals));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
    // vertex tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    // vertex bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

void Mesh::draw(const Shader &shader) const {
    ui32 no_diffuse  = 1;
    ui32 no_specular = 1;
    ui32 no_emission = 1;
    ui32 no_normal   = 1;
    ui32 no_height   = 1;

    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string n, base_name;
        base_name = textures[i].type;
        if (std::strcmp(base_name.c_str(), "texture_diffuse") == 0)
            n = std::to_string(no_diffuse++);
        else if (std::strcmp(base_name.c_str(), "material.texture_specular") == 0)
            n = std::to_string(no_specular++);// transfer unsigned int to string
        else if (std::strcmp(base_name.c_str(), "material.texture_emission") == 0)
            n = std::to_string(no_emission++);// transfer unsigned int to string
        else if (std::strcmp(base_name.c_str(), "material.texture_normal") == 0)
            n = std::to_string(no_normal++);// transfer unsigned int to string
        else if (std::strcmp(base_name.c_str(), "material.texture_height") == 0)
            n = std::to_string(no_height++);// transfer unsigned int to string
        else
            n = "";// whatever the basename is
        shader.setInt(base_name + n, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::destroy() const {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    for (const auto &texture : textures)
        glDeleteTextures(1, &texture.id);
}
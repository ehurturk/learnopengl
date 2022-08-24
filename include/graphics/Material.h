#pragma once

#include "Shader.hpp"
#include "common.h"
#include "Resource.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

class Material {
public:
    Material(const Shader &shader);
    ~Material();

    void set_texture(const std::string &name, const Texture &texture);
    void set_value(const std::string &name, int value);
    void set_value(const std::string &name, float value);
    void set_value(const std::string &name, bool value);
    void set_value(const std::string &name, const glm::mat4 &value);
    void set_value(const std::string &name, const glm::mat3 &value);
    void set_value(const std::string &name, const glm::vec4 &value);
    void set_value(const std::string &name, const glm::vec3 &value);

    void bind();

private:
    Shader *shader;
};

class MaterialInstance {
public:
    MaterialInstance(Material &mat);
    ~MaterialInstance();

private:
    Material *mat;
};
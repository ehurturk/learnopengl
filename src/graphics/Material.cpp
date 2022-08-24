#include "graphics/Material.h"

Material::Material(const Shader &shader){

};

Material::~Material() {
}

void Material::bind() {
    shader->use();
}

void Material::set_texture(const std::string &name, const Texture &texture) {
}

void Material::set_value(const std::string &name, int value) {
}

void Material::set_value(const std::string &name, float value) {
}

void Material::set_value(const std::string &name, bool value) {
}

void Material::set_value(const std::string &name, const glm::mat4 &value) {
}

void Material::set_value(const std::string &name, const glm::mat3 &value) {
}

void Material::set_value(const std::string &name, const glm::vec4 &value) {
}

void Material::set_value(const std::string &name, const glm::vec3 &value) {
}
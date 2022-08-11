#pragma once

#include "common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Entity {
public:
    Entity() {}
    virtual ~Entity() {}

    virtual void draw() = 0;

    inline const std::string &get_name() { return name; }
    inline glm::vec3 &get_position() { return position; }
    inline glm::vec3 &get_rotation() { return rotation; }
    inline glm::vec3 &get_scale() { return scale; }

protected:
    std::string name;
    glm::vec3 position, rotation, scale;
};
#pragma once

#include "Entity.hpp"
#include "common.h"

class Scene {
public:
    Scene();
    ~Scene();

    void add_entity(Entity &entity);

    void render();

    void save(const std::string &path);
    void load(const std::string &path);

private:
    std::vector<Entity *> m_Entities;
};
#pragma once

#include "Entity.hpp"
#include "common.h"

class Scene {
public:
    Scene();
    ~Scene();

    void add_entity(Entity &entity);

    Entity &get_entity_by_name(const std::string &name);
    Entity &get_entity_by_id(const std::string &id);

    void render();

    void save(const std::string &path);
    void load(const std::string &path);

private:
    std::vector<Entity *> m_Entities;
};
#include "Scene.h"
#include <fstream>
#include <string>

Scene::Scene() {
}

void Scene::render() {
    for (const auto &entity : m_Entities) {
        entity->draw();
    }
}

void Scene::save(const std::string &path) {
    // save entities
    std::ofstream out;
    out.open(path);
    for (const auto &entity : m_Entities) {
        out << entity->get_name() << ":\n";
        out << "\tposition: ";
        out << "[" + std::to_string(entity->get_position().x) + ", " + std::to_string(entity->get_position().y) + ", " + std::to_string(entity->get_position().z) + "]\n";
        out << "\trotation: ";
        out << "[" + std::to_string(entity->get_rotation().x) + ", " + std::to_string(entity->get_rotation().y) + ", " + std::to_string(entity->get_rotation().z) + "]\n";
        out << "\tscale:";
        out << "[" + std::to_string(entity->get_scale().x) + ", " + std::to_string(entity->get_scale().y) + ", " + std::to_string(entity->get_scale().z) + "]\n";
    }
    out.close();
}

void Scene::add_entity(Entity &entity) {
    m_Entities.push_back(&entity);
}

void Scene::load(const std::string &path) {
    // load entities
    std::ifstream file;
    file.open(path);

    file.close();
}
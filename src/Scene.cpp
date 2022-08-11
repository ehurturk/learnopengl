#include "Scene.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <string>

Scene::Scene() {
}

void Scene::save(const std::string &path) {
    std::ofstream file;
    YAML::Emitter out;
    for (const auto &entity : m_Entities) {
        out << YAML::BeginSeq;
        out << entity->get_name();
        out << YAML::BeginMap;
        out << YAML::Key << "position";
        out << YAML::Value << "(" + std::to_string(entity->get_position().x) + ", " + std::to_string(entity->get_position().y) + ", " + std::to_string(entity->get_position().z) + ")";
        out << YAML::Key << "rotation";
        out << YAML::Value << "(" + std::to_string(entity->get_rotation().x) + ", " + std::to_string(entity->get_rotation().y) + ", " + std::to_string(entity->get_rotation().z) + ")";
        out << YAML::Key << "scale";
        out << YAML::Value << "(" + std::to_string(entity->get_scale().x) + ", " + std::to_string(entity->get_scale().y) + ", " + std::to_string(entity->get_scale().z) + ")";
    }
    std::cout << out.c_str() << std::endl;
    file.open(path);
    file << out.c_str();
    file.close();
}

void Scene::add_entity(Entity &entity) {
    m_Entities.push_back(&entity);
}

void Scene::load(const std::string &path) {
    YAML::Node scene = YAML::LoadFile(path);
    // load entities
    for (YAML::const_iterator it = scene.begin(); it != scene.end(); ++it) {
    }
}
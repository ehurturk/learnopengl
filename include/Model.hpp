#pragma once

#include "common.h"
#include "Shader.hpp"
#include "Resource.hpp"
#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>


class Model {
public:
    Model();
    ~Model();
    void draw(const Shader &shader);
    void load_model(const std::string &path);

    void add_texture(const std::string &path);
    void add_texture(const Texture &tex);
    void add_texture(unsigned int tex, const std::string &base_name);

private:
    enum class ModelType {
        UNKNOWN = -1,
        OBJ,
        GLTF,
    };

    void process_node(aiNode *node, const aiScene *scene, ModelType type);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene, ModelType type);
    std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                                Texture::TextureType tex_type);

    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
};
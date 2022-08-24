#include "Model.hpp"
#include <string>

Model::Model() {}

Model::~Model() {
    for (const auto &mesh : meshes) {
        mesh.destroy();
    }
}

void Model::draw(const Shader &shader) {
    for (const auto &mesh : meshes)
        mesh.draw(shader);
}

void Model::load_model(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory       = path.substr(0, path.find_last_of("/"));
    size_t ext_     = path.find_last_of(".");
    std::string ext = path.substr(ext_ + 1);
    if (ext == "obj")
        process_node(scene->mRootNode, scene, ModelType::OBJ);
    else if (ext == "gltf" || ext == "fbx")
        process_node(scene->mRootNode, scene, ModelType::GLTF);
    else {
        std::cout << "WARNING: File type may have inconsistent normal texture extractions." << std::endl;
        process_node(scene->mRootNode, scene, ModelType::GLTF);
    }
}

// TODO Complete these!
void Model::add_texture(const std::string &path) {
    Texture tex = ResourceManager::load_ogl_texture_from_path(path);
    add_texture(tex);
}

void Model::add_texture(const Texture &tex) {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].textures.push_back(tex);
    }
}

void Model::add_texture(unsigned int tex, const std::string &base_name, bool array) {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].textures.push_back({ .type = base_name, .id = tex, .array = array });
    }
}

void Model::process_node(aiNode *node, const aiScene *scene, ModelType type) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene, type));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, type);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene, ModelType type) {
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<ui32> indices;

    // fill in vertex data form the model info
    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 temp_vec;
        // get the position data
        temp_vec.x      = mesh->mVertices[i].x;
        temp_vec.y      = mesh->mVertices[i].y;
        temp_vec.z      = mesh->mVertices[i].z;
        vertex.position = temp_vec;
        // get the normal data
        if (mesh->HasNormals()) {
            temp_vec.x     = mesh->mNormals[i].x;
            temp_vec.y     = mesh->mNormals[i].y;
            temp_vec.z     = mesh->mNormals[i].z;
            vertex.normals = temp_vec;
        }
        // get the uv data
        if (mesh->mTextureCoords[0]) {
            glm::vec2 uv_data;
            uv_data.x = mesh->mTextureCoords[0][i].x;
            uv_data.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = uv_data;
        } else
            vertex.uv = glm::vec2(0.0f, 0.0f);
        if (mesh->HasTangentsAndBitangents()) {
            // tangent
            temp_vec.x     = mesh->mTangents[i].x;
            temp_vec.y     = mesh->mTangents[i].y;
            temp_vec.z     = mesh->mTangents[i].z;
            vertex.tangent = temp_vec;
            // bitangent
            temp_vec.x       = mesh->mBitangents[i].x;
            temp_vec.y       = mesh->mBitangents[i].y;
            temp_vec.z       = mesh->mBitangents[i].z;
            vertex.bitangent = temp_vec;
        }
        vertices.push_back(vertex);
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat                   = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = load_material_textures(mat, aiTextureType_DIFFUSE, Texture::TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        std::vector<Texture> specular_maps = load_material_textures(mat, aiTextureType_SPECULAR, Texture::TextureType::SPECULAR);
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
        std::vector<Texture> emission_maps = load_material_textures(mat, aiTextureType_EMISSIVE, Texture::TextureType::EMISSION);
        textures.insert(textures.end(), emission_maps.begin(), emission_maps.end());

        std::vector<Texture> normal_maps;
        switch (type) {
            case ModelType::OBJ:
                normal_maps = load_material_textures(mat, aiTextureType_HEIGHT, Texture::TextureType::NORMAL);
                textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
                break;
            case ModelType::GLTF:
            case ModelType::UNKNOWN:
                normal_maps = load_material_textures(mat, aiTextureType_NORMALS, Texture::TextureType::NORMAL);
                textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
                break;
        }
        std::vector<Texture> height_maps = load_material_textures(mat, aiTextureType_AMBIENT, Texture::TextureType::HEIGHT);
        textures.insert(textures.end(), height_maps.begin(), height_maps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial *mat, aiTextureType type, Texture::TextureType tex_type) {
    std::vector<Texture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            bool srgb;
            if (tex_type == Texture::TextureType::DIFFUSE || tex_type == Texture::TextureType::SPECULAR || tex_type == Texture::TextureType::EMISSION)
                srgb = true;
            else
                srgb = false;

            Texture tex = ResourceManager::load_ogl_texture_from_path(directory + "/" + str.C_Str(), tex_type, srgb);
            tex.path    = str.C_Str();
            textures.push_back(tex);
            textures_loaded.push_back(tex);
        }
    }

    return textures;
}

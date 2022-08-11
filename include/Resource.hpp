#pragma once

#include "thirdparty/stb_image.h"
#include "common.h"

#include <fstream>
#include <glad/glad.h>
#include <string>

struct Image {
    unsigned char *data;
    ui32 width;
    ui32 height;
    ui32 no_components;
};

struct Texture {
    enum TextureType {
        DIFFUSE,
        SPECULAR,
        EMISSION,
        NORMAL,
        HEIGHT
    };
    std::string type;
    ui32 id;
    std::string path;
};

struct ResourceManager {
    static Image load_image_from_path(const std::string &path);
    static Texture load_ogl_texture_from_path(const std::string &path, Texture::TextureType type = Texture::TextureType::DIFFUSE, bool srgb = true);
    static const char *load_file(const std::string &path);
};
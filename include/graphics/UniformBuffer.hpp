#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "common.h"

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void init(unsigned int size);

    // bind specific portion of the buffer (usage: multiple uniform blocks)
    void bind(int binding, size_t offset, size_t size);
    // bind all the buffer (usage: specific uniform blocks)
    void bind(int binding);

    template<typename T>
    void load_data(const T &data);
    template<typename T>
    void set_data(const T &data, size_t offset);

    template<>
    void load_data<glm::mat4>(const glm::mat4 &data);
    template<>
    void set_data<glm::mat4>(const glm::mat4 &data, size_t offset);

    template<>
    void load_data<glm::vec3>(const glm::vec3 &data);

    template<>
    void load_data<int>(const int &data);

    template<>
    void load_data<float>(const float &data);

private:
    unsigned int id;
    unsigned int offset = 0;
    unsigned int b_size;
};
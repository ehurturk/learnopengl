#include "graphics/UniformBuffer.hpp"
#include "glm/gtc/type_ptr.hpp"

UniformBuffer::UniformBuffer() {
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &id);
}

void UniformBuffer::init(unsigned int size) {
    b_size = size;
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bind(int binding, size_t offset, size_t size) {
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, id, offset, size);
}

void UniformBuffer::bind(int binding) {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}

template<>
void UniformBuffer::load_data<glm::mat4>(const glm::mat4 &data) {
    if (offset + sizeof(glm::mat4) > b_size) {
        std::cout << "Uniform buffer size is not enough to add a mat4." << std::endl;
        return;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(data));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    offset += sizeof(glm::mat4);
}

template<>
void UniformBuffer::set_data<glm::mat4>(const glm::mat4 &data, size_t offset) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(data));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
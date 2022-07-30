#include "Shader.hpp"
#include "common.h"
#include "glm/gtc/type_ptr.hpp"

Shader::Shader() {}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::create() {
    ID = glCreateProgram();
}

void Shader::load_shader(const char *path, ShaderType type) {
    std::string code_;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream shader_stream;
        shader_stream << file.rdbuf();
        file.close();
        code_ = shader_stream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char *code = code_.c_str();
    ui32 shader_id;
    switch (type) {
        case ShaderType::VERTEX_SHADER:
            shader_id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(shader_id, 1, &code, NULL);
            glCompileShader(shader_id);
            checkCompileErrors(shader_id, "VERTEX");
            break;
        case ShaderType::FRAGMENT_SHADER:
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(shader_id, 1, &code, NULL);
            glCompileShader(shader_id);
            checkCompileErrors(shader_id, "FRAGMENT");
            break;
        default:
            std::cout << "ERROR: Shader type not supported yet (ERR_INFO:PATH:" << path << ")." << std::endl;
            break;
    }

    glAttachShader(ID, shader_id);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shader as it is linked into the program now and no longer necessary
    glDeleteShader(shader_id);
}

// activate the shader
// ------------------------------------------------------------------------
void Shader::use() {
    glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_uniform_block_binding(const std::string &name, int binding) const {
    unsigned int ubo_index = glGetUniformBlockIndex(ID, name.c_str());
    glUniformBlockBinding(ID, ubo_index, binding);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
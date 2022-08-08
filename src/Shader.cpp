#include "Shader.hpp"
#include "common.h"
#include "glm/gtc/type_ptr.hpp"
#include <sstream>
#include <string>

Shader::Shader() {}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::create() {
    ID = glCreateProgram();
}

void Shader::load_shader(const char *path) {
    // std::string code_;
    // std::ifstream file;
    // file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // try {
    //     file.open(path);
    //     std::stringstream shader_stream;
    //     shader_stream << file.rdbuf();
    //     code_ = shader_stream.str();
    // } catch (std::ifstream::failure &e) {
    //     std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    // }
    // const char *code = code_.c_str();
    std::ifstream file(path);
    std::string line;
    std::stringstream vertex;
    std::stringstream fragment;
    int cur_type = 0;
    while (std::getline(file, line)) {

        if (line == "<FRAGMENT>")
            cur_type = 1;
        else if (line == "<VERTEX>")
            cur_type = 0;
        if (cur_type == 0 && (line != "<VERTEX>"))
            vertex << line << "\n";
        else if (cur_type == 1 && line != "<FRAGMENT>")
            fragment << line << "\n";
    }
    vertex << "\0";
    fragment << "\0";
    file.close();


    std::string vertex_code = vertex.str().c_str();
    std::string frag_code   = fragment.str().c_str();

    const GLchar *vcode = vertex_code.c_str();
    const GLchar *fcode = frag_code.c_str();

    ui32 vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vcode, NULL);
    glCompileShader(vertex_shader_id);
    checkCompileErrors(vertex_shader_id, "VERTEX");
    glAttachShader(ID, vertex_shader_id);
    ui32 frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader_id, 1, &fcode, NULL);
    glCompileShader(frag_shader_id);
    checkCompileErrors(frag_shader_id, "FRAGMENT");
    glAttachShader(ID, frag_shader_id);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shader as it is linked into the program now and no longer necessary
    glDeleteShader(vertex_shader_id);
    glDeleteShader(frag_shader_id);
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
#include "ShaderProgram.h"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

ShaderProgram::ShaderProgram(const Shader& vs, const Shader& fs) {
    programId = glCreateProgram();
    glAttachShader(programId, vs.getId());
    glAttachShader(programId, fs.getId());
    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(programId, logLen, nullptr, log.data());
        throw std::runtime_error("Shader linking failed: " + std::string(log.data()));
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programId);
}

void ShaderProgram::useProgram() const {
    glUseProgram(programId);
}

GLuint ShaderProgram::getId() const {
    return programId;
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    glUniform4fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
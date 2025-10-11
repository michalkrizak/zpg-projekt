#include "ShaderProgram.h"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

ShaderProgram::ShaderProgram(const Shader& vs, const Shader& fs) {
    programId = glCreateProgram();

    vs.attachShaderToProgram(*this);
    fs.attachShaderToProgram(*this);

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

    glDetachShader(programId, vs.shaderId);
    glDetachShader(programId, fs.shaderId);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programId);
}

void ShaderProgram::useProgram() const {
    glUseProgram(programId);
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform4fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::onCameraChanged(const glm::mat4& view, const glm::mat4& projection) {
    cachedView = view;
    cachedProj = projection;
    useProgram();
    setUniform("view", cachedView);
    setUniform("projection", cachedProj);
}

void ShaderProgram::setInitialViewProj(const glm::mat4& view, const glm::mat4& projection) {
    cachedView = view;
    cachedProj = projection;
}

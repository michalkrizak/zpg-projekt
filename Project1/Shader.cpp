#include "Shader.h"
#include <stdexcept>
#include <vector>
#include "ShaderProgram.h"

Shader::Shader(GLenum type, const char* src) {
    shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(shaderId, logLen, nullptr, log.data());
        throw std::runtime_error("Shader compilation failed: " + std::string(log.data()));
    }
}

Shader::~Shader() {
    glDeleteShader(shaderId);
}

void Shader::attachShaderToProgram(ShaderProgram& program) const {
    glAttachShader(program.programId, shaderId);
}

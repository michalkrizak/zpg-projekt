#include "ShaderProgram.h"
#include <stdexcept>
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

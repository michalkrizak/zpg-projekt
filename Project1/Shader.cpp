#include "Shader.h"
#include <stdexcept>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ShaderProgram.h"

Shader::Shader() : shaderId(0) {
}

Shader::Shader(GLenum type, const char* src) {
    createShader(type, src);
}

Shader Shader::fromFile(GLenum type, const char* filePath) {
    Shader shader;
    shader.createShaderFromFile(type, filePath);
    return shader;
}

void Shader::createShader(GLenum shaderType, const char* shaderCode) {
    // Creates an empty shader
    shaderId = glCreateShader(shaderType);
    // Sets the source code of the shader.
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    // Compiles the shader source code
    glCompileShader(shaderId);
    
    // Check compilation status
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

void Shader::createShaderFromFile(GLenum shaderType, const char* shaderFile) {
    // Loading the contents of a file into a variable
    std::ifstream file(shaderFile);
    if (!file.is_open()) {
        std::cout << "Unable to open file " << shaderFile << std::endl;
        exit(-1);
    }
    std::string shaderCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    createShader(shaderType, shaderCode.c_str());
}

Shader::~Shader() {
    if (shaderId != 0) {
        glDeleteShader(shaderId);
    }
}

void Shader::attachShaderToProgram(ShaderProgram& program) const {
    glAttachShader(program.programId, shaderId);
}

#include "Shader.h"
#include <stdexcept>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader() : shaderId(0) {
}

Shader::Shader(GLuint id) : shaderId(id) {
}

Shader* Shader::createFromSource(GLenum type, const char* source) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(id, logLen, nullptr, log.data());
        glDeleteShader(id);
        throw std::runtime_error("Shader compilation failed: " + std::string(log.data()));
    }
    
    return new Shader(id);
}

Shader* Shader::createFromFile(GLenum type, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + std::string(filename));
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    return createFromSource(type, source.c_str());
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

void Shader::attachShader(GLuint programID) const {
    glAttachShader(programID, shaderId);
}

void Shader::detachShader(GLuint programID) const {
    glDetachShader(programID, shaderId);
}

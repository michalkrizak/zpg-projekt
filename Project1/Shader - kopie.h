#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
public:
    // Factory methods for creating shaders
    static Shader* createFromSource(GLenum type, const char* source);
    static Shader* createFromFile(GLenum type, const char* filename);
    
    // Explicit constructor with shader ID
    explicit Shader(GLuint id);
    
    ~Shader();

    // Attach this shader to a program by programID
    void attachShader(GLuint programID) const;
    // Detach this shader from a program by programID
    void detachShader(GLuint programID) const;

private:
    GLuint shaderId;
    
    // Private default constructor
    Shader();
    
    // Core shader creation method
    void createShader(GLenum shaderType, const char* shaderCode);
    
    // Load shader code from file
    void createShaderFromFile(GLenum shaderType, const char* shaderFile);
};

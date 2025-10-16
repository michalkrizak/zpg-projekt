#pragma once
#include <GL/glew.h>
#include <string>

class ShaderProgram; // forward declaration


class Shader {
public:
    // Constructor for inline shader code (backward compatibility)
    Shader(GLenum type, const char* src);
    
    // Constructor for loading shader from file
    static Shader fromFile(GLenum type, const char* filePath);
    
    ~Shader();

    void attachShaderToProgram(ShaderProgram& program) const;

private:
    friend class ShaderProgram; // allow ShaderProgram to access shaderId for optional detach
    GLuint shaderId;
    
    // Private default constructor for factory method
    Shader();
    
    // Core shader creation method
    void createShader(GLenum shaderType, const char* shaderCode);
    
    // Load shader code from file
    void createShaderFromFile(GLenum shaderType, const char* shaderFile);
};

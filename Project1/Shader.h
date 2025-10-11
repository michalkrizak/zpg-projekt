#pragma once
#include <GL/glew.h>
#include <string>

class ShaderProgram; // forward declaration


class Shader {
public:
    Shader(GLenum type, const char* src);
    ~Shader();

    void attachShaderToProgram(ShaderProgram& program) const;

private:
    friend class ShaderProgram; // allow ShaderProgram to access shaderId for optional detach
    GLuint shaderId;
};

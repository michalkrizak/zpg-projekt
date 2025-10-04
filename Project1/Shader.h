#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
public:
    Shader(GLenum type, const char* src);
    ~Shader();

    GLuint getId() const;

private:
    GLuint shaderId;
};

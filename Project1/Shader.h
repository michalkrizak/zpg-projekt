#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void use() const;
    GLuint getProgram() const;

private:
    GLuint program;
    GLuint compileShader(GLenum type, const char* src);
};

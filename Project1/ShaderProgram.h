#pragma once
#include <GL/glew.h>
#include "Shader.h"

class ShaderProgram {
public:
    ShaderProgram(const Shader& vs, const Shader& fs);
    ~ShaderProgram();

    void useProgram() const;
    GLuint getId() const;

private:
    GLuint programId;
};

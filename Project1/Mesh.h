#pragma once
#include <GL/glew.h>

class Mesh {
public:
    Mesh(const float* vertices, size_t size, int vertexSize);
    ~Mesh();

    void draw() const;

private:
    GLuint VAO, VBO;
    int count;
};

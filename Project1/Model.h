#pragma once
#include <GL/glew.h>
#include <vector>

class Model {
public:
    Model(const float* vertices, size_t size, int vertexSize);
    ~Model();

    void bindModel() const;
    int getCount() const;

private:
    GLuint VAO, VBO;
    int count;
};


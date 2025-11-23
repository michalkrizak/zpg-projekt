#pragma once
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <string>

class Model {
public:
    Model(const float* vertices, size_t size, int vertexSize);
    ~Model();

    void bindModel() const;
    int getCount() const;

    static std::unique_ptr<Model> loadFromOBJ(const std::string& path);

private:
    GLuint VAO, VBO;
    int count;
};


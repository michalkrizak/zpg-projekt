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

    // Loads a simple OBJ file (supports v, vn, f with triangulation; vt/materials ignored)
    // Returns nullptr on failure. Vertex format: position (3) + normal (3) = 6 floats per vertex.
    static std::unique_ptr<Model> loadFromOBJ(const std::string& path);

private:
    GLuint VAO, VBO;
    int count;
};


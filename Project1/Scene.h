#pragma once
#include <vector>
#include <memory>
#include "DrawableObject.h"
#include <glm/glm.hpp>

class Scene {
public:
    void addObject(std::unique_ptr<DrawableObject> obj);
    void drawAll() const;
    void drawAll(const glm::vec3& viewPos) const;

    std::vector<std::shared_ptr<ShaderProgram>> getShaderPrograms() const;

private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
};

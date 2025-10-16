#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

class TransformComponent {
public:
    virtual ~TransformComponent() = default;
    virtual glm::mat4 getMatrix() const = 0;
};


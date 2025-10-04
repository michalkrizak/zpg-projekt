#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

class AbstractTransformation {
public:
    virtual ~AbstractTransformation() = default;
    virtual glm::mat4 getMatrix() const = 0;
};


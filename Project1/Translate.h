#pragma once
#include "TransformComponent.h"

class Translate : public TransformComponent {
public:
    Translate(float x, float y, float z) : x(x), y(y), z(z) {}
    glm::mat4 getMatrix() const override {
        return glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    }
private:
    float x, y, z;
};

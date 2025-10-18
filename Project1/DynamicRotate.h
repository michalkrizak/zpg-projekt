#pragma once
#include "TransformComponent.h"
#include <GLFW/glfw3.h>

class DynamicRotate : public TransformComponent {
public:
    DynamicRotate(float angleSpeed, float x, float y, float z)
        : angleSpeed(angleSpeed), axis(x, y, z) {}

    glm::mat4 getMatrix() const override {
        float t = static_cast<float>(glfwGetTime());
        return glm::rotate(glm::mat4(1.0f), angleSpeed * t, axis);
    }

private:
    float angleSpeed;
    glm::vec3 axis;
};

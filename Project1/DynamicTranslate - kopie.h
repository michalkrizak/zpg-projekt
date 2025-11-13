#pragma once
#include "TransformComponent.h"
#include <GLFW/glfw3.h>

class DynamicTranslate : public TransformComponent {
public:
    DynamicTranslate(float ax, float ay, float az, float speed = 1.0f)
        : ax(ax), ay(ay), az(az), speed(speed) {}

    glm::mat4 getMatrix() const override {
        float t = static_cast<float>(glfwGetTime());
        return glm::translate(glm::mat4(1.0f), glm::vec3(ax * sin(speed * t), ay * sin(speed * t), az * sin(speed * t)));
    }

private:
    float ax, ay, az;   // amplitudy
    float speed;        // rychlost
};

#pragma once
#include "AbstractTransformation.h"
#include <GLFW/glfw3.h>

class DynamicScale : public AbstractTransformation {
public:
    DynamicScale(float ax, float ay, float az, float speed = 1.0f)
        : ax(ax), ay(ay), az(az), speed(speed) {}

    glm::mat4 getMatrix() const override {
        float t = static_cast<float>(glfwGetTime());
        float sx = 1.0f + ax * sin(speed * t);
        float sy = 1.0f + ay * sin(speed * t);
        float sz = 1.0f + az * sin(speed * t);
        return glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
    }

private:
    float ax, ay, az;   // amplitudy
    float speed;        // rychlost
};

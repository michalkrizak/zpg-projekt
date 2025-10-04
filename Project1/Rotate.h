#pragma once
#include "AbstractTransformation.h"
#include <glm/gtc/matrix_transform.hpp>

class Rotate : public AbstractTransformation {
public:
    Rotate(float angle, float x, float y, float z)
        : angle(angle), axis(x, y, z) {}

    glm::mat4 getMatrix() const override {
        return glm::rotate(glm::mat4(1.0f), angle, axis);
    }

private:
    float angle;      // úhel v radiánech
    glm::vec3 axis;   // osa rotace
};

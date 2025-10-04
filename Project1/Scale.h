#pragma once
#include "AbstractTransformation.h"
#include <glm/gtc/matrix_transform.hpp>

class Scale : public AbstractTransformation {
public:
    Scale(float sx, float sy, float sz)
        : sx(sx), sy(sy), sz(sz) {}

    glm::mat4 getMatrix() const override {
        return glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
    }

private:
    float sx, sy, sz;   // škálovací faktory
};

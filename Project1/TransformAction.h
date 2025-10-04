#pragma once
#include "AbstractTransformation.h"

class TransformAction {
public:
    void addTransformation(std::unique_ptr<AbstractTransformation> t);
    glm::mat4 getModelMatrix() const;

private:
    std::vector<std::unique_ptr<AbstractTransformation>> transformations;
};

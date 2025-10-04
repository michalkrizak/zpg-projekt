#pragma once
#include "AbstractTransformation.h"

class TransformAction : public AbstractTransformation {
public:
    void addTransformation(std::unique_ptr<AbstractTransformation> t);

    glm::mat4 getMatrix() const override;

private:
    std::vector<std::unique_ptr<AbstractTransformation>> transformations;
};




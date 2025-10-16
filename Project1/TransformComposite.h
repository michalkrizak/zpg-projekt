#pragma once
#include "TransformComponent.h"

class TransformComposite : public TransformComponent {
public:
    void addTransformation(std::unique_ptr<TransformComponent> t);

    glm::mat4 getMatrix() const override;

private:
    std::vector<std::unique_ptr<TransformComponent>> transformations;
};

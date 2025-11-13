#include "TransformComposite.h"

glm::mat4 TransformComposite::getMatrix() const {
    glm::mat4 model(1.0f);
    for (const auto& t : transformations) {
        model = model * t->getMatrix();
    }
    return model;
}

void TransformComposite::addTransformation(std::unique_ptr<TransformComponent> t) {
    transformations.push_back(std::move(t));
}

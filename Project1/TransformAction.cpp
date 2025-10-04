#include "TransformAction.h"

glm::mat4 TransformAction::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    for (const auto& t : transformations) {
        model = model * t->getMatrix();
    }
    return model;
}

void TransformAction::addTransformation(std::unique_ptr<AbstractTransformation> t) {
    transformations.push_back(std::move(t));
}

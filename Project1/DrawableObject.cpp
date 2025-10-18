#include "DrawableObject.h"

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp)
    : model(std::move(m)), shaderProgram(std::move(sp)), modelType(0)
{}

void DrawableObject::draw() const {
    shaderProgram->useProgram();

    // Pošleme modelovou matici do shaderu pomocí setUniform
    shaderProgram->setUniform("model", transform.getMatrix());
    shaderProgram->setUniform("modelType", modelType);
    shaderProgram->setUniform("objectColor", color);

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
}

void DrawableObject::draw(const glm::vec3& viewPos) const {
    shaderProgram->useProgram();

    // Pošleme modelovou matici, modelType a viewPos do shaderu
    shaderProgram->setUniform("model", transform.getMatrix());
    shaderProgram->setUniform("modelType", modelType);
    shaderProgram->setUniform("viewPos", viewPos);
    shaderProgram->setUniform("objectColor", color);

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
}

TransformComposite& DrawableObject::getTransform() {
    return transform;
}

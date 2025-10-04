#include "DrawableObject.h"

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp)
    : model(std::move(m)), shaderProgram(std::move(sp))
{}

void DrawableObject::draw() const {
    shaderProgram->useProgram();

    // Pošleme modelovou matici do shaderu pomocí setUniform
    shaderProgram->setUniform("model", transform.getMatrix());

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
}

TransformAction& DrawableObject::getTransform() {
    return transform;
}

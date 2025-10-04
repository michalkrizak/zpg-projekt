#include "DrawableObject.h"

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp)
    : model(std::move(m)), shaderProgram(std::move(sp))
{}

void DrawableObject::draw() const {
    shaderProgram->useProgram();

    // Aplikace transformaèní matice do shaderu
    GLuint programId = shaderProgram->getId();
    GLint loc = glGetUniformLocation(programId, "model");
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &transform.getModelMatrix()[0][0]);
    }

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
}

TransformAction& DrawableObject::getTransform() {
    return transform;
}

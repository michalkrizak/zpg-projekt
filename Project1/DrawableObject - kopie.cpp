#include "DrawableObject.h"
#include <glm/gtc/matrix_inverse.hpp>

DrawableObject::DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp)
    : model(std::move(m)), shaderProgram(std::move(sp)), modelType(0)
{}

void DrawableObject::draw() const {
    shaderProgram->useProgram();

    // Calculate and send model matrix
    glm::mat4 modelMatrix = transform.getMatrix();
    shaderProgram->setUniform("model", modelMatrix);
    
    // Calculate and send normal matrix (transpose(inverse(model)))
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    shaderProgram->setUniform("normalMatrix", normalMatrix);
    
    shaderProgram->setUniform("modelType", modelType);
    shaderProgram->setUniform("objectColor", color);
    
    // Material coefficients
    shaderProgram->setUniform("ra", ra);
    shaderProgram->setUniform("rd", rd);
    shaderProgram->setUniform("rs", rs);
    shaderProgram->setUniform("h", h);

    // Texture support
    if (useTexture && texture) {
        texture->bind(0);
        shaderProgram->setUniform("useTexture", 1);
        shaderProgram->setUniform("textureSampler", 0);
    } else {
        shaderProgram->setUniform("useTexture", 0);
    }

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
    // Unbind program after drawing
    glUseProgram(0);
}

void DrawableObject::draw(const glm::vec3& viewPos) const {
    shaderProgram->useProgram();

    // Calculate and send model matrix
    glm::mat4 modelMatrix = transform.getMatrix();
    shaderProgram->setUniform("model", modelMatrix);
    
    // Calculate and send normal matrix (transpose(inverse(model)))
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    shaderProgram->setUniform("normalMatrix", normalMatrix);
    
    shaderProgram->setUniform("modelType", modelType);
    shaderProgram->setUniform("viewPos", viewPos);
    
    // If selected, highlight the object
    glm::vec3 finalColor = isSelected ? color * 1.5f : color;
    shaderProgram->setUniform("objectColor", finalColor);
    
    // Material coefficients
    shaderProgram->setUniform("ra", ra);
    shaderProgram->setUniform("rd", rd);
    shaderProgram->setUniform("rs", rs);
    shaderProgram->setUniform("h", h);

    // Texture support
    if (useTexture && texture) {
        texture->bind(0);
        shaderProgram->setUniform("useTexture", 1);
        shaderProgram->setUniform("textureSampler", 0);
    } else {
        shaderProgram->setUniform("useTexture", 0);
    }

    model->bindModel();
    glDrawArrays(GL_TRIANGLES, 0, model->getCount());
    // Unbind program after drawing
    glUseProgram(0);
}

TransformComposite& DrawableObject::getTransform() {
    return transform;
}

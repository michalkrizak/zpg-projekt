#pragma once
#include "Model.h"
#include "ShaderProgram.h"
#include "TransformComposite.h"
#include <glm/glm.hpp>

class DrawableObject {
public:
    DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp);
    virtual ~DrawableObject() = default;

    virtual void draw() const;
    virtual void draw(const glm::vec3& viewPos) const;

    TransformComposite& getTransform();

    std::shared_ptr<ShaderProgram> getShaderProgram() const { return shaderProgram; }

    void setModelType(int type) { modelType = type; }
    int getModelType() const { return modelType; }
    void setColor(const glm::vec3& c) { color = c; }
    glm::vec3 getColor() const { return color; }

protected:
    std::unique_ptr<Model> model;
    std::shared_ptr<ShaderProgram> shaderProgram;
    TransformComposite transform;
    int modelType = 0; // 0=Constant, 1=Lambert, 2=Phong, 3=Blinn
    glm::vec3 color{0.8f, 0.8f, 0.8f};
};

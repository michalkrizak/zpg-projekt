#pragma once
#include "Model.h"
#include "ShaderProgram.h"
#include "TransformAction.h"

class DrawableObject {
public:
    DrawableObject(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp);
    virtual ~DrawableObject() = default;

    virtual void draw() const;

    TransformAction& getTransform();

protected:
    std::unique_ptr<Model> model;
    std::shared_ptr<ShaderProgram> shaderProgram;
    TransformAction transform;
};

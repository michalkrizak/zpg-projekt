#pragma once
#include "DrawableObject.h"
#include "DynamicLight.h"
#include "TransformComposite.h"
#include "Translate.h"
#include "Scale.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <memory>

class Firefly : public DrawableObject, public DynamicLight {
public:
    Firefly(std::unique_ptr<Model> model,
        std::shared_ptr<ShaderProgram> program,
        const glm::vec3& centerPos,
        const glm::vec3& color,
        float radius,
        float speed,
        float phaseOffset = 0.0f,
        float visualScaleVal = 0.03f,
        float verticalAmplitude = 0.15f)
        : DrawableObject(std::move(model), std::move(program))
        , DynamicLight(centerPos, color * 0.3f, radius, speed, phaseOffset, verticalAmplitude)
        , visualScale(visualScaleVal)
    {
        // Visual setup: color and initial transform
        DrawableObject::setColor(color);
        auto initial = std::make_unique<TransformComposite>();
        initial->addTransformation(std::make_unique<Translate>(centerPos.x, centerPos.y, centerPos.z));
        initial->addTransformation(std::make_unique<Scale>(visualScale, visualScale, visualScale));
        getTransform().addTransformation(std::move(initial));
    }

    void update() {
        DynamicLight::update();
        const glm::vec3 pos = getPosition();
        auto xform = std::make_unique<TransformComposite>();
        xform->addTransformation(std::make_unique<Translate>(pos.x, pos.y, pos.z));
        xform->addTransformation(std::make_unique<Scale>(visualScale, visualScale, visualScale));
        getTransform() = TransformComposite();
        getTransform().addTransformation(std::move(xform));
    }

private:
    float visualScale;
};

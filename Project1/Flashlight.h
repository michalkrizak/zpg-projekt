#pragma once
#include "LightTypes.h"
#include <glm/glm.hpp>

class Flashlight {
public:
    Flashlight(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 0.9f),
        float intensity = 1.5f,
        float cutOffAngle = 12.5f,
        float outerCutOffAngle = 17.5f)
        : lightData(LightData::createSpot(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
            color, cutOffAngle, outerCutOffAngle, intensity))
        , isOn(false)
    {
        // Nastavení útlumu pro baterku (silnější světlo, ale kratší dosah)
        lightData.constant = 1.0f;
        lightData.linear = 0.09f;
        lightData.quadratic = 0.032f;
    }

    void toggle() {
        isOn = !isOn;
        lightData.enabled = isOn;
    }

    void turnOn() {
        isOn = true;
        lightData.enabled = true;
    }

    void turnOff() {
        isOn = false;
        lightData.enabled = false;
    }

    bool getIsOn() const {
        return isOn;
    }

    void updatePosition(const glm::vec3& position) {
        lightData.position = position;
    }

    void updateDirection(const glm::vec3& direction) {
        lightData.direction = glm::normalize(direction);
    }

    void updateFromCamera(const glm::vec3& cameraPos, const glm::vec3& cameraFront) {
        lightData.position = cameraPos;
        lightData.direction = glm::normalize(cameraFront);
    }

    const LightData& getLightData() const {
        return lightData;
    }

    LightData& getLightData() {
        return lightData;
    }

private:
    LightData lightData;
    bool isOn;
};

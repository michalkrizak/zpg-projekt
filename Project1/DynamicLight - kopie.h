#pragma once
#include "Light.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

class DynamicLight : public Light {
public:
    DynamicLight(const glm::vec3& centerPos, const glm::vec3& color,
        float radius, float speed, float phaseOffset = 0.0f,
        float verticalAmp = 0.3f)
        : Light(centerPos, color)
        , centerPosition(centerPos)
        , orbitRadius(radius)
        , orbitSpeed(speed)
        , phase(phaseOffset)
        , verticalAmplitude(verticalAmp)
    {}

    void update() {
        float time = static_cast<float>(glfwGetTime());
        float angle = time * orbitSpeed + phase;

        // Kruhový pohyb v XZ rovinì
        float x = centerPosition.x + orbitRadius * std::cos(angle);
        float y = centerPosition.y + verticalAmplitude * std::sin(angle * 2.0f); // Vertikální kolísání
        float z = centerPosition.z + orbitRadius * std::sin(angle);

        setPosition(glm::vec3(x, y, z));
    }

private:
    glm::vec3 centerPosition;
    float orbitRadius;
    float orbitSpeed;
    float phase;
    float verticalAmplitude;
};

#pragma once
#include <glm/glm.hpp>

enum class LightType {
    POINT,        // Point light
    DIRECTIONAL,  // (sun, moon)
    SPOT,         // Reflector (flashlight)
    AMBIENT       // Ambient
};

struct LightData {
    LightType type;
    glm::vec3 position;      // For POINT and SPOT
    glm::vec3 direction;     // For DIRECTIONAL and SPOT
    glm::vec3 color;
    float intensity;

    // for SPOT light
    float cutOff;    
    float outerCutOff;

    // Attenuation
    float constant;
    float linear;
    float quadratic;

    bool enabled;           

    LightData(const glm::vec3& pos, const glm::vec3& col, float inten = 1.0f)
        : type(LightType::POINT)
        , position(pos)
        , direction(glm::vec3(0.0f))
        , color(col)
        , intensity(inten)
        , cutOff(0.0f)
        , outerCutOff(0.0f)
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f)
        , enabled(true)
    {}

    static LightData createDirectional(const glm::vec3& dir, const glm::vec3& col, float inten = 1.0f) {
        LightData light(glm::vec3(0.0f), col, inten);
        light.type = LightType::DIRECTIONAL;
        light.direction = glm::normalize(dir);
        return light;
    }

    static LightData createSpot(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col,
        float cutOffAngle = 12.5f, float outerCutOffAngle = 17.5f, float inten = 1.0f) {
        LightData light(pos, col, inten);
        light.type = LightType::SPOT;
        light.direction = glm::normalize(dir);
        light.cutOff = glm::radians(cutOffAngle);
        light.outerCutOff = glm::radians(outerCutOffAngle);
        light.constant = 1.0f;
        light.linear = 0.09f;
        light.quadratic = 0.032f;
        return light;
    }


    static LightData createAmbient(const glm::vec3& col, float inten = 0.1f) {
        LightData light(glm::vec3(0.0f), col, inten);
        light.type = LightType::AMBIENT;
        return light;
    }
};

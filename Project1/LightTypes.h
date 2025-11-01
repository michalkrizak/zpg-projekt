#pragma once
#include <glm/glm.hpp>

enum class LightType {
    POINT,        // Bodov� sv�tlo
    DIRECTIONAL,  // Sm�rov� sv�tlo (slunce, m�s�c)
    SPOT,         // Reflektor (baterka)
    AMBIENT       // Ambientn� osv�tlen�
};

struct LightData {
    LightType type;
    glm::vec3 position;      // Pro POINT a SPOT
    glm::vec3 direction;     // Pro DIRECTIONAL a SPOT
    glm::vec3 color;
    float intensity;

    // Pro SPOT sv�tlo (baterka)
    float cutOff;            // Vnit�n� �hel ku�ele (v radi�nech)
    float outerCutOff;       // Vn�j�� �hel ku�ele (v radi�nech)

    // Attenuation (�tlum) pro POINT a SPOT
    float constant;
    float linear;
    float quadratic;

    bool enabled;            // Zapnuto/vypnuto

    // Konstruktor pro bodov� sv�tlo
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

    // Konstruktor pro sm�rov� sv�tlo
    static LightData createDirectional(const glm::vec3& dir, const glm::vec3& col, float inten = 1.0f) {
        LightData light(glm::vec3(0.0f), col, inten);
        light.type = LightType::DIRECTIONAL;
        light.direction = glm::normalize(dir);
        return light;
    }

    // Konstruktor pro reflektor (baterka)
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

    // Konstruktor pro ambientn� sv�tlo
    static LightData createAmbient(const glm::vec3& col, float inten = 0.1f) {
        LightData light(glm::vec3(0.0f), col, inten);
        light.type = LightType::AMBIENT;
        return light;
    }
};

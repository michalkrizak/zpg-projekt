#pragma once
#include <glm/glm.hpp>

class ILightObserver {
public:
    virtual ~ILightObserver() = default;
    virtual void onLightChanged(const glm::vec3& position, const glm::vec3& color) = 0;
};

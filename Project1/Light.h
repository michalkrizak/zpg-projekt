#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "LightObserver.h"

class Light {
public:
    Light(const glm::vec3& position, const glm::vec3& color);

    glm::vec3 getPosition() const;
    glm::vec3 getColor() const;

    void setPosition(const glm::vec3& pos);
    void setColor(const glm::vec3& col);

    void addObserver(const std::shared_ptr<ILightObserver>& obs);
    void removeObserver(const std::shared_ptr<ILightObserver>& obs);

    void notifyObservers();

private:
    glm::vec3 position;
    glm::vec3 color;
    std::vector<std::weak_ptr<ILightObserver>> observers;
};

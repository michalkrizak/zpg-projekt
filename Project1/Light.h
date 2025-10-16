#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "LightObserver.h"

class Light {
public:
    Light(const glm::vec3& position, const glm::vec3& color)
        : position(position), color(color) {}

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }

    void setPosition(const glm::vec3& pos) {
        position = pos;
        notifyObservers();
    }

    void setColor(const glm::vec3& col) {
        color = col;
        notifyObservers();
    }

    void addObserver(const std::shared_ptr<ILightObserver>& obs) {
        observers.emplace_back(obs);
    }

    void removeObserver(const std::shared_ptr<ILightObserver>& obs) {
        observers.erase(std::remove_if(observers.begin(), observers.end(), 
            [&](const std::weak_ptr<ILightObserver>& w) {
                auto sp = w.lock();
                return !sp || sp == obs;
            }), observers.end());
    }

    void notifyObservers() {
        for (auto it = observers.begin(); it != observers.end(); ) {
            if (auto sp = it->lock()) {
                sp->onLightChanged(position, color);
                ++it;
            } else {
                it = observers.erase(it);
            }
        }
    }

private:
    glm::vec3 position;
    glm::vec3 color;
    std::vector<std::weak_ptr<ILightObserver>> observers;
};

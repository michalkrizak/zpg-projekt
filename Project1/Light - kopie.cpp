#include "Light.h"
#include <algorithm>

Light::Light(const glm::vec3& position, const glm::vec3& color)
	: position(position), color(color) {}

glm::vec3 Light::getPosition() const { return position; }
glm::vec3 Light::getColor() const { return color; }

void Light::setPosition(const glm::vec3& pos) {
	position = pos;
	notifyObservers();
}

void Light::setColor(const glm::vec3& col) {
	color = col;
	notifyObservers();
}

void Light::addObserver(const std::shared_ptr<ILightObserver>& obs) {
	observers.emplace_back(obs);
}

void Light::removeObserver(const std::shared_ptr<ILightObserver>& obs) {
	observers.erase(std::remove_if(observers.begin(), observers.end(),
		[&](const std::weak_ptr<ILightObserver>& w) {
			auto sp = w.lock();
			return !sp || sp == obs;
		}), observers.end());
}

void Light::notifyObservers() {
	for (auto it = observers.begin(); it != observers.end(); ) {
		if (auto sp = it->lock()) {
			sp->onLightChanged(position, color);
			++it;
		}
		else {
			it = observers.erase(it);
		}
	}
}


#include "Scene.h"

void Scene::addObject(std::unique_ptr<DrawableObject> obj) {
    objects.push_back(std::move(obj));
}

void Scene::drawAll() const {
    for (const auto& obj : objects) {
        obj->draw();
    }
}

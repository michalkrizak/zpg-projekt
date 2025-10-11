#include "Scene.h"
#include <unordered_set>

void Scene::addObject(std::unique_ptr<DrawableObject> obj) {
    objects.push_back(std::move(obj));
}

void Scene::drawAll() const {
    for (const auto& obj : objects) {
        obj->draw();
    }
}

std::vector<std::shared_ptr<ShaderProgram>> Scene::getShaderPrograms() const {
    std::vector<std::shared_ptr<ShaderProgram>> result;
    std::unordered_set<ShaderProgram*> seen;
    result.reserve(objects.size());
    for (const auto& obj : objects) {
        auto sp = obj->getShaderProgram();
        if (sp && seen.insert(sp.get()).second) {
            result.push_back(sp);
        }
    }
    return result;
}

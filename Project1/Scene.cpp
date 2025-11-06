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

void Scene::drawAll(const glm::vec3& viewPos) const {
    for (const auto& obj : objects) {
        obj->draw(viewPos);
    }
}

void Scene::drawAll(const glm::vec3& viewPos, const glm::mat4& view, const glm::mat4& projection) const {
    // Draw skybox first (will be behind everything)
    if (skybox) {
        skybox->draw(view, projection);
    }
    
    // Draw all objects
    for (const auto& obj : objects) {
        obj->draw(viewPos);
    }
}

void Scene::setSkybox(std::unique_ptr<Skybox> sky) {
    skybox = std::move(sky);
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

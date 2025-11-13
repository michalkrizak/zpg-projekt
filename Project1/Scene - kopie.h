#pragma once
#include <vector>
#include <memory>
#include "DrawableObject.h"
#include "Skybox.h"
#include <glm/glm.hpp>

class Scene {
public:
    void addObject(std::unique_ptr<DrawableObject> obj);
    void drawAll() const;
    void drawAll(const glm::vec3& viewPos) const;
    void drawAll(const glm::vec3& viewPos, const glm::mat4& view, const glm::mat4& projection) const;

    std::vector<std::shared_ptr<ShaderProgram>> getShaderPrograms() const;
    
    void setSkybox(std::unique_ptr<Skybox> sky);
    
    // Object picking and selection
    void drawAllWithStencil(const glm::vec3& viewPos) const;
    DrawableObject* getObjectByID(unsigned int id);
    void clearSelection();
    bool deleteSelectedObject();
    
    const std::vector<std::unique_ptr<DrawableObject>>& getObjects() const { return objects; }

private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::unique_ptr<Skybox> skybox;
};

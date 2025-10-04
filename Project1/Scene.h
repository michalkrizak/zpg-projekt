#pragma once
#include <vector>
#include <memory>
#include "DrawableObject.h"

class Scene {
public:
    void addObject(std::unique_ptr<DrawableObject> obj);
    void drawAll() const;

private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
};

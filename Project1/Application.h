#pragma once
#include "Scene.h"
#pragma once
#include "Scene.h"
#include "Window.h"
#include <vector>
#include <memory>
#include "Camera.h"

class Application {
public:
    Application(int width, int height, const std::string& title);
    void run();

    void addScene(std::unique_ptr<Scene> scene);
    void setActiveScene(size_t index);

private:
    Window window;
    std::vector<std::unique_ptr<Scene>> scenes;
    size_t activeSceneIndex = 0;
    std::shared_ptr<Camera> camera;
};

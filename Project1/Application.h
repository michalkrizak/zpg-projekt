#pragma once
#include "Scene.h"
#include "Window.h"
#include <vector>
#include <memory>
#include "Camera.h"

class Light;

class Application {
public:
    Application(int width, int height, const std::string& title);
    void initialize();
    void run();

    void addScene(std::unique_ptr<Scene> scene);
    void setActiveScene(size_t index);

    std::shared_ptr<Camera> getCamera() const { return camera; }

    Window& getWindow() { return window; }

private:
    void createForestScene();
    void createSphereScene();
    void createTriangleScene();
    void createSolarScene();

    Window window;
    std::vector<std::unique_ptr<Scene>> scenes;
    size_t activeSceneIndex = 0;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Light> mainLight;
};

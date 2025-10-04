#pragma once
#include "Scene.h"
#include "Window.h"

class Application {
public:
    Application(int width, int height, const std::string& title);
    void run();

    Scene& getScene();

private:
    Window window;
    Scene scene;
};

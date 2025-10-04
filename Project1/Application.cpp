#include "Application.h"
#include <GL/glew.h>

Application::Application(int width, int height, const std::string& title)
    : window(width, height, title)
{}

void Application::run() {
    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        scene.drawAll();

        window.swapBuffers();
        window.pollEvents();
    }
}

Scene& Application::getScene() {
    return scene;
}

// Application.cpp
#include "Application.h"
#include <GL/glew.h>
#include <iostream>

static Application* g_app = nullptr;

// Pøepínání scén pomocí kláves 1–3
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && g_app) {
        switch (key) {
        case GLFW_KEY_1: g_app->setActiveScene(0); break;
        case GLFW_KEY_2: g_app->setActiveScene(1); break;
        case GLFW_KEY_3: g_app->setActiveScene(2); break;
        case GLFW_KEY_4: g_app->setActiveScene(3); break;
        }
    }
}

Application::Application(int width, int height, const std::string& title)
    : window(width, height, title)
{
    g_app = this;
    glfwSetKeyCallback(window.getGLFWwindow(), keyCallback);

    // Nastavení poèáteèního viewportu
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
}

void Application::addScene(std::unique_ptr<Scene> scene) {
    scenes.push_back(std::move(scene));
}

void Application::setActiveScene(size_t index) {
    if (index < scenes.size()) {
        activeSceneIndex = index;
        std::cout << "Pøepnuto na scénu " << index << std::endl;
    }
}

void Application::run() {
    while (!window.shouldClose()) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!scenes.empty() && activeSceneIndex < scenes.size()) {
            scenes[activeSceneIndex]->drawAll();
        }

        window.swapBuffers();
        window.pollEvents();
    }
}

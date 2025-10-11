#include "Application.h"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>

static Application* g_app = nullptr;
// Simple mouse state for right-button look
static bool g_rightMouseDown = false;
static double g_lastX = 0.0, g_lastY = 0.0;

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (!g_app) return;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            g_rightMouseDown = true;
            glfwGetCursorPos(window, &g_lastX, &g_lastY);
        } else if (action == GLFW_RELEASE) {
            g_rightMouseDown = false;
        }
    }
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!g_app || !g_rightMouseDown) return;
    float xoffset = static_cast<float>(xpos - g_lastX);
    float yoffset = static_cast<float>(g_lastY - ypos);
    g_lastX = xpos;
    g_lastY = ypos;
    if (auto cam = g_app->getCamera()) {
        cam->processMouseMovement(xoffset, yoffset);
    }
}

static void processWSAD(GLFWwindow* win, std::shared_ptr<Camera>& cam, float deltaTime) {
    if (!cam) return;
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) cam->processKeyboard('W', deltaTime);
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) cam->processKeyboard('S', deltaTime);
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) cam->processKeyboard('A', deltaTime);
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) cam->processKeyboard('D', deltaTime);
}

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
    glfwSetMouseButtonCallback(window.getGLFWwindow(), mouseButtonCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), cursorPosCallback);

    camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
}

void Application::addScene(std::unique_ptr<Scene> scene) {
    scenes.push_back(std::move(scene));
    if (camera) {
        int fbWidth=0, fbHeight=0; glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight>0? static_cast<float>(fbWidth)/fbHeight : 1.0f;
        auto& scn = scenes.back();
        for (auto& sp : scn->getShaderPrograms()) {
            sp->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
            camera->addObserver(sp);
        }
    }
}

void Application::setActiveScene(size_t index) {
    if (index < scenes.size()) {
        activeSceneIndex = index;
        std::cout << "P�epnuto na sc�nu " << index << std::endl;
        if (camera) {
            int fbWidth=0, fbHeight=0; glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
            float aspect = fbHeight>0? static_cast<float>(fbWidth)/fbHeight : 1.0f;
            for (auto& sp : scenes[activeSceneIndex]->getShaderPrograms()) {
                sp->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
                camera->addObserver(sp);
            }
        }
    }
}

void Application::run() {
    float lastTime = static_cast<float>(glfwGetTime());
    while (!window.shouldClose()) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentTime = static_cast<float>(glfwGetTime());
        float delta = currentTime - lastTime;
        lastTime = currentTime;

        // WSAD input
        processWSAD(window.getGLFWwindow(), camera, delta);

        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.0f;
    if (auto cam = getCamera()) cam->notifyObservers(aspect);

        if (!scenes.empty() && activeSceneIndex < scenes.size()) {
            scenes[activeSceneIndex]->drawAll();
        }

        window.swapBuffers();
        window.pollEvents();
    }
}

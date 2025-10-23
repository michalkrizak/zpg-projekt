#include "Application.h"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>
#include <random>
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Translate.h"
#include "Scale.h"
#include "DynamicRotate.h"
#include "Light.h"
#include "tree.h"
#include "bushes.h"
#include "plain.h"
#include "sphere.h"

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

    // Create main light for all scenes
    mainLight = std::make_shared<Light>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.9f));
}

void Application::initialize() {
    createForestScene();
    createSphereScene();
    createTriangleScene();
    createSolarScene();
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
            if (camera) {
                scenes[activeSceneIndex]->drawAll(camera->getPosition());
            } else {
                scenes[activeSceneIndex]->drawAll();
            }
        }

        window.swapBuffers();
        window.pollEvents();
    }
}

void Application::createForestScene() {
    // Forest shaders
    Shader* vsForest = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsForest = Shader::createFromFile(GL_FRAGMENT_SHADER, "phong.frag");
    auto programForest = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsForest, fsForest});

    // Ground shaders
    Shader* vsg = Shader::createFromFile(GL_VERTEX_SHADER, "ground.vert");
    Shader* fsg = Shader::createFromFile(GL_FRAGMENT_SHADER, "ground.frag");
    auto programGround = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsg, fsg});

    auto forest = std::make_unique<Scene>();

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
    std::uniform_real_distribution<float> distScale(0.15f, 0.35f);

    // Ground plane
    {
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), programGround);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(30.0f, 1.0f, 30.0f));
        ground->getTransform().addTransformation(std::move(t));
        forest->addObject(std::move(ground));
    }

    // 150 trees
    for (int i = 0; i < 150; ++i) {
        auto m = std::make_unique<Model>(tree, treeDataSize, 6);
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        float x = distPos(rng);
        float z = distPos(rng);
        float s = distScale(rng);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(x, -1.0f, z));
        t->addTransformation(std::make_unique<Scale>(s, s, s));
        obj->getTransform().addTransformation(std::move(t));
        forest->addObject(std::move(obj));
    }

    // 150 bushes
    for (int i = 0; i < 150; ++i) {
        auto m = std::make_unique<Model>(bushes, bushesDataSize, 6);
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        float x = distPos(rng);
        float z = distPos(rng);
        float s = distScale(rng) * 3.0f;
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(x, -1.0f, z));
        t->addTransformation(std::make_unique<Scale>(s, s, s));
        obj->getTransform().addTransformation(std::move(t));
        forest->addObject(std::move(obj));
    }

    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programForest));
    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programGround));
    mainLight->notifyObservers();

    addScene(std::move(forest));
}

void Application::createSphereScene() {
    auto sphereScene = std::make_unique<Scene>();
    float sphereDistance = 2.0f;
    float sphereScale = 1.0f;

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "universal.frag");
    auto programUniversal = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsUniversal});

    // Sphere 1: Constant
    auto m1 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj1 = std::make_unique<DrawableObject>(std::move(m1), programUniversal);
    auto t1 = std::make_unique<TransformComposite>();
    t1->addTransformation(std::make_unique<Translate>(sphereDistance, 0.0f, 0.0f));
    t1->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj1->getTransform().addTransformation(std::move(t1));
    obj1->setModelType(0);
    obj1->setColor(glm::vec3(1.0f, 0.8f, 0.2f));
    sphereScene->addObject(std::move(obj1));

    // Sphere 2: Lambert
    auto m2 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj2 = std::make_unique<DrawableObject>(std::move(m2), programUniversal);
    auto t2 = std::make_unique<TransformComposite>();
    t2->addTransformation(std::make_unique<Translate>(-sphereDistance, 0.0f, 0.0f));
    t2->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj2->getTransform().addTransformation(std::move(t2));
    obj2->setModelType(1);
    obj2->setColor(glm::vec3(0.2f, 0.8f, 0.2f));
    sphereScene->addObject(std::move(obj2));

    // Sphere 3: Phong
    auto m3 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj3 = std::make_unique<DrawableObject>(std::move(m3), programUniversal);
    auto t3 = std::make_unique<TransformComposite>();
    t3->addTransformation(std::make_unique<Translate>(0.0f, sphereDistance, 0.0f));
    t3->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj3->getTransform().addTransformation(std::move(t3));
    obj3->setModelType(2);
    obj3->setColor(glm::vec3(0.2f, 0.6f, 1.0f));
    sphereScene->addObject(std::move(obj3));

    // Sphere 4: Blinn-Phong
    auto m4 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj4 = std::make_unique<DrawableObject>(std::move(m4), programUniversal);
    auto t4 = std::make_unique<TransformComposite>();
    t4->addTransformation(std::make_unique<Translate>(0.0f, -sphereDistance, 0.0f));
    t4->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj4->getTransform().addTransformation(std::move(t4));
    obj4->setModelType(3);
    obj4->setColor(glm::vec3(0.9f, 0.3f, 0.3f));
    sphereScene->addObject(std::move(obj4));

    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programUniversal));
    mainLight->notifyObservers();

    // Additional lights for demonstration
    auto light2 = std::make_shared<Light>(glm::vec3(3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.3f, 0.3f));
    light2->addObserver(std::static_pointer_cast<ILightObserver>(programUniversal));
    light2->notifyObservers();

    auto light3 = std::make_shared<Light>(glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(0.3f, 1.0f, 0.3f));
    light3->addObserver(std::static_pointer_cast<ILightObserver>(programUniversal));
    light3->notifyObservers();

    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programUniversal->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programUniversal));
    }

    addScene(std::move(sphereScene));
}

void Application::createTriangleScene() {
    auto triangleScene = std::make_unique<Scene>();
    static const float triVerts[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    auto triModel = std::make_unique<Model>(triVerts, sizeof(triVerts), 3);
    Shader* vsTri = Shader::createFromFile(GL_VERTEX_SHADER, "tri.vert");
    Shader* fsTri = Shader::createFromFile(GL_FRAGMENT_SHADER, "tri.frag");
    auto programTri = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsTri, fsTri});
    auto triObj = std::make_unique<DrawableObject>(std::move(triModel), programTri);
    triangleScene->addObject(std::move(triObj));

    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programTri->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programTri));
    }

    addScene(std::move(triangleScene));
}

void Application::createSolarScene() {
    auto solarScene = std::make_unique<Scene>();

    Shader* vsSolar = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsSun = Shader::createFromFile(GL_FRAGMENT_SHADER, "constant.frag");
    Shader* fsPlanet = Shader::createFromFile(GL_FRAGMENT_SHADER, "color_phong.frag");
    auto programSun = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSolar, fsSun});
    auto programPlanet = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSolar, fsPlanet});

    // SUN
    {
        auto m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto sun = std::make_unique<DrawableObject>(std::move(m), programSun);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Scale>(1.2f, 1.2f, 1.2f));
        sun->getTransform().addTransformation(std::move(t));
        sun->setModelType(0);
        sun->setColor(glm::vec3(1.0f, 0.9f, 0.2f));
        solarScene->addObject(std::move(sun));
    }

    // EARTH orbiting around SUN
    float earthOrbitRadius = 5.0f;
    float earthOrbitSpeed = 0.6f;
    float earthScale = 0.5f;
    {
        auto m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto earth = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(earthScale, earthScale, earthScale));
        earth->getTransform().addTransformation(std::move(t));
        earth->setColor(glm::vec3(0.2f, 0.6f, 1.0f));
        solarScene->addObject(std::move(earth));
    }

    // MOON orbiting around EARTH
    float moonOrbitRadius = 1.5f;
    float moonOrbitSpeed = 1.8f;
    float moonScale = 0.18f;
    {
        auto m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto moon = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(moonOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(moonOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(moonScale, moonScale, moonScale));
        moon->getTransform().addTransformation(std::move(t));
        moon->setColor(glm::vec3(0.7f, 0.7f, 0.7f));
        solarScene->addObject(std::move(moon));
    }

    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programPlanet));
    mainLight->notifyObservers();

    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;

        programSun->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programSun));

        programPlanet->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programPlanet));
    }

    addScene(std::move(solarScene));
}

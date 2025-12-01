#include "Application.h"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Translate.h"
#include "Scale.h"
#include "Rotate.h"
#include "DynamicRotate.h"
#include "Light.h"
#include "DynamicLight.h"
#include "Firefly.h"
#include "Texture.h"
#include "Skybox.h"
#include "tree.h"
#include "bushes.h"
#include "plain.h"
#include "sphere.h"
#include "gift.h"
#include "LinePathTransform.h"
#include "BezierCurveTransform.h"
#include "BezierSplineTransform.h"
#include "Target.h"

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
    // Left mouse button for tree planting in scene 5, Bezier points in scene 6
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // Scene 5 is tree planting scene - use special handler
        if (g_app->getActiveSceneIndex() == 5) {
            g_app->plantTreeAtClick(xpos, ypos);
        } 
        // Scene 6 is formula scene - add Bezier control points
        else if (g_app->getActiveSceneIndex() == 6) {
            g_app->addBezierControlPoint(xpos, ypos);
        }
        else {
            g_app->handleMouseClick(xpos, ypos);
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
        case GLFW_KEY_5: g_app->setActiveScene(4); break; // backface test scene
        case GLFW_KEY_6: g_app->setActiveScene(5); break; // Tree planting scene
        case GLFW_KEY_7: g_app->setActiveScene(6); break; // Formula scene
        case GLFW_KEY_8: g_app->setActiveScene(7); break; // Arcade game scene
        
        // Delete selected object
        case GLFW_KEY_DELETE:
            g_app->deleteSelectedObject();
            break;
        
        // Flashlight control - F key
        case GLFW_KEY_F:
            g_app->getFlashlight().toggle();
            printf("Flashlight %s\n", g_app->getFlashlight().getIsOn() ? "ON" : "OFF");
            break;
        
        // FOV control - Q, E, R
        case GLFW_KEY_Q: 
            if (auto cam = g_app->getCamera()) {
                cam->setFOV(45.0f);
                int fbWidth = 0, fbHeight = 0;
                glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
                float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
                cam->notifyObservers(aspect);
                printf("FOV set to 45 degrees\n");
            }
            break;
        case GLFW_KEY_E:
            if (auto cam = g_app->getCamera()) {
                cam->setFOV(90.0f);
                int fbWidth = 0, fbHeight = 0;
                glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
                float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
                cam->notifyObservers(aspect);
                printf("FOV set to 90 degrees\n");
            }
            break;
        case GLFW_KEY_R:
            if (auto cam = g_app->getCamera()) {
                cam->setFOV(130.0f);
                int fbWidth = 0, fbHeight = 0;
                glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
                float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
                cam->notifyObservers(aspect);
                printf("FOV set to 130 degrees\n");
            }
            break;
        
        // Create custom Bezier path - C key (only in formula scene)
        case GLFW_KEY_C:
            if (g_app->getActiveSceneIndex() == 6) {
                g_app->createCustomBezierFormula();
            }
            break;
        
        // Clear Bezier control points - X key (only in formula scene)
        case GLFW_KEY_X:
            if (g_app->getActiveSceneIndex() == 6) {
                g_app->clearBezierControlPoints();
                printf("\n*** All Bezier control points cleared! ***\n");
                printf("Click in the scene to add new points.\n\n");
            }
            break;
        }
    }
}

Application::Application(int width, int height, const std::string& title)
    : window(width, height, title)
    , flashlight(glm::vec3(1.0f, 1.0f, 0.9f), 2.0f, 12.5f, 17.5f)
{
    g_app = this;
    glfwSetKeyCallback(window.getGLFWwindow(), keyCallback);
    glfwSetMouseButtonCallback(window.getGLFWwindow(), mouseButtonCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), cursorPosCallback);

    camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    // Create main light for all scenes - positioned high above, moonlight-like intensity
   mainLight = std::make_shared<Light>(
        glm::vec3(0.0f, 40.0f, 0.0f),
        glm::vec3(0.2f, 0.2f, 0.18f)
    );


    // 1. Ambient light (dark, nighttime environment)
    staticLights.push_back(LightData::createAmbient(glm::vec3(0.1f, 0.1f, 0.15f), 0.05f));
    
    // 2. Directional light (moon)
    staticLights.push_back(LightData::createDirectional(
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(0.15f, 0.15f, 0.2f),
        1.0f
    ));

}

void Application::initialize() {
    createForestScene();
    createSphereScene();
    createTriangleScene();
    createSolarScene();
    createBackfaceTestScene();
    createTreePlantingScene();
    createFormulaScene();
    createArcadeScene();
}

void Application::addScene(std::unique_ptr<Scene> scene) {
    // Assign unique IDs to pickable objects
    for (const auto& obj : scene->getObjects()) {
        obj->setID(nextObjectID++);
    }
    
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
        std::cout << "Prepnuto na scenu " << index << std::endl;
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
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        float currentTime = static_cast<float>(glfwGetTime());
        float delta = currentTime - lastTime;
        lastTime = currentTime;

        // WSAD input
        processWSAD(window.getGLFWwindow(), camera, delta);
        
        // Update flashlight position and direction based on camera
        if (camera) {
            flashlight.updateFromCamera(camera->getPosition(), camera->getFront());
        }
        
        // Update dynamic lights (fireflies)
        updateDynamicLights();

        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.0f;
        if (auto cam = getCamera()) cam->notifyObservers(aspect);

        if (!scenes.empty() && activeSceneIndex < scenes.size()) {
            if (camera) {
                // For formula scene (index 6) use rendering with skybox
                if (activeSceneIndex == 6) {
                    scenes[activeSceneIndex]->drawAll(camera->getPosition(), 
                                                     camera->getViewMatrix(), 
                                                     camera->getProjectionMatrix(aspect));
                } else {
                    scenes[activeSceneIndex]->drawAllWithStencil(camera->getPosition());
                }
            } else {
                scenes[activeSceneIndex]->drawAll();
            }
        }

        window.swapBuffers();
        window.pollEvents();
    }
}

void Application::createForestScene() {
    // Forest shaders - using advanced_lighting.frag for all light types support
    Shader* vsForest = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsForest = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto programForest = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsForest, fsForest});

    // Ground shaders
    Shader* vsg = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsg = Shader::createFromFile(GL_FRAGMENT_SHADER, "ground.frag");
    auto programGround = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsg, fsg});

    auto forest = std::make_unique<Scene>();

    auto groundTexture = std::make_shared<Texture>("assets/textures/grass.png");
    auto shrekTexture = std::make_shared<Texture>("assets/textures/shrek.png");
    auto fionaTexture = std::make_shared<Texture>("assets/textures/fiona.png");
    auto toiletTexture = std::make_shared<Texture>("assets/textures/toiled.jpg");

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
    std::uniform_real_distribution<float> distScale(0.15f, 0.35f);

    // Add important pickable objects FIRST (so they get low IDs < 255)
    if (auto m = Model::loadFromOBJ("assets/shrek.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(0.4f, 0.4f, 0.4f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setTexture(shrekTexture);
        forest->addObject(std::move(obj));
    }
    else {
        std::cerr << "Forest scene: failed to load assets/shrek.obj" << std::endl;
    }

    if (auto m = Model::loadFromOBJ("assets/fiona.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(1.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(0.4f, 0.4f, 0.4f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setTexture(fionaTexture);
        forest->addObject(std::move(obj));
    }
    else {
        std::cerr << "Forest scene: failed to load assets/fiona.obj" << std::endl;
    }

    if (auto m = Model::loadFromOBJ("assets/toiled.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(-1.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(0.4f, 0.4f, 0.4f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setTexture(toiletTexture);
        forest->addObject(std::move(obj));
    }
    else {
        std::cerr << "Forest scene: failed to load assets/toiled.obj" << std::endl;
    }

    // Ground plane with grass texture
    {
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), programForest);
        
        ground->setMaterial(0.1f, 1.0f, 0.02f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(30.0f, 1.0f, 30.0f));
        ground->getTransform().addTransformation(std::move(t));
        ground->setTexture(groundTexture);
        forest->addObject(std::move(ground));
    }

    // 120 trees
    for (int i = 0; i < 120; ++i) {
        auto m = std::make_unique<Model>(tree, treeDataSize, 6);
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        obj->setColor(glm::vec3(0.15f, 0.3f, 0.1f));
        float x = distPos(rng);
        float z = distPos(rng);
        float s = distScale(rng);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(x, -1.0f, z));
        t->addTransformation(std::make_unique<Scale>(s, s, s));
        obj->getTransform().addTransformation(std::move(t));
        forest->addObject(std::move(obj));
    }

    // 120 bushes
    for (int i = 0; i < 120; ++i) {
        auto m = std::make_unique<Model>(bushes, bushesDataSize, 6);
        auto obj = std::make_unique<DrawableObject>(std::move(m), programForest);
        obj->setColor(glm::vec3(0.1f, 0.25f, 0.08f));
        float x = distPos(rng);
        float z = distPos(rng);
        float s = distScale(rng) * 3.0f;
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(x, -1.0f, z));
        t->addTransformation(std::make_unique<Scale>(s, s, s));
        obj->getTransform().addTransformation(std::move(t));
        forest->addObject(std::move(obj));
    }


    // Create fireflies - glowing spheres with dynamic lights
    Shader* vsEmissive = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsEmissive = Shader::createFromFile(GL_FRAGMENT_SHADER, "emissive.frag");
    auto programEmissive = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsEmissive, fsEmissive});
    
    std::uniform_real_distribution<float> distFireflyPos(-12.0f, 12.0f);
    std::uniform_real_distribution<float> distFireflySpeed(0.3f, 1.2f);
    std::uniform_real_distribution<float> distFireflyPhase(0.0f, 6.28f);
    std::uniform_real_distribution<float> distFireflyRadius(1.5f, 3.5f);
    std::uniform_real_distribution<float> distFireflyHeight(0.05f, 0.35f);
    
    // Various colors for fireflies (green, yellow, warm white) - high intensity for visible light
    std::vector<glm::vec3> fireflyColors = {
        glm::vec3(2.0f, 1.8f, 0.5f)
    };
    
    // Create 12 fireflies as DrawableObject + DynamicLight
    for (int i = 0; i < 12; ++i) {
        float x = distFireflyPos(rng);
        float z = distFireflyPos(rng);
        float y = distFireflyHeight(rng); // Lower, randomized flight heights
        float speed = distFireflySpeed(rng);
        float phase = distFireflyPhase(rng);
        float radius = distFireflyRadius(rng);
        
        // Create Firefly object (derived from DrawableObject) and add to scene
        const float fireflyScale = 0.02f;
        auto fireflyModel = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto fireflyObj = std::make_unique<Firefly>(
            std::move(fireflyModel),
            programEmissive,
            glm::vec3(x, y, z),
            fireflyColors[i % fireflyColors.size()],
            radius,
            speed,
            phase,
            fireflyScale,
            0.15f
        );
        Firefly* fireflyPtr = fireflyObj.get();
        forest->addObject(std::move(fireflyObj));
        
    fireflyPtr->addObserver(std::static_pointer_cast<ILightObserver>(programForest));
    fireflyPtr->addObserver(std::static_pointer_cast<ILightObserver>(programGround));
    fireflyPtr->notifyObservers();
        
    // Store non-owning pointer for per-frame updates
    fireflies.push_back(fireflyPtr);
    }

    // Skydome - large hemisphere/sphere with sky texture
    /*auto skydomeTexture = std::make_shared<Texture>("assets/textures/skydome.png");
    Shader* vsSkydome = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsSkydome = Shader::createFromFile(GL_FRAGMENT_SHADER, "skydome.frag");
    auto programSkydome = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSkydome, fsSkydome});
    {
        auto m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto skydome = std::make_unique<DrawableObject>(std::move(m), programSkydome);
        skydome->setTexture(skydomeTexture);
        skydome->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(50.0f, 50.0f, 50.0f));
        skydome->getTransform().addTransformation(std::move(t));
        skydome->setModelType(0);
        forest->addObject(std::move(skydome));
    }*/

    addScene(std::move(forest));
}


void Application::createFormulaScene() {
    auto scene = std::make_unique<Scene>();

    Shader* vs = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fs = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto program = std::make_shared<ShaderProgram>(std::vector<Shader*>{vs, fs});

    // Load textures (if they exist, otherwise use colors only)
    auto formulaTexture = std::make_shared<Texture>("assets/textures/shaded.png");
    auto tungTexture = std::make_shared<Texture>("assets/textures/tung.png");
    auto treeTexture = std::make_shared<Texture>("assets/textures/tree1.png");
    auto groundTexture = std::make_shared<Texture>("assets/textures/grass.png");
    auto bubakTexture = std::make_shared<Texture>("assets/textures/bubak.png");

    // Try loading the Formula 1 OBJ with Bezier spline path
    auto model = Model::loadFromOBJ("assets/base.obj");
    if (model) {
        auto obj = std::make_unique<DrawableObject>(std::move(model), program);
        obj->setTexture(formulaTexture);
        auto t = std::make_unique<TransformComposite>();

        std::vector<glm::vec3> splinePoints = {
            glm::vec3(-6.0f, -1.0f, -6.0f), 
            glm::vec3(-6.0f, -1.0f, -3.0f),  
            glm::vec3(-5.0f, -1.0f, -1.0f), 
            glm::vec3(-2.0f, -1.0f, -2.0f), 

            glm::vec3(0.0f,  -1.0f, -3.0f),   
            glm::vec3(2.0f,  -1.0f, -5.0f),    
            glm::vec3(5.0f,  -1.0f, -4.0f),  

            glm::vec3(6.0f,  -1.0f, -2.0f),    
            glm::vec3(5.0f,  -1.0f,  0.0f),    
            glm::vec3(-2.0f, -1.0f,  1.0f),  

            glm::vec3(-5.0f, -1.0f,  2.0f),      
            glm::vec3(-5.0f, -1.0f,  6.0f),      
            glm::vec3(0.0f,  -1.0f,  6.0f),    

            glm::vec3(4.0f,  -1.0f,  5.0f),   
            glm::vec3(-4.0f,  -1.0f,  -14.0f),   
            glm::vec3(-6.0f, -1.0f, -6.0f)      
        };
        
        t->addTransformation(std::make_unique<BezierSplineTransform>(splinePoints, 20.0f, true, true));

        //t->addTransformation(std::make_unique<Rotate>(glm::radians(90.0f), 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(0.6f, 0.6f, 0.6f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2); // Phong shading
        obj->setColor(glm::vec3(0.9f, 0.1f, 0.1f)); 

        obj->setMaterial(0.2f, 0.8f, 0.9f, 128.0f);
        scene->addObject(std::move(obj));
    }
    else {
        std::cerr << "Formula scene: failed to load assets/formula1.obj" << std::endl;
    }

    // Load additional assets and place them around with textures
    // House with wood texture
    if (auto m = Model::loadFromOBJ("assets/tung.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), program);
        obj->setTexture(tungTexture);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(-3.5f, -1.0f, -4.0f));
        t->addTransformation(std::make_unique<Scale>(0.5f, 0.5f, 0.5f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setColor(glm::vec3(0.8f, 0.6f, 0.3f));
        scene->addObject(std::move(obj));
    }
    else {
        std::cerr << "Formula scene: failed to load assets/house.obj" << std::endl;
    }

    // Cube with metal texture
    if (auto m = Model::loadFromOBJ("assets/tree1.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), program);
        obj->setTexture(treeTexture);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(3.0f, -1.0f, -3.0f));
        t->addTransformation(std::make_unique<Scale>(0.7f, 0.7f, 0.7f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setColor(glm::vec3(0.7f, 0.7f, 0.9f));
        scene->addObject(std::move(obj));
    }
    else {
        std::cerr << "Formula scene: failed to load assets/tree1.obj" << std::endl;
    }

    // Square without texture (keep original)
    if (auto m = Model::loadFromOBJ("assets/bubak.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(m), program);
        obj->setTexture(bubakTexture);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(-1.0f, -1.0f, 2.5f));
        t->addTransformation(std::make_unique<Scale>(1.0f, 1.0f, 1.0f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setColor(glm::vec3(0.2f, 0.2f, 0.25f));
        scene->addObject(std::move(obj));
    }
    else {
        std::cerr << "Formula scene: failed to load assets/buabk.obj" << std::endl;
    }

    // Optional: simple ground under the model for reference with texture
    Shader* vsg = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsg = Shader::createFromFile(GL_FRAGMENT_SHADER, "ground.frag");
    auto groundProgram = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsg, fsg});
    {
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), groundProgram);
        ground->setTexture(groundTexture);
        ground->setColor(glm::vec3(0.12f, 0.12f, 0.12f));
        // Material: matte asphalt (low ambient, full diffuse, minimal specular, low shininess)
        ground->setMaterial(0.1f, 1.0f, 0.05f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(10.0f, 1.0f, 10.0f));
        ground->getTransform().addTransformation(std::move(t));
        scene->addObject(std::move(ground));
    }

    // Add skybox
    Shader* vsSkybox = Shader::createFromFile(GL_VERTEX_SHADER, "skybox.vert");
    Shader* fsSkybox = Shader::createFromFile(GL_FRAGMENT_SHADER, "skybox.frag");
    auto skyboxProgram = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSkybox, fsSkybox});

    std::vector<std::string> cubemapFaces = {
        "assets/textures/cubemap/posx.jpg",
        "assets/textures/cubemap/negx.jpg",
        "assets/textures/cubemap/posy.jpg",
        "assets/textures/cubemap/negy.jpg",
        "assets/textures/cubemap/posz.jpg",
        "assets/textures/cubemap/negz.jpg"
    };

    auto skybox = std::make_unique<Skybox>(cubemapFaces, skyboxProgram);
    scene->setSkybox(std::move(skybox));

    addScene(std::move(scene));
}

void Application::createSphereScene() {
    auto sphereScene = std::make_unique<Scene>();
    float sphereDistance = 2.0f;
    float sphereScale = 1.0f;

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "phong.frag");
    //Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "phong-wrong.frag");
    auto programUniversal = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsUniversal});

    // Sphere 1: Constant
    auto m1 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj1 = std::make_unique<DrawableObject>(std::move(m1), programUniversal);
    auto t1 = std::make_unique<TransformComposite>();
    t1->addTransformation(std::make_unique<Translate>(sphereDistance, 0.0f, 0.0f));
    t1->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj1->getTransform().addTransformation(std::move(t1));
    obj1->setModelType(2);
    obj1->setColor(glm::vec3(1.0f, 0.8f, 0.2f));
    sphereScene->addObject(std::move(obj1));

    // Sphere 2: Lambert
    auto m2 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj2 = std::make_unique<DrawableObject>(std::move(m2), programUniversal);
    auto t2 = std::make_unique<TransformComposite>();
    t2->addTransformation(std::make_unique<Translate>(-sphereDistance, 0.0f, 0.0f));
    t2->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj2->getTransform().addTransformation(std::move(t2));
    obj2->setModelType(2);
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
    obj4->setModelType(2);
    obj4->setColor(glm::vec3(0.9f, 0.3f, 0.3f));
    sphereScene->addObject(std::move(obj4));


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

void Application::createBackfaceTestScene() {
    auto sphereScene = std::make_unique<Scene>();
    float sphereDistance = 2.0f;
    float sphereScale = 1.0f;

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* phongWrong = Shader::createFromFile(GL_FRAGMENT_SHADER, "phong-wrong.frag");
    Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "universal.frag");

    auto programUniversal = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsUniversal});
    auto programWrongPhong = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, phongWrong});

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
    auto obj3 = std::make_unique<DrawableObject>(std::move(m3), programWrongPhong);
    auto t3 = std::make_unique<TransformComposite>();
    t3->addTransformation(std::make_unique<Translate>(0.0f, sphereDistance, 0.0f));
    t3->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj3->getTransform().addTransformation(std::move(t3));
    //obj3->setModelType(2);
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


    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programUniversal->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programUniversal));
    }

    addScene(std::move(sphereScene));
}
void Application::createSolarScene() {
    auto solarScene = std::make_unique<Scene>();

    Shader* vsSolar = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsSun = Shader::createFromFile(GL_FRAGMENT_SHADER, "emissive.frag");
    Shader* fsPlanet = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto programSun = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSolar, fsSun});
    auto programPlanet = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsSolar, fsPlanet});

    // Load planet textures
    auto sunTexture = std::make_shared<Texture>("assets/textures/planets/sun.jpg");
    auto mercuryTexture = std::make_shared<Texture>("assets/textures/planets/mercury.jpg");
    auto venusTexture = std::make_shared<Texture>("assets/textures/planets/venus.jpg");
    auto jupiterTexture = std::make_shared<Texture>("assets/textures/planets/jupiter.jpg");
    auto saturnTexture = std::make_shared<Texture>("assets/textures/planets/saturn.jpg");
    auto uranTexture = std::make_shared<Texture>("assets/textures/planets/uran.jpg");
    auto neptunTexture = std::make_shared<Texture>("assets/textures/planets/neptun.jpg");
    auto earthTexture = std::make_shared<Texture>("assets/textures/planets/earth.jpg");
    auto moonTexture = std::make_shared<Texture>("assets/textures/planets/moon.jpg");
    auto marsTexture = std::make_shared<Texture>("assets/textures/planets/mars.jpg");


    std::string planetModelPath = "assets/sphereUV.obj";
    auto sphereModelWithUV = Model::loadFromOBJ(planetModelPath);
    if (!sphereModelWithUV) {
        std::cerr << "Warning: Could not load " << planetModelPath << ", planets will not have textures" << std::endl;
    } else {
        std::cout << "Successfully loaded planet model with UV coordinates" << std::endl;
    }

    // SLUNCE
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) {
            m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        }
        auto sun = std::make_unique<DrawableObject>(std::move(m), programSun);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<DynamicRotate>(0.05f, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(2.0f, 2.0f, 2.0f));
        sun->getTransform().addTransformation(std::move(t));
        sun->setModelType(0);
        sun->setTexture(sunTexture);
        sun->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        solarScene->addObject(std::move(sun));
    }

    // MERKUR
    float mercuryOrbitRadius = 3.5f;
    float mercuryOrbitSpeed = 1.6f;
    float mercuryRotationSpeed = 0.15f;
    float mercuryScale = 0.18f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto mercury = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(mercuryOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(mercuryOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(mercuryRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(mercuryScale, mercuryScale, mercuryScale));
        mercury->getTransform().addTransformation(std::move(t));
        mercury->setTexture(mercuryTexture);
        mercury->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        mercury->setModelType(2);
        mercury->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(mercury));
    }


    // VENUS
    float venusOrbitRadius = 5.5f;
    float venusOrbitSpeed = 1.1f; 
    float venusRotationSpeed = -0.1f;
    float venusScale = 0.45f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto venus = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(venusOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(venusOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(venusRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(venusScale, venusScale, venusScale));
        venus->getTransform().addTransformation(std::move(t));
        venus->setTexture(venusTexture);
        venus->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        venus->setModelType(2);
        venus->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(venus));
    }


    // EARTH
    float earthOrbitRadius = 7.5f;
    float earthOrbitSpeed = 1.0f;
    float earthRotationSpeed = 3.0f;
    float earthScale = 0.48f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto earth = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(earthRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(earthScale, earthScale, earthScale));
        earth->getTransform().addTransformation(std::move(t));
        earth->setTexture(earthTexture);
        earth->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        earth->setModelType(2);
        earth->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(earth));
    }

    // MOON
    float moonOrbitRadius = 1.0f;
    float moonOrbitSpeed = 4.5f;
    float moonRotationSpeed = 0.3f;
    float moonScale = 0.13f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto moon = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(moonOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(moonOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(moonRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(moonScale, moonScale, moonScale));
        moon->getTransform().addTransformation(std::move(t));
        moon->setTexture(moonTexture);
        moon->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        moon->setModelType(2);
        moon->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(moon));
    }

    auto model = Model::loadFromOBJ("assets/login.obj");
    if (model) {
        float loginOrbitRadius = 1.3f;
        float loginOrbitSpeed = 4.0f;
        float loginRotationSpeed = 0.6f;
        float loginScale = 0.13f;

        auto obj = std::make_unique<DrawableObject>(std::move(model), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(loginOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(loginOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(loginRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(loginScale, loginScale, loginScale));
        obj->getTransform().addTransformation(std::move(t));
        obj->setTexture(moonTexture);
        obj->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        obj->setModelType(2);
        obj->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);

        solarScene->addObject(std::move(obj));
    }


    // MARS
    float marsOrbitRadius = 10.0f;
    float marsOrbitSpeed = 0.8f;  
    float marsRotationSpeed = 2.9f;
    float marsScale = 0.25f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto mars = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(marsOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(marsOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(marsRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(marsScale, marsScale, marsScale));
        mars->getTransform().addTransformation(std::move(t));
        mars->setTexture(marsTexture);
        mars->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        mars->setModelType(2);
        mars->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(mars));
    }

    // JUPITER
    float jupiterOrbitRadius = 14.0f;
    float jupiterOrbitSpeed = 0.52f; 
    float jupiterRotationSpeed = 5.0f; 
    float jupiterScale = 1.2f; 
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto jupiter = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(jupiterOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(jupiterOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(jupiterRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(jupiterScale, jupiterScale, jupiterScale));
        jupiter->getTransform().addTransformation(std::move(t));
        jupiter->setTexture(jupiterTexture);
        jupiter->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        jupiter->setModelType(2);
        jupiter->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(jupiter));
    }


    // SATURN
    float saturnOrbitRadius = 18.0f;
    float saturnOrbitSpeed = 0.38f;
    float saturnRotationSpeed = 4.7f;
    float saturnScale = 1.0f; 
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto saturn = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(saturnOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(saturnOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(saturnRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(saturnScale, saturnScale, saturnScale));
        saturn->getTransform().addTransformation(std::move(t));
        saturn->setTexture(saturnTexture);
        saturn->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        saturn->setModelType(2);
        saturn->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(saturn));
    }


    // URAN
    float uranOrbitRadius = 22.0f;
    float uranOrbitSpeed = 0.22f;
    float uranRotationSpeed = 3.5f;
    float uranScale = 0.55f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto uran = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(uranOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(uranOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(uranRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(uranScale, uranScale, uranScale));
        uran->getTransform().addTransformation(std::move(t));
        uran->setTexture(uranTexture);
        uran->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        uran->setModelType(2);
        uran->setMaterial(0.1f, 0.60f, 0.0f, 0.0f);
        solarScene->addObject(std::move(uran));
    }


    // NEPTUNE - most distant planet
    float neptunOrbitRadius = 26.0f;
    float neptunOrbitSpeed = 0.18f;
    float neptunRotationSpeed = 3.8f;
    float neptunScale = 0.53f;
    {
        std::unique_ptr<Model> m;
        if (sphereModelWithUV) {
            m = Model::loadFromOBJ(planetModelPath);
        }
        if (!m) m = std::make_unique<Model>(sphere, sphereDataSize, 6);
        auto neptun = std::make_unique<DrawableObject>(std::move(m), programPlanet);
        auto t = std::make_unique<TransformComposite>();
        // 1. Orbit kolem Slunce
        t->addTransformation(std::make_unique<DynamicRotate>(neptunOrbitSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Translate>(neptunOrbitRadius, 0.0f, 0.0f));
        t->addTransformation(std::make_unique<DynamicRotate>(neptunRotationSpeed, 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(neptunScale, neptunScale, neptunScale));
        neptun->getTransform().addTransformation(std::move(t));
        neptun->setTexture(neptunTexture);
        neptun->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        neptun->setModelType(2);
        neptun->setMaterial(0.1f, 0.60f, 0.0f, 10.0f);
        solarScene->addObject(std::move(neptun));
    }

    
    auto sunLight = std::make_shared<Light>(
        glm::vec3(0.0f, 0.0f, 0.0f),      
        glm::vec3(4.0f, 3.8f, 3.4f)   
    );
    sunLight->addObserver(std::static_pointer_cast<ILightObserver>(programPlanet));
    sunLight->notifyObservers();

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

void Application::createTreePlantingScene() {
    auto plantingScene = std::make_unique<Scene>();

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsAdvanced = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto programTrees = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsAdvanced});
    auto programGround = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsAdvanced});

    auto grassTexture = std::make_shared<Texture>("assets/textures/grass.png");

    auto terrainModel = Model::loadFromOBJ("assets/textures/teren/teren.obj");
    if (terrainModel) {
        auto ground = std::make_unique<DrawableObject>(std::move(terrainModel), programGround);
        ground->setTexture(grassTexture);
        ground->setColor(glm::vec3(0.3f, 0.5f, 0.2f)); // Green grass color
        ground->setMaterial(0.2f, 1.0f, 0.05f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(2.0f, 2.0f, 2.0f));
        ground->getTransform().addTransformation(std::move(t));
        ground->setModelType(2); // Phong shading
        plantingScene->addObject(std::move(ground));
    } else {
        // Fallback to simple plane if terrain model not found
        std::cerr << "Tree planting scene: failed to load terrain model, using plain" << std::endl;
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), programGround);
        ground->setTexture(grassTexture);
        ground->setColor(glm::vec3(0.3f, 0.5f, 0.2f));
        ground->setMaterial(0.2f, 1.0f, 0.05f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(30.0f, 1.0f, 30.0f));
        ground->getTransform().addTransformation(std::move(t));
        ground->setModelType(2);
        plantingScene->addObject(std::move(ground));
    }

    // Add a few initial trees as examples
    for (int i = 0; i < 5; ++i) {
        auto m = std::make_unique<Model>(tree, treeDataSize, 6);
        auto obj = std::make_unique<DrawableObject>(std::move(m), programTrees);
        obj->setColor(glm::vec3(0.15f, 0.3f, 0.1f));
        float x = (i - 2) * 3.0f;
        float z = -5.0f;
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(x, -1.0f, z));
        t->addTransformation(std::make_unique<Scale>(0.3f, 0.3f, 0.3f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        plantingScene->addObject(std::move(obj));
    }


    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programTrees->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        programGround->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programTrees));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programGround));
    }

    addScene(std::move(plantingScene));
}

void Application::updateDynamicLights() {
    // Update positions
    for (auto& light : dynamicLights) {
        light->update();
    }
    for (auto& firefly : fireflies) {
        firefly->update();
    }

    // Rebuild light uniforms each frame so moving lights update correctly
    if (!scenes.empty() && activeSceneIndex < scenes.size()) {
        auto programs = scenes[activeSceneIndex]->getShaderPrograms();
        
        // Assemble all lights into one list
        std::vector<LightData> allLights;

        // 1. Add static lights (ambient, directional, point) - only for forest scene
        if (activeSceneIndex == 0) {

            allLights.insert(allLights.end(), staticLights.begin(), staticLights.end());
        }

        if (activeSceneIndex == 1 || activeSceneIndex == 4) {

            allLights.push_back(LightData::createAmbient(glm::vec3(0.3f, 0.3f, 0.3f), 0.5f));

            LightData centerLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.95f, 0.8f), 3.0f);
            centerLight.constant = 1.0f;
            centerLight.linear = 0.02f;
            centerLight.quadratic = 0.001f;
            allLights.push_back(centerLight);
        }

        // 1a. For planetary scene (index 3) add strong lighting for planet visibility
        if (activeSceneIndex == 3) {
            allLights.push_back(LightData::createAmbient(glm::vec3(0.3f, 0.3f, 0.3f), 0.5f));
            // Strong point light from center (simulating Sun as light source)
            LightData sunLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.95f, 0.8f), 3.0f);
            sunLight.constant = 1.0f;
            sunLight.linear = 0.02f;
            sunLight.quadratic = 0.001f;
            allLights.push_back(sunLight);
        }
        
        // 1b. For tree planting scene (index 5) add bright daylight
        if (activeSceneIndex == 5) {
            allLights.push_back(LightData::createAmbient(glm::vec3(0.4f, 0.45f, 0.5f), 0.6f));
            allLights.push_back(LightData::createDirectional(
                glm::vec3(-0.3f, -1.0f, -0.2f),
                glm::vec3(1.0f, 0.95f, 0.8f),
                1.2f
            ));
        }
        
        // 1c. For formula scene (index 6) add brighter ambient
        if (activeSceneIndex == 6) {
            allLights.push_back(LightData::createAmbient(glm::vec3(1.4f, 1.4f, 1.4f), 1.6f));
            allLights.push_back(LightData::createDirectional(
                glm::vec3(-0.2f, -1.0f, -0.15f),
                glm::vec3(1.0f, 0.98f, 0.95f),
                1.2f
            ));
        }
        
        // 1d. For arcade game (index 7) bright lighting for better target visibility
        if (activeSceneIndex == 7) {
            allLights.push_back(LightData::createAmbient(glm::vec3(0.5f, 0.5f, 0.5f), 0.7f));
            allLights.push_back(LightData::createDirectional(
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                1.5f
            ));
        }

        // 2. Add flashlight ONLY if it's on
        if (flashlight.getIsOn()) {
            allLights.push_back(flashlight.getLightData());
        }

        // 3. Add fireflies only in forest scene (index 0)
        if (activeSceneIndex == 0) {
            for (auto& f : fireflies) {
                LightData fireflyLight(f->Light::getPosition(), f->Light::getColor(), 1.0f);
                fireflyLight.constant = 1.0f;
                fireflyLight.linear = 0.14f;
                fireflyLight.quadratic = 0.07f;
                allLights.push_back(fireflyLight);
            }
        }

        // 4. Add other dynamic lights
        for (auto& l : dynamicLights) {
            LightData dynamicLight(l->getPosition(), l->getColor(), 1.0f);
            allLights.push_back(dynamicLight);
        }
        
        // Set all lights to all shader programs
        for (auto& sp : programs) {
            sp->setAdvancedLights(allLights);
        }
    }
}

void Application::handleMouseClick(double xpos, double ypos) {
    if (activeSceneIndex >= scenes.size() || !camera) return;
    
    // Load ID and position in world coordinates
    GLbyte color[4];
    GLfloat depth;
    GLubyte stencilByte;  // 8-bit stencil value
    
    GLint x = static_cast<GLint>(xpos);
    GLint y = static_cast<GLint>(ypos);
    
    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    int newy = fbHeight - y;
    
    glReadPixels(x, newy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glReadPixels(x, newy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels(x, newy, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &stencilByte);
    unsigned int index = static_cast<unsigned int>(stencilByte);
    
    //printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n", x, y, color[0], color[1], color[2], color[3], depth, index);
           
    
    // Special handling for arcade game (scene 7)
    if (activeSceneIndex == 7 && index != 0) {
        int points = arcadeGame.hitTarget(index);
        if (points > 0) {
            printf("*** HIT! +%d points! Total score: %d ***\n", points, arcadeGame.getScore());
            
            DrawableObject* obj = scenes[activeSceneIndex]->getObjectByID(index);
            if (obj) {
                obj->setColor(glm::vec3(0.2f, 0.2f, 0.2f));
            }
        }
        return;
    }
    
    // Clear previous selection
    scenes[activeSceneIndex]->clearSelection();
    
    if (index != 0) {
        // Select the clicked object
        DrawableObject* obj = scenes[activeSceneIndex]->getObjectByID(index);
        if (obj) {
            obj->setSelected(true);
            
            // Calculate position in global coordinate system
           /* glm::vec3 screenX = glm::vec3(x, newy, depth);
            glm::mat4 view = camera->getViewMatrix();
            glm::mat4 projection = camera->getProjectionMatrix(fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f);
            glm::vec4 viewPort = glm::vec4(0, 0, fbWidth, fbHeight);
            glm::vec3 pos = glm::unProject(screenX, view, projection, viewPort);*/
            
            printf("Selected object with ID: %u\n", index);
        }
    } else {
        std::cout << "No object selected" << std::endl;
    }
}

void Application::deleteSelectedObject() {
    if (activeSceneIndex >= scenes.size()) return;
    
    if (scenes[activeSceneIndex]->deleteSelectedObject()) {
        std::cout << "Objekt byl smazan" << std::endl;
    } else {
        std::cout << "Zadby objekt neni vybran k odstraneni" << std::endl;
    }
}

void Application::plantTreeAtClick(double xpos, double ypos) {
    // Only works in tree planting scene (index 5)
    if (activeSceneIndex != 5 || !camera) return;

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    
    GLint x = static_cast<GLint>(xpos);
    GLint y = static_cast<GLint>(ypos);
    int newy = fbHeight - y;
    
    // Read depth at click position
    GLfloat depth;
    glReadPixels(x, newy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    
    // If depth is 1.0, we clicked on background - ignore
    if (depth >= 0.9999f) {
        printf("Clicked on background (depth=%f), ignoring\n", depth);
        return;
    }
    
    // Use unProject to get world position
    glm::vec3 screenPos = glm::vec3(x, newy, depth);
    glm::mat4 view = camera->getViewMatrix();
    float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
    glm::mat4 projection = camera->getProjectionMatrix(aspect);
    glm::vec4 viewport = glm::vec4(0, 0, fbWidth, fbHeight);
    glm::vec3 worldPos = glm::unProject(screenPos, view, projection, viewport);
    
    printf("Planting tree at world position: [%f, %f, %f]\n", worldPos.x, worldPos.y, worldPos.z);
    
    // Create new tree at the clicked position
    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsAdvanced = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto program = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsAdvanced});
    
    // Setup camera for new shader program
    program->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
    camera->addObserver(std::static_pointer_cast<ICameraObserver>(program));

    
    auto m = std::make_unique<Model>(tree, treeDataSize, 6);
    auto treeObj = std::make_unique<DrawableObject>(std::move(m), program);
    treeObj->setColor(glm::vec3(0.15f, 0.3f, 0.1f));
    treeObj->setModelType(2);
    treeObj->setID(nextObjectID++);
    
    auto t = std::make_unique<TransformComposite>();
    t->addTransformation(std::make_unique<Translate>(worldPos.x, worldPos.y, worldPos.z));
    t->addTransformation(std::make_unique<Scale>(0.3f, 0.3f, 0.3f));
    treeObj->getTransform().addTransformation(std::move(t));
    
    scenes[activeSceneIndex]->addObject(std::move(treeObj));
    
    printf("Tree planted successfully!\n");
}

void Application::addBezierControlPoint(double xpos, double ypos) {
    // Only works in formula scene (index 6)
    if (activeSceneIndex != 6 || !camera) return;

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
    
    GLint x = static_cast<GLint>(xpos);
    GLint y = static_cast<GLint>(ypos);
    int newy = fbHeight - y;
    
    // Read depth at click position
    GLfloat depth;
    glReadPixels(x, newy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    
    // Use unProject to get world position
    glm::vec3 screenPos = glm::vec3(x, newy, depth);
    glm::mat4 view = camera->getViewMatrix();
    float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
    glm::mat4 projection = camera->getProjectionMatrix(aspect);
    glm::vec4 viewport = glm::vec4(0, 0, fbWidth, fbHeight);
    glm::vec3 worldPos = glm::unProject(screenPos, view, projection, viewport);
    
    // Add control point
    bezierControlPoints.push_back(worldPos);
    printf("\n=== BEZIER CONTROL POINT ADDED ===\n");
    printf("Point %zu: [%.2f, %.2f, %.2f]\n", 
           bezierControlPoints.size(), worldPos.x, worldPos.y, worldPos.z);
    
    // If we have at least 4 points, we can create/update curve
    if (bezierControlPoints.size() >= 4) {
        int numSegments = (bezierControlPoints.size() - 1) / 3;
        printf(">>> You can now create Bezier spline with %d segment(s)\n", numSegments);
        printf(">>> To use YOUR points, you need to:\n");
        printf("    2. Press 'C' to create a NEW formula with your custom path\n");
        
        // Create new segment every 4 points
        if ((bezierControlPoints.size() - 1) % 3 == 0) {
            printf(">>> SEGMENT %d COMPLETE! Click 3 more points for next segment.\n", numSegments);
        }
    } else {
        printf(">>> Need %d more point(s) to create first segment (minimum 4 points total)\n", 
               4 - (int)bezierControlPoints.size());
    }
    printf("==================================\n\n");
}

void Application::createCustomBezierFormula() {
    // Check if we have enough points
    if (bezierControlPoints.size() < 4) {
        printf("\n*** ERROR: Need at least 4 control points! ***\n");
        printf("Current points: %zu. Add %zu more point(s) by clicking in the scene.\n\n", 
               bezierControlPoints.size(), 4 - bezierControlPoints.size());
        return;
    }
    
    // Validate number of points for proper spline (should be 3n+1)
    int numSegments = (bezierControlPoints.size() - 1) / 3;
    int expectedPoints = numSegments * 3 + 1;
    if (bezierControlPoints.size() != expectedPoints) {
        printf("\n*** WARNING: Point count not optimal for spline! ***\n");
        printf("You have %zu points. For %d complete segment(s), you need %d points.\n",
               bezierControlPoints.size(), numSegments, expectedPoints);
        printf("Extra points will be ignored.\n\n");
    }
    
    printf("\n=== CREATING CUSTOM BEZIER FORMULA ===\n");
    printf("Using %zu control points for %d segment(s)\n", bezierControlPoints.size(), numSegments);
    
    // Get the current scene (formula scene should be index 6)
    if (activeSceneIndex != 6 || scenes.size() <= 6) {
        printf("ERROR: Not in formula scene!\n");
        return;
    }
    
    printf("Creating new formula with custom path...\n");
    
    // Create new formula with custom path
    Shader* vs = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fs = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto program = std::make_shared<ShaderProgram>(std::vector<Shader*>{vs, fs});
    
    // Setup camera for new shader program
    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        program->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(program));
    }

    
    auto formulaTexture = std::make_shared<Texture>("assets/textures/shrek.png");
    auto model = Model::loadFromOBJ("assets/formula1.obj");
    if (model) {
        auto obj = std::make_unique<DrawableObject>(std::move(model), program);
        obj->setTexture(formulaTexture);
        auto t = std::make_unique<TransformComposite>();
        
        // Use user's control points!
        float duration = 5.0f * numSegments; // 5 seconds per segment
        t->addTransformation(std::make_unique<BezierSplineTransform>(
            bezierControlPoints, duration, true, true
        ));
        t->addTransformation(std::make_unique<Rotate>(glm::radians(90.0f), 0.0f, 1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(0.02f, 0.02f, 0.02f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2);
        obj->setColor(glm::vec3(0.1f, 0.9f, 0.1f)); // Green for custom path!
        obj->setMaterial(0.2f, 0.8f, 0.9f, 128.0f);
        
        // Add the new formula to the scene
        scenes[activeSceneIndex]->addObject(std::move(obj));
        
        printf(">>> NEW FORMULA CREATED with your custom path!\n");
        printf(">>> Formula is now GREEN to indicate custom path.\n");
        printf(">>> Duration: %.1f seconds (%d segments x 5s each)\n", duration, numSegments);
        printf(">>> The original red formula is still there, you now have TWO formulas!\n");
        printf(">>> Press 'X' to clear points and create another one.\n");
        printf("=====================================\n\n");
    } else {
        printf("ERROR: Failed to load formula1.obj\n");
    }
}

void Application::createArcadeScene() {
    // Reset object IDs for arcade scene to ensure they fit in 8-bit stencil buffer (0-255)
    nextObjectID = 1;
    
    auto arcadeScene = std::make_unique<Scene>();

    // Arcade shaders
    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsAdvanced = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto programTargets = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsAdvanced});

    // Background/ground shader
    Shader* vsBg = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsBg = Shader::createFromFile(GL_FRAGMENT_SHADER, "ground.frag");
    auto programBg = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsBg, fsBg});

    // Background texture
    auto groundTexture = std::make_shared<Texture>("assets/textures/grass.png");

    // Create background/ground
    {
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), programBg);
        ground->setTexture(groundTexture);
        ground->setColor(glm::vec3(0.2f, 0.3f, 0.15f));
        ground->setMaterial(0.2f, 1.0f, 0.05f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -2.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(30.0f, 1.0f, 30.0f));
        ground->getTransform().addTransformation(std::move(t));
        ground->setModelType(2);
        arcadeScene->addObject(std::move(ground));
    }

    // Create targets with random paths
    std::vector<Target::TargetType> targetTypes = {
        Target::TargetType::SPHERE,
        Target::TargetType::SPHERE,
        Target::TargetType::SPHERE,
        Target::TargetType::CUBE,
        Target::TargetType::CUBE,
        Target::TargetType::GIFT
    };

    for (size_t i = 0; i < targetTypes.size(); ++i) {
        Target::TargetType type = targetTypes[i];
        std::unique_ptr<Model> model;
        glm::vec3 color;
        float scale = 0.3f;

        // Select model based on target type
        switch (type) {
            case Target::TargetType::SPHERE:
                model = std::make_unique<Model>(sphere, sphereDataSize, 6);
                color = glm::vec3(0.2f, 0.6f, 1.0f);
                scale = 0.3f;
                break;
            case Target::TargetType::CUBE:
                if (auto m = Model::loadFromOBJ("assets/cube.obj")) {
                    model = std::move(m);
                } else {
                    model = std::make_unique<Model>(sphere, sphereDataSize, 6);
                }
                color = glm::vec3(1.0f, 0.8f, 0.2f);
                scale = 0.4f;
                break;
            case Target::TargetType::GIFT:
                model = std::make_unique<Model>(gift, giftDataSize, 6);
                color = glm::vec3(1.0f, 0.2f, 0.3f);
                scale = 1.0f;
                break;
        }

        auto target = std::make_unique<Target>(std::move(model), programTargets, type);
        target->setColor(color);
        target->setModelType(2);
        target->setMaterial(0.2f, 1.0f, 0.6f, 32.0f);

        // Generate random path (polyline with 3-5 points)
        int numPoints = 3 + (i % 3);
        auto path = arcadeGame.generateRandomPath(numPoints, -8.0f, 8.0f, -1.0f, 2.0f, -8.0f, 8.0f);
        float duration = arcadeGame.getRandomDuration(3.0f, 8.0f);

        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<LinePathTransform>(path, duration, true));
        t->addTransformation(std::make_unique<Scale>(scale, scale, scale));
        target->getTransform().addTransformation(std::move(t));

        arcadeScene->addObject(std::move(target));
    }


    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programTargets->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        programBg->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programTargets));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programBg));
    }

    addScene(std::move(arcadeScene));
    
    // Now register targets in the game (after addScene, when they have correct IDs)
    for (size_t i = 1; i < scenes.back()->getObjects().size(); ++i) {
        DrawableObject* obj = scenes.back()->getObjects()[i].get();
        Target* target = dynamic_cast<Target*>(obj);
        if (target) {
            arcadeGame.addTarget(target);
            printf("Registered target with ID %u, points %d\n", target->getID(), target->getPointValue());
        }
    }
    
    printf("Arcade scene created with targets!\n");
}

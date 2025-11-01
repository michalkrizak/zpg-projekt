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
#include "DynamicLight.h"
#include "Firefly.h"
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
        case GLFW_KEY_5: g_app->setActiveScene(4); break; // backface test scene
        case GLFW_KEY_6: g_app->setActiveScene(5); break; // FOV test scene
    case GLFW_KEY_7: g_app->setActiveScene(6); break; // Formula scene
        
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
        }
    }
}

Application::Application(int width, int height, const std::string& title)
    : window(width, height, title)
    , flashlight(glm::vec3(1.0f, 1.0f, 0.9f), 2.0f, 12.5f, 17.5f)  // Bílé světlo, vyšší intenzita
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

     /*mainLight = std::make_shared<Light>(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.08f)
    );*/
    
    // Inicializace různých typů osvětlení
    // 1. Ambientní osvětlení (tmavé, noční prostředí)
    staticLights.push_back(LightData::createAmbient(glm::vec3(0.1f, 0.1f, 0.15f), 0.05f));
    
    // 2. Směrové světlo (měsíc)
    staticLights.push_back(LightData::createDirectional(
        glm::vec3(-0.2f, -1.0f, -0.3f),  // Směr dolů a mírně do strany
        glm::vec3(0.15f, 0.15f, 0.2f),   // Modravé měsíční světlo
        0.3f                              // Nízká intenzita
    ));
    
    // 3. Bodové světlo (např. lampa nebo oheň)
    staticLights.push_back(LightData(
        glm::vec3(5.0f, 1.0f, 5.0f),     // Pozice
        glm::vec3(1.0f, 0.6f, 0.2f),     // Oranžová barva (oheň)
        1.0f                              // Intenzita
    ));
}

void Application::initialize() {
    createForestScene();
    createSphereScene();
    createTriangleScene();
    createSolarScene();
    createBackfaceTestScene();
    createFOVTestScene();
    createFormulaScene();
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

void Application::createFormulaScene() {
    auto scene = std::make_unique<Scene>();

    Shader* vs = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fs = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
    auto program = std::make_shared<ShaderProgram>(std::vector<Shader*>{vs, fs});

    // Try loading the Formula 1 OBJ
    auto model = Model::loadFromOBJ("assets/formula1.obj");
    if (model) {
        auto obj = std::make_unique<DrawableObject>(std::move(model), program);
        auto t = std::make_unique<TransformComposite>();
        // Adjust transforms to fit scene (scale down and place on ground)
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, -2.0f));
        t->addTransformation(std::make_unique<Scale>(0.02f, 0.02f, 0.02f));
        obj->getTransform().addTransformation(std::move(t));
        obj->setModelType(2); // Phong shading
        obj->setColor(glm::vec3(0.9f, 0.9f, 0.9f));
        // Material: shiny car body (low ambient, full diffuse, high specular, high shininess)
        obj->setMaterial(0.05f, 1.0f, 0.8f, 64.0f);
        scene->addObject(std::move(obj));
    } else {
        std::cerr << "Formula scene: failed to load assets/formula1.obj" << std::endl;
    }

    // Load additional assets and place them around
    struct AssetItem { const char* path; glm::vec3 translate; glm::vec3 scale; glm::vec3 color; };
    std::vector<AssetItem> extras = {
        { "assets/house.obj",   glm::vec3(-3.5f, -1.0f, -4.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.6f, 0.3f) },
        { "assets/cube.obj",    glm::vec3( 3.0f, -1.0f, -3.0f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.7f, 0.7f, 0.9f) },
        { "assets/square.obj",  glm::vec3( 0.0f, -0.5f,  2.5f), glm::vec3(2.0f, 0.1f, 2.0f), glm::vec3(0.2f, 0.2f, 0.25f) }
    };
    for (const auto& it : extras) {
        if (auto m = Model::loadFromOBJ(it.path)) {
            auto obj = std::make_unique<DrawableObject>(std::move(m), program);
            auto t = std::make_unique<TransformComposite>();
            t->addTransformation(std::make_unique<Translate>(it.translate.x, it.translate.y, it.translate.z));
            t->addTransformation(std::make_unique<Scale>(it.scale.x, it.scale.y, it.scale.z));
            obj->getTransform().addTransformation(std::move(t));
            obj->setModelType(2);
            obj->setColor(it.color);
            scene->addObject(std::move(obj));
        } else {
            std::cerr << "Formula scene: failed to load " << it.path << std::endl;
        }
    }

    // Optional: simple ground under the model for reference
    Shader* vsg = Shader::createFromFile(GL_VERTEX_SHADER, "ground.vert");
    Shader* fsg = Shader::createFromFile(GL_FRAGMENT_SHADER, "ground.frag");
    auto groundProgram = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsg, fsg});
    {
        auto m = std::make_unique<Model>(plain, plainDataSize, 6);
        auto ground = std::make_unique<DrawableObject>(std::move(m), groundProgram);
        ground->setColor(glm::vec3(0.12f, 0.12f, 0.12f));
        // Material: matte asphalt (low ambient, full diffuse, minimal specular, low shininess)
        ground->setMaterial(0.1f, 1.0f, 0.05f, 8.0f);
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<Translate>(0.0f, -1.0f, 0.0f));
        t->addTransformation(std::make_unique<Scale>(10.0f, 1.0f, 10.0f));
        ground->getTransform().addTransformation(std::move(t));
        scene->addObject(std::move(ground));
    }

    addScene(std::move(scene));
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    // Forest shaders - používáme advanced_lighting.frag pro podporu všech typů světel
    Shader* vsForest = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsForest = Shader::createFromFile(GL_FRAGMENT_SHADER, "advanced_lighting.frag");
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
        ground->setColor(glm::vec3(0.1f, 0.3f, 0.1f)); // Tmavě zelená zem
        // Material: grass/soil (low ambient, full diffuse, very low specular, low shininess)
        ground->setMaterial(0.1f, 1.0f, 0.02f, 8.0f);
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
        obj->setColor(glm::vec3(0.15f, 0.3f, 0.1f)); // Tmavě zelená barva stromů
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
        obj->setColor(glm::vec3(0.1f, 0.25f, 0.08f)); // Tmavě zelená barva keřů
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

    // Create fireflies (světlušky) - glowing spheres with dynamic lights
    Shader* vsEmissive = Shader::createFromFile(GL_VERTEX_SHADER, "emissive.vert");
    Shader* fsEmissive = Shader::createFromFile(GL_FRAGMENT_SHADER, "emissive.frag");
    auto programEmissive = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsEmissive, fsEmissive});
    
    std::uniform_real_distribution<float> distFireflyPos(-12.0f, 12.0f);
    std::uniform_real_distribution<float> distFireflySpeed(0.3f, 1.2f);
    std::uniform_real_distribution<float> distFireflyPhase(0.0f, 6.28f);
    std::uniform_real_distribution<float> distFireflyRadius(1.5f, 3.5f);
    std::uniform_real_distribution<float> distFireflyHeight(0.05f, 0.35f);
    
    // Různé barvy pro světlušky (zelená, žlutá, teplá bílá) - vysoká intenzita pro viditelné světlo
    std::vector<glm::vec3> fireflyColors = {
       // glm::vec3(0.8f, 2.0f, 0.5f),  // Jasně zelená
        glm::vec3(2.0f, 1.8f, 0.5f),  // Žlutá
       // glm::vec3(1.5f, 2.0f, 0.8f),  // Světle zelená
       // glm::vec3(2.0f, 1.5f, 0.6f),  // Teplá žlutá
        //glm::vec3(0.6f, 1.8f, 1.0f),  // Tyrkysová
       // glm::vec3(1.8f, 1.0f, 0.4f)   // Oranžová
    };
    
    // Vytvoříme 12 světlušek jako DrawableObject + DynamicLight
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

void Application::createBackfaceTestScene() {
    auto testScene = std::make_unique<Scene>();

    float sphereDistance = 2.0f;
    float sphereScale = 1.0f;

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "universal.frag");
    Shader* fsWrong = Shader::createFromFile(GL_FRAGMENT_SHADER, "phong_wrong_backside.frag");
    auto programUniversal = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsUniversal});
    auto programWrong = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsWrong});

    // Sphere 1: Phong CORRECT (right side)
    auto m1 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj1 = std::make_unique<DrawableObject>(std::move(m1), programUniversal);
    auto t1 = std::make_unique<TransformComposite>();
    t1->addTransformation(std::make_unique<Translate>(sphereDistance, 0.0f, 0.0f));
    t1->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj1->getTransform().addTransformation(std::move(t1));
    obj1->setModelType(2);
    obj1->setColor(glm::vec3(0.8f, 0.8f, 0.8f)); // Neutral gray
    testScene->addObject(std::move(obj1));

    // Sphere 2: WRONG (left side) - should be incorrectly lit on camera-facing side due to abs(dot)
    auto m2 = std::make_unique<Model>(sphere, sphereDataSize, 6);
    auto obj2 = std::make_unique<DrawableObject>(std::move(m2), programWrong);
    auto t2 = std::make_unique<TransformComposite>();
    t2->addTransformation(std::make_unique<Translate>(-sphereDistance, 0.0f, 0.0f));
    t2->addTransformation(std::make_unique<Scale>(sphereScale, sphereScale, sphereScale));
    obj2->getTransform().addTransformation(std::move(t2));
    obj2->setColor(glm::vec3(0.8f, 0.8f, 0.8f)); // Same neutral gray
    testScene->addObject(std::move(obj2));

    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programUniversal));
    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(programWrong));
    mainLight->notifyObservers();

    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        programUniversal->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        programWrong->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programUniversal));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(programWrong));
    }

    addScene(std::move(testScene));
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

void Application::createFOVTestScene() {
    auto fovScene = std::make_unique<Scene>();

    Shader* vsCommon = Shader::createFromFile(GL_VERTEX_SHADER, "common.vert");
    Shader* fsUniversal = Shader::createFromFile(GL_FRAGMENT_SHADER, "universal.frag");
    auto program = std::make_shared<ShaderProgram>(std::vector<Shader*>{vsCommon, fsUniversal});

    // Create a grid of spheres to visualize FOV effect
    float gridSize = 3.0f;
    float spacing = 2.0f;
    int rows = 5;
    int cols = 5;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            auto m = std::make_unique<Model>(sphere, sphereDataSize, 6);
            auto obj = std::make_unique<DrawableObject>(std::move(m), program);
            
            float x = (col - cols / 2.0f) * spacing;
            float y = (row - rows / 2.0f) * spacing;
            float z = -5.0f;

            auto t = std::make_unique<TransformComposite>();
            t->addTransformation(std::make_unique<Translate>(x, y, z));
            t->addTransformation(std::make_unique<Scale>(0.4f, 0.4f, 0.4f));
            obj->getTransform().addTransformation(std::move(t));
            obj->setModelType(2); // Phong
            
            // Color gradient from red to blue
            float colorFactor = static_cast<float>(row * cols + col) / (rows * cols);
            obj->setColor(glm::vec3(1.0f - colorFactor, 0.3f, colorFactor));
            
            fovScene->addObject(std::move(obj));
        }
    }

    mainLight->addObserver(std::static_pointer_cast<ILightObserver>(program));
    mainLight->notifyObservers();

    if (camera) {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? static_cast<float>(fbWidth) / fbHeight : 1.0f;
        program->setInitialViewProj(camera->getViewMatrix(), camera->getProjectionMatrix(aspect));
        camera->addObserver(std::static_pointer_cast<ICameraObserver>(program));
    }

    addScene(std::move(fovScene));
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
        
        // Sestavíme všechna světla do jednoho seznamu
        std::vector<LightData> allLights;

        // 1. Přidáme statická světla (ambient, directional, bodová)
        allLights.insert(allLights.end(), staticLights.begin(), staticLights.end());

        // 1a. Pro scénu s formulí (index 6) přidejme světlejší ambient, aby nebyla tma
        if (activeSceneIndex == 6) {
            allLights.push_back(LightData::createAmbient(glm::vec3(0.2f, 0.2f, 0.2f), 0.6f));
            // a silnější konstantní směrové světlo (studiové)
            allLights.push_back(LightData::createDirectional(
                glm::vec3(-0.2f, -1.0f, -0.15f),           // směr shora mírně dopředu
                glm::vec3(1.0f, 0.98f, 0.95f),              // téměř bílé světlo
                1.2f                                        // vyšší intenzita
            ));
        }

        // 2. Přidáme baterku POUZE pokud je zapnutá
        if (flashlight.getIsOn()) {
            allLights.push_back(flashlight.getLightData());
        }

        // 3. Přidáme světlušky jen ve scéně lesa (index 0)
        if (activeSceneIndex == 0) {
            for (auto& f : fireflies) {
                LightData fireflyLight(f->Light::getPosition(), f->Light::getColor(), 1.0f);
                fireflyLight.constant = 1.0f;
                fireflyLight.linear = 0.14f;
                fireflyLight.quadratic = 0.07f;
                allLights.push_back(fireflyLight);
            }
        }

        // 4. Přidáme další dynamická světla
        for (auto& l : dynamicLights) {
            LightData dynamicLight(l->getPosition(), l->getColor(), 1.0f);
            allLights.push_back(dynamicLight);
        }
        
        // Nastavíme všechna světla do všech shader programů (pokročilý systém)
        for (auto& sp : programs) {
            sp->setAdvancedLights(allLights);
        }
    }
}

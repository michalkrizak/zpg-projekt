#pragma once
#include "Scene.h"
#include "Window.h"
#include <vector>
#include <memory>
#include "Camera.h"
#include "Flashlight.h"
#include "LightTypes.h"
#include "ArcadeGame.h"

class Light;
class DynamicLight;
class Firefly;

class Application {
public:
    Application(int width, int height, const std::string& title);
    void initialize();
    void run();

    void addScene(std::unique_ptr<Scene> scene);
    void setActiveScene(size_t index);

    std::shared_ptr<Camera> getCamera() const { return camera; }
    Flashlight& getFlashlight() { return flashlight; }

    Window& getWindow() { return window; }
    size_t getActiveSceneIndex() const { return activeSceneIndex; }
    
    // Object picking
    void handleMouseClick(double xpos, double ypos);
    void deleteSelectedObject();
    void plantTreeAtClick(double xpos, double ypos);
    
    // Arcade game
    ArcadeGame& getArcadeGame() { return arcadeGame; }

private:
    void createForestScene();
    void createSphereScene();
    void createTriangleScene();
    void createSolarScene();
    void createBackfaceTestScene();
    void createTreePlantingScene();
    void createFormulaScene();
    void createArcadeScene();
    
    void updateDynamicLights();

    Window window;
    std::vector<std::unique_ptr<Scene>> scenes;
    size_t activeSceneIndex = 0;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Light> mainLight;
    std::vector<std::shared_ptr<DynamicLight>> dynamicLights;
    std::vector<class Firefly*> fireflies; // non-owning pointers; objects are owned by Scene
    
    // Nový systém osvětlení
    Flashlight flashlight;
    std::vector<LightData> staticLights;  // Ambientní, směrová, bodová světla
    
    // Object picking
    unsigned int nextObjectID = 1;
    
    // Arcade game
    ArcadeGame arcadeGame;
};

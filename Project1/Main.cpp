#include "Application.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Translate.h"
#include "Scale.h"
#include "DynamicRotate.h"
#include <memory>
#include <random>
#include <iostream>
#include "tree.h"
#include "bushes.h"
#include "plain.h"
#include "sphere.h"
#include "Light.h"

using namespace std;

int main() {
    try {
    Application app(2000, 1000, "ZPG");

        // Forest shaders
        Shader vsForest = Shader::fromFile(GL_VERTEX_SHADER, "common.vert");
        Shader fsForest = Shader::fromFile(GL_FRAGMENT_SHADER, "phong.frag");
        auto programForest = make_shared<ShaderProgram>(vsForest, fsForest);

        // Ground shaders
        Shader vsg = Shader::fromFile(GL_VERTEX_SHADER, "ground.vert");
        Shader fsg = Shader::fromFile(GL_FRAGMENT_SHADER, "ground.frag");
        auto programGround = make_shared<ShaderProgram>(vsg, fsg);

        auto forest = std::make_unique<Scene>();

        std::mt19937 rng(1337);
        std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
        std::uniform_real_distribution<float> distScale(0.15f, 0.35f);

        // Ground plane
        {
            auto m = make_unique<Model>(plain, plainDataSize, 6);
            auto ground = make_unique<DrawableObject>(move(m), programGround);
            auto t = make_unique<TransformComposite>();
            t->addTransformation(make_unique<Translate>(0.0f, -1.0f, 0.0f));
            t->addTransformation(make_unique<Scale>(30.0f, 1.0f, 30.0f));
            ground->getTransform().addTransformation(move(t));
            forest->addObject(move(ground));
        }

        // 50 trees
        for (int i = 0; i < 150; ++i) {
            auto m = make_unique<Model>(tree, treeDataSize, 6);
            auto obj = make_unique<DrawableObject>(move(m), programForest);
            float x = distPos(rng);
            float z = distPos(rng);
            float s = distScale(rng);
            auto t = make_unique<TransformComposite>();
            t->addTransformation(make_unique<Translate>(x, -1.0f, z));
            t->addTransformation(make_unique<Scale>(s, s, s));
            obj->getTransform().addTransformation(move(t));
            forest->addObject(move(obj));
        }

        // 50 bushes
        for (int i = 0; i < 150; ++i) {
            auto m = make_unique<Model>(bushes, bushesDataSize, 6);
            auto obj = make_unique<DrawableObject>(move(m), programForest);
            float x = distPos(rng);
            float z = distPos(rng);
            float s = distScale(rng) * 3.0f; // menší než stromy
            auto t = make_unique<TransformComposite>();
            t->addTransformation(make_unique<Translate>(x, -1.0f, z));
            t->addTransformation(make_unique<Scale>(s, s, s));
            obj->getTransform().addTransformation(move(t));
            forest->addObject(move(obj));
        }

        auto light = std::make_shared<Light>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.9f));
        light->addObserver(programForest);
        light->addObserver(programGround);
        light->notifyObservers();

        app.addScene(std::move(forest));


        //4 Spheres
        auto sphereScene = std::make_unique<Scene>();
        float sphereDistance = 2.0f;
        float sphereScale = 1.0f;

        Shader vsCommon = Shader::fromFile(GL_VERTEX_SHADER, "common.vert");
        Shader fsUniversal = Shader::fromFile(GL_FRAGMENT_SHADER, "universal.frag");
        auto programUniversal = make_shared<ShaderProgram>(vsCommon, fsUniversal);

        // Sphere 1: Constant
        auto m1 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto obj1 = make_unique<DrawableObject>(move(m1), programUniversal);
        auto t1 = make_unique<TransformComposite>();
        t1->addTransformation(make_unique<Translate>(sphereDistance, 0.0f, 0.0f));
        t1->addTransformation(make_unique<Scale>(sphereScale, sphereScale, sphereScale));
        obj1->getTransform().addTransformation(move(t1));
        obj1->setModelType(0); // Constant
        obj1->setColor(glm::vec3(1.0f, 0.8f, 0.2f));
        sphereScene->addObject(move(obj1));

        // Sphere 2: Lambert
        auto m2 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto obj2 = make_unique<DrawableObject>(move(m2), programUniversal);
        auto t2 = make_unique<TransformComposite>();
        t2->addTransformation(make_unique<Translate>(-sphereDistance, 0.0f, 0.0f));
        t2->addTransformation(make_unique<Scale>(sphereScale, sphereScale, sphereScale));
        obj2->getTransform().addTransformation(move(t2));
        obj2->setModelType(1); // Lambert
        obj2->setColor(glm::vec3(0.2f, 0.8f, 0.2f));
        sphereScene->addObject(move(obj2));

        // Sphere 3: Phong
        auto m3 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto obj3 = make_unique<DrawableObject>(move(m3), programUniversal);
        auto t3 = make_unique<TransformComposite>();
        t3->addTransformation(make_unique<Translate>(0.0f, sphereDistance, 0.0f));
        t3->addTransformation(make_unique<Scale>(sphereScale, sphereScale, sphereScale));
        obj3->getTransform().addTransformation(move(t3));
        obj3->setModelType(2); // Phong
        obj3->setColor(glm::vec3(0.2f, 0.6f, 1.0f));
        sphereScene->addObject(move(obj3));

        // Sphere 4: Blinn-Phong
        auto m4 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto obj4 = make_unique<DrawableObject>(move(m4), programUniversal);
        auto t4 = make_unique<TransformComposite>();
        t4->addTransformation(make_unique<Translate>(0.0f, -sphereDistance, 0.0f));
        t4->addTransformation(make_unique<Scale>(sphereScale, sphereScale, sphereScale));
        obj4->getTransform().addTransformation(move(t4));
        obj4->setModelType(3); // Blinn-Phong
        obj4->setColor(glm::vec3(0.9f, 0.3f, 0.3f));
        sphereScene->addObject(move(obj4));

        light->addObserver(programUniversal);
        light->notifyObservers();

        if (app.getCamera()) {
            int fbWidth=0, fbHeight=0; glfwGetFramebufferSize(app.getWindow().getGLFWwindow(), &fbWidth, &fbHeight);
            float aspect = fbHeight>0? static_cast<float>(fbWidth)/fbHeight : 1.0f;
            programUniversal->setInitialViewProj(app.getCamera()->getViewMatrix(), app.getCamera()->getProjectionMatrix(aspect));
            app.getCamera()->addObserver(programUniversal);
        }

        app.addScene(std::move(sphereScene));

        // Triangle (testing)
        auto triangleScene = std::make_unique<Scene>();
        static const float triVerts[] = {
            // x, y, z
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };
        auto triModel = make_unique<Model>(triVerts, sizeof(triVerts), 3);
        Shader vsTri = Shader::fromFile(GL_VERTEX_SHADER, "tri.vert");
        Shader fsTri = Shader::fromFile(GL_FRAGMENT_SHADER, "tri.frag");
        auto programTri = make_shared<ShaderProgram>(vsTri, fsTri);
        auto triObj = make_unique<DrawableObject>(move(triModel), programTri);
        triangleScene->addObject(move(triObj));

        // Connect camera to triangle program
        if (app.getCamera()) {
            int fbWidth=0, fbHeight=0; glfwGetFramebufferSize(app.getWindow().getGLFWwindow(), &fbWidth, &fbHeight);
            float aspect = fbHeight>0? static_cast<float>(fbWidth)/fbHeight : 1.0f;
            programTri->setInitialViewProj(app.getCamera()->getViewMatrix(), app.getCamera()->getProjectionMatrix(aspect));
            app.getCamera()->addObserver(programTri);
        }

        app.addScene(std::move(triangleScene));

        //  Solar System (Sun, Earth, Moon)
        auto solarScene = std::make_unique<Scene>();

        Shader vsSolar = Shader::fromFile(GL_VERTEX_SHADER, "common.vert");
        Shader fsSun = Shader::fromFile(GL_FRAGMENT_SHADER, "constant.frag");
        Shader fsPlanet = Shader::fromFile(GL_FRAGMENT_SHADER, "color_phong.frag");
        auto programSun = make_shared<ShaderProgram>(vsSolar, fsSun);
        auto programPlanet = make_shared<ShaderProgram>(vsSolar, fsPlanet);

        // SUN
        {
            auto m = make_unique<Model>(sphere, sphereDataSize, 6);
            auto sun = make_unique<DrawableObject>(move(m), programSun);
            auto t = make_unique<TransformComposite>();
            t->addTransformation(make_unique<Scale>(1.2f, 1.2f, 1.2f));
            sun->getTransform().addTransformation(std::move(t));
            sun->setModelType(0);
            sun->setColor(glm::vec3(1.0f, 0.9f, 0.2f));
            solarScene->addObject(std::move(sun));
        }

        // EARTH orbiting around SUN
        float earthOrbitRadius = 5.0f;
        float earthOrbitSpeed = 0.6f;  // radians/sec
        float earthScale = 0.5f;
        {
            auto m = make_unique<Model>(sphere, sphereDataSize, 6);
            auto earth = make_unique<DrawableObject>(move(m), programPlanet);
            auto t = make_unique<TransformComposite>();
            t->addTransformation(make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
            t->addTransformation(make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
            t->addTransformation(make_unique<Scale>(earthScale, earthScale, earthScale));
            earth->getTransform().addTransformation(std::move(t));
            earth->setColor(glm::vec3(0.2f, 0.6f, 1.0f));
            solarScene->addObject(std::move(earth));
        }

        // MOON orbiting around EARTH
        float moonOrbitRadius = 1.5f;
        float moonOrbitSpeed = 1.8f;   // faster than Earth
        float moonScale = 0.18f;
        {
            auto m = make_unique<Model>(sphere, sphereDataSize, 6);
            auto moon = make_unique<DrawableObject>(move(m), programPlanet);
            auto t = make_unique<TransformComposite>();
            // Earth's orbit transform
            t->addTransformation(make_unique<DynamicRotate>(earthOrbitSpeed, 0.0f, 1.0f, 0.0f));
            t->addTransformation(make_unique<Translate>(earthOrbitRadius, 0.0f, 0.0f));
            // Then Moon's own orbiting
            t->addTransformation(make_unique<DynamicRotate>(moonOrbitSpeed, 0.0f, 1.0f, 0.0f));
            t->addTransformation(make_unique<Translate>(moonOrbitRadius, 0.0f, 0.0f));
            t->addTransformation(make_unique<Scale>(moonScale, moonScale, moonScale));
            moon->getTransform().addTransformation(std::move(t));
            moon->setColor(glm::vec3(0.7f, 0.7f, 0.7f));
            solarScene->addObject(std::move(moon));
        }

        // Register planet program with light and camera
        light->addObserver(programPlanet);
        light->notifyObservers();
        if (app.getCamera()) {
            int fbWidth=0, fbHeight=0; glfwGetFramebufferSize(app.getWindow().getGLFWwindow(), &fbWidth, &fbHeight);
            float aspect = fbHeight>0? static_cast<float>(fbWidth)/fbHeight : 1.0f;
            for (auto& sp : {programSun, programPlanet}) {
                sp->setInitialViewProj(app.getCamera()->getViewMatrix(), app.getCamera()->getProjectionMatrix(aspect));
                app.getCamera()->addObserver(sp);
            }
        }

        app.addScene(std::move(solarScene));

        app.run();
    }
    catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

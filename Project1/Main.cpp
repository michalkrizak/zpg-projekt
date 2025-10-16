#include "Application.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Translate.h"
#include "Scale.h"
#include <memory>
#include <random>
#include <iostream>
#include "tree.h"
#include "bushes.h"
#include "plain.h"
#include "Light.h"

using namespace std;

int main() {
    try {
    Application app(2000, 1000, "ZPG");

        // Forest shaders with camera support and basic point light
        Shader vsForest = Shader::fromFile(GL_VERTEX_SHADER, "forest.vert");
        Shader fsForest = Shader::fromFile(GL_FRAGMENT_SHADER, "forest.frag");
        auto programForest = make_shared<ShaderProgram>(vsForest, fsForest);

        // Ground shaders: compute world position and shade procedurally
        Shader vsg = Shader::fromFile(GL_VERTEX_SHADER, "ground.vert");
        Shader fsg = Shader::fromFile(GL_FRAGMENT_SHADER, "ground.frag");
        auto programGround = make_shared<ShaderProgram>(vsg, fsg);

    // Build a single scene with 50 trees and 50 bushes
        auto forest = std::make_unique<Scene>();

        std::mt19937 rng(1337);
        std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
        std::uniform_real_distribution<float> distScale(0.15f, 0.35f);

        // Ground plane
        {
            auto m = make_unique<Model>(plain, plainDataSize, 6);
            auto ground = make_unique<DrawableObject>(move(m), programGround);
            auto t = make_unique<TransformAction>();
            // Place ground at y ~ -1 and scale large to cover the area
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
            auto t = make_unique<TransformAction>();
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
            auto t = make_unique<TransformAction>();
            t->addTransformation(make_unique<Translate>(x, -1.0f, z));
            t->addTransformation(make_unique<Scale>(s, s, s));
            obj->getTransform().addTransformation(move(t));
            forest->addObject(move(obj));
        }

        // Create a point light and register ShaderPrograms as observers
        auto light = std::make_shared<Light>(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.9f));
        light->addObserver(programForest);
        light->addObserver(programGround);
        light->notifyObservers(); // Initial notification

        // Register the scene and run
        app.addScene(std::move(forest));
        app.run();
    }
    catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

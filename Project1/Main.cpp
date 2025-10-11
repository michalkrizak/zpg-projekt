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

using namespace std;

int main() {
    try {
    Application app(2000, 1000, "ZPG");

        // Forest shaders with camera support (view/projection)
        Shader vsForest(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec3 color;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main(){\n"
            "  fragColor = color;\n"
            "  gl_Position = projection * view * model * vec4(position,1.0);\n"
            "}");

        Shader fsForest(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 fragColor;\n"
            "out vec4 outColor;\n"
            "void main(){ outColor = vec4(fragColor, 1.0); }");

        auto programForest = make_shared<ShaderProgram>(vsForest, fsForest);

        // Ground shaders: compute world position and shade procedurally
        Shader vsg(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec3 color;\n"
            "out vec3 vWorldPos;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main(){\n"
            "  vec4 wp = model * vec4(position,1.0);\n"
            "  vWorldPos = wp.xyz;\n"
            "  gl_Position = projection * view * wp;\n"
            "}");

        Shader fsg(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 vWorldPos;\n"
            "out vec4 outColor;\n"
            "float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1,311.7)))*43758.5453); }\n"
            "void main(){\n"
            "  // Checker-like tiling with subtle noise\n"
            "  float tileSize = 2.0;\n"
            "  vec2 t = floor(vWorldPos.xz / tileSize);\n"
            "  float checker = mod(t.x + t.y, 2.0);\n"
            "  vec3 colA = vec3(0.18, 0.35, 0.12);\n"
            "  vec3 colB = vec3(0.15, 0.32, 0.10);\n"
            "  float n = hash(t) * 0.08;\n"
            "  vec3 c = mix(colA, colB, checker) + n;\n"
            "  outColor = vec4(c,1.0);\n"
            "}");

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

        // Register the scene and run
        app.addScene(std::move(forest));
        app.run();
    }
    catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

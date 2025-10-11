#include "Application.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableObject.h"
#include "Translate.h"
#include "DynamicTranslate.h"
#include "DynamicRotate.h"
#include "DynamicScale.h"
#include "Scale.h"
#include "Rotate.h"
#include <memory>
#include "sphere.h"
#include "bushes.h"
#include "gift.h"
#include "plain.h"
#include "suzi_flat.h"
#include "suzi_smooth.h"
#include "tree.h"
#include "house.h"
#include <iostream>
#include "Camera.h"

using namespace std;

// Globální promìnné pro kameru
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Funkce pro zpracování vstupu
void processInput(GLFWwindow* window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard('W', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard('S', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard('A', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard('D', deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = 400, lastY = 300;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Obrácenì, protože y-coordinates jdou odspodu nahoru

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xOffset, yOffset);
}

int main() {
    try {
        Application app(2000, 1000, "ZPG");

        float triangle[] = {
             0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        float groundTriangle[] = {
             1.0f, -0.9f, 0.0f, 1.0f, 0.0f, 0.0f,
             -1.0f, -0.9f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        float triangleCenteres[] = {
            -0.166f,  0.166f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.333f,  0.166f, 0.0f,  0.0f, 1.0f, 0.0f,
            -0.166f, -0.333f, 0.0f,  0.0f, 0.0f, 1.0f
        };

        Shader vs1(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "layout(location=1) in vec3 color;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor=color;gl_Position=model*vec4(vp,1.0);}");

        Shader vsGreen(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor = vec3(0.0, 1.0, 0.0);gl_Position = model * vec4(vp,1.0);}");

        Shader vsBlue(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor = vec3(0.0, 0.0, 1.0);gl_Position = model * vec4(vp,1.0);}");

        Shader vsYellow(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor = vec3(1.0, 1.0, 0.0);gl_Position = model * vec4(vp,1.0);}");

        Shader vsGift(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor = vec3(1.0, 0.0, 0.0);gl_Position = model * vec4(vp,1.0);}");

        Shader vs(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor = vec3(1.0, 0.5, 0.0);gl_Position = model * vec4(vp,1.0);}");

        Shader fs(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 fragColor;\n"
            "out vec4 outColor;\n"
            "void main(){outColor=vec4(fragColor,1.0);}");

        Shader vsTree(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;  // pozice vrcholu\n"
            "layout(location=1) in vec3 vertexColor;  // barva vrcholu (kmen nebo koruna)\n"
            "out vec3 fragColor;  // barva pro fragment shader\n"
            "uniform mat4 model;\n"
            "void main() {\n"
            "   fragColor = vertexColor;  // posíláme barvu vrcholu do fragment shaderu\n"
            "   gl_Position = model * vec4(vp, 1.0);\n"
            "}");

        Shader fsTree(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 fragColor;  // barva z vertex shaderu\n"
            "out vec4 outColor;  // výsledná barva pro pixel\n"
            "void main() {\n"
            "   outColor = vec4(fragColor, 1.0);  // aplikujeme barvu na pixel\n"
            "}");

        Shader vsHouse(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in vec3 color;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main() {\n"
            "    fragColor = color;\n"
            "    gl_Position = projection * view * model * vec4(position, 1.0f);\n"
            "}");


        Shader fsHouse(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 fragColor;\n"
            "out vec4 FragColor;\n"
            "void main() {\n"
            "    FragColor = vec4(fragColor, 1.0f);\n"
            "}");

        auto program = make_shared<ShaderProgram>(vs, fs);
        auto program1 = make_shared<ShaderProgram>(vs1, fs);
        auto programGreen = make_shared<ShaderProgram>(vsGreen, fs);
        auto programTree = make_shared<ShaderProgram>(vsTree, fsTree);
        auto programYellow = make_shared<ShaderProgram>(vsYellow, fs);
        auto programGift = make_shared<ShaderProgram>(vsGift, fs);
        auto programHouse = make_shared<ShaderProgram>(vsHouse, fsHouse);
        auto programBlue = make_shared<ShaderProgram>(vsBlue, fs);



        // Triangle 1 – posunutý doleva
        auto model1 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj1 = make_unique<DrawableObject>(move(model1), program);
        obj1->getTransform().addTransformation(make_unique<Translate>(-0.5f, 0.0f, 0.0f));

        // Triangle 2 – rotující
        auto model2 = make_unique<Model>(triangleCenteres, sizeof(triangleCenteres), 6);
        auto obj2 = make_unique<DrawableObject>(move(model2), program);
        obj2->getTransform().addTransformation(make_unique<DynamicRotate>(1.0f, 0.0f, 0.0f, 1.0f));


        // Triangle 3 – pulsující scale
        auto model3 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj3 = make_unique<DrawableObject>(move(model3), program);
        obj3->getTransform().addTransformation(make_unique<DynamicScale>(0.5f, 0.5f, 0.0f, 2.0f));

        // Triangle 4 – houpající se translate
        auto model4 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj4 = make_unique<DrawableObject>(move(model4), program);
        obj4->getTransform().addTransformation(make_unique<DynamicTranslate>(0.5f, 0.0f, 0.0f, 2.0f));

        // Triangle 5 – statická rotace
        auto model5 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj5 = make_unique<DrawableObject>(move(model5), program);
        obj5->getTransform().addTransformation(make_unique<Rotate>(glm::radians(45.0f), 0.0f, 0.0f, 1.0f));

        // Triangle 6 – statický scale
        auto model6 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj6 = make_unique<DrawableObject>(move(model6), program);
        obj6->getTransform().addTransformation(make_unique<Scale>(0.5f, 0.5f, 1.0f));



        // Triangle 7 – composite transformace
        auto model7 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj7 = make_unique<DrawableObject>(move(model7), program);

        auto composite = make_unique<TransformAction>();
        composite->addTransformation(make_unique<Translate>(0.5f, 0.0f, 0.0f));
        composite->addTransformation(make_unique<Rotate>(glm::radians(45.0f), 0.0f, 0.0f, 1.0f));
        composite->addTransformation(make_unique<Scale>(0.5f, 0.5f, 0.5f));

        obj7->getTransform().addTransformation(move(composite));


        // koule na +X
        /*auto sphere1 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sph1 = make_unique<DrawableObject>(move(sphere1), program);
        sph1->getTransform().addTransformation(make_unique<Translate>(0.0f, 0.0f, 0.0f));*/

        // koule na +X
        auto sphere1 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sph1 = make_unique<DrawableObject>(move(sphere1), program);
        auto compositeSphere1 = make_unique<TransformAction>();
        compositeSphere1->addTransformation(make_unique<Translate>(0.5f, 0.0f, 0.0f));
        compositeSphere1->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        sph1->getTransform().addTransformation(move(compositeSphere1));

        // koule na -X
        auto sphere2 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sph2 = make_unique<DrawableObject>(move(sphere2), program);
        auto compositeSphere2 = make_unique<TransformAction>();
        compositeSphere2->addTransformation(make_unique<Translate>(-0.5f, 0.0f, 0.0f));
        compositeSphere2->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        sph2->getTransform().addTransformation(move(compositeSphere2));

        // koule na +Y
        auto sphere3 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sph3 = make_unique<DrawableObject>(move(sphere3), program);
        auto compositeSphere3 = make_unique<TransformAction>();
        compositeSphere3->addTransformation(make_unique<Translate>(0.0f, 0.5f, 0.0f));
        compositeSphere3->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        sph3->getTransform().addTransformation(move(compositeSphere3));

        // koule na -Y
        auto sphere4 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sph4 = make_unique<DrawableObject>(move(sphere4), program);
        auto compositeSphere4 = make_unique<TransformAction>();
        compositeSphere4->addTransformation(make_unique<Translate>(0.0f, -0.5f, 0.0f));
        compositeSphere4->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        sph4->getTransform().addTransformation(move(compositeSphere4));

        auto giftModel = make_unique<Model>(gift, giftDataSize, 6);
        auto gft = make_unique<DrawableObject>(move(giftModel), programGift);
        auto compositeGift = make_unique<TransformAction>();
        compositeGift->addTransformation(make_unique<Translate>(0.3f, -0.9f, 0.0f));
        compositeGift->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        gft->getTransform().addTransformation(move(compositeGift));

        auto plainModel = make_unique<Model>(plain, plainDataSize, 6);
        auto plain = make_unique<DrawableObject>(move(plainModel), programTree);
        auto compositePlain = make_unique<TransformAction>();
        compositePlain->addTransformation(make_unique<Translate>(0.6f, -0.6f, 0.0f));
        compositePlain->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        plain->getTransform().addTransformation(move(compositePlain));

        auto sphereModel = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sphr = make_unique<DrawableObject>(move(sphereModel), programYellow);
        auto compositeSphr = make_unique<TransformAction>();
        compositeSphr->addTransformation(make_unique<Translate>(0.6f, 0.6f, 0.0f));
        compositeSphr->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        sphr->getTransform().addTransformation(move(compositeSphr));

        auto suziFlatModel = make_unique<Model>(suziFlat, suziFlatDataSize, 6);
        auto suziFlat = make_unique<DrawableObject>(move(suziFlatModel), programTree);
        auto compositeSuziFlat = make_unique<TransformAction>();
        compositeSuziFlat->addTransformation(make_unique<Translate>(-0.7f, -0.69f, 0.0f));
        compositeSuziFlat->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        suziFlat->getTransform().addTransformation(move(compositeSuziFlat));

        auto suziSmoothModel = make_unique<Model>(suziSmooth, suziSmoothDataSize, 6);
        auto suziSmooth = make_unique<DrawableObject>(move(suziSmoothModel), programTree);
        auto compositeSuziSmooth = make_unique<TransformAction>();
        compositeSuziSmooth->addTransformation(make_unique<Translate>(-0.7f, 0.5f, 0.0f));
        compositeSuziSmooth->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        suziSmooth->getTransform().addTransformation(move(compositeSuziSmooth));

        auto treeModel = make_unique<Model>(tree, treeDataSize, 6);
        auto tree1 = make_unique<DrawableObject>(move(treeModel), programTree);
        auto compositeTree = make_unique<TransformAction>();
        compositeTree->addTransformation(make_unique<Translate>(0.0f, -0.9f, 0.0f));
        compositeTree->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        tree1->getTransform().addTransformation(move(compositeTree));

        auto bush1 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh1 = make_unique<DrawableObject>(move(bush1), programGreen);
        auto compositeBush1 = make_unique<TransformAction>();
        compositeBush1->addTransformation(make_unique<Translate>(0.1f, -0.9f, 0.0f));
        compositeBush1->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh1->getTransform().addTransformation(move(compositeBush1));

        auto bush2 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh2 = make_unique<DrawableObject>(move(bush2), programGreen);
        auto compositeBush2 = make_unique<TransformAction>();
        compositeBush2->addTransformation(make_unique<Translate>(0.2f, -0.9f, 0.0f));
        compositeBush2->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh2->getTransform().addTransformation(move(compositeBush2));

        auto bush3 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh3 = make_unique<DrawableObject>(move(bush3), programGreen);
        auto compositeBush3 = make_unique<TransformAction>();
        compositeBush3->addTransformation(make_unique<Translate>(-0.1f, -0.9f, 0.0f));
        compositeBush3->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh3->getTransform().addTransformation(move(compositeBush3));

        auto bush4 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh4 = make_unique<DrawableObject>(move(bush4), programGreen);
        auto compositeBush4 = make_unique<TransformAction>();
        compositeBush4->addTransformation(make_unique<Translate>(-0.2f, -0.9f, 0.0f));
        compositeBush4->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh4->getTransform().addTransformation(move(compositeBush4));

        auto treeModel2 = make_unique<Model>(tree, treeDataSize, 6);
        auto tree2 = make_unique<DrawableObject>(move(treeModel2), programTree);
        auto compositeTree2 = make_unique<TransformAction>();
        compositeTree2->addTransformation(make_unique<Translate>(-0.3f, -0.9f, 0.0f));
        compositeTree2->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        tree2->getTransform().addTransformation(move(compositeTree2));

        auto bush5 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh5 = make_unique<DrawableObject>(move(bush5), programGreen);
        auto compositeBush5 = make_unique<TransformAction>();
        compositeBush5->addTransformation(make_unique<Translate>(-0.4f, -0.9f, 0.0f));
        compositeBush5->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh5->getTransform().addTransformation(move(compositeBush5));

        auto bush6 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh6 = make_unique<DrawableObject>(move(bush6), programGreen);
        auto compositeBush6 = make_unique<TransformAction>();
        compositeBush6->addTransformation(make_unique<Translate>(-0.5f, -0.9f, 0.0f));
        compositeBush6->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh6->getTransform().addTransformation(move(compositeBush6));

        auto bush7 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh7 = make_unique<DrawableObject>(move(bush7), programGreen);
        auto compositeBush7 = make_unique<TransformAction>();
        compositeBush7->addTransformation(make_unique<Translate>(0.9f, -0.9f, 0.0f));
        compositeBush7->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh7->getTransform().addTransformation(move(compositeBush7));

        auto bush8 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh8 = make_unique<DrawableObject>(move(bush8), programGreen);
        auto compositeBush8 = make_unique<TransformAction>();
        compositeBush8->addTransformation(make_unique<Translate>(0.95f, -0.9f, 0.0f));
        compositeBush8->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh8->getTransform().addTransformation(move(compositeBush8));

        auto bush9 = make_unique<Model>(bushes, bushesDataSize, 6);
        auto bsh9 = make_unique<DrawableObject>(move(bush9), programGreen);
        auto compositeBush9 = make_unique<TransformAction>();
        compositeBush9->addTransformation(make_unique<Translate>(1.0f, -0.9f, 0.0f));
        compositeBush9->addTransformation(make_unique<Scale>(0.2f, 0.2f, 0.2f));
        bsh9->getTransform().addTransformation(move(compositeBush9));

        auto home = make_unique<Model>(house, houseDataSize, 6);
        auto home1 = make_unique<DrawableObject>(move(home), programTree);
        auto compositeHome = make_unique<TransformAction>();
        compositeHome->addTransformation(make_unique<Translate>(0.6f, -0.55f, 0.0f));
        compositeHome->addTransformation(make_unique<Scale>(0.5f, 0.7f, 0.8f));
        home1->getTransform().addTransformation(move(compositeHome));

        auto sphereModel1 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sphr1 = make_unique<DrawableObject>(move(sphereModel1), programBlue);
        auto compositeSphr1 = make_unique<TransformAction>();
        compositeSphr1->addTransformation(make_unique<Translate>(-0.8f, 0.8f, 0.0f));
        compositeSphr1->addTransformation(make_unique<Scale>(0.2f, 0.15f, 0.2f));
        sphr1->getTransform().addTransformation(move(compositeSphr1));

        auto sphereModel2 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sphr2 = make_unique<DrawableObject>(move(sphereModel2), programBlue);
        auto compositeSphr2 = make_unique<TransformAction>();
        compositeSphr2->addTransformation(make_unique<Translate>(-0.6f, 0.8f, 0.0f));
        compositeSphr2->addTransformation(make_unique<Scale>(0.2f, 0.15f, 0.2f));
        sphr2->getTransform().addTransformation(move(compositeSphr2));

        auto sphereModel3 = make_unique<Model>(sphere, sphereDataSize, 6);
        auto sphr3 = make_unique<DrawableObject>(move(sphereModel3), programBlue);
        auto compositeSphr3 = make_unique<TransformAction>();
        compositeSphr3->addTransformation(make_unique<Translate>(-0.4f, 0.8f, 0.0f));
        compositeSphr3->addTransformation(make_unique<Scale>(0.2f, 0.15f, 0.2f));
        sphr3->getTransform().addTransformation(move(compositeSphr3));

        auto ground = make_unique<Model>(groundTriangle, sizeof(groundTriangle), 6);
        auto grnd = make_unique<DrawableObject>(move(ground), programGreen);
        auto compositeGrnd = make_unique<TransformAction>();
        //compositeGrnd->addTransformation(make_unique<Translate>(-0.7f, -0.3f, 0.0f));
        compositeGrnd->addTransformation(make_unique<Scale>(1.0f, 1.0f, 0.2f));
        grnd->getTransform().addTransformation(move(compositeGrnd));


        auto scene1 = std::make_unique<Scene>();
        scene1->addObject(move(obj2));

        auto scene2 = std::make_unique<Scene>();
        scene2->addObject(move(obj1));
        scene2->addObject(move(obj3));
        scene2->addObject(move(obj4));
        scene2->addObject(move(obj5));
        scene2->addObject(move(obj6));
        scene2->addObject(move(obj7));

        auto sceneSphere = std::make_unique<Scene>();
        sceneSphere->addObject(move(sph1));
        sceneSphere->addObject(move(sph2));
        sceneSphere->addObject(move(sph3));
        sceneSphere->addObject(move(sph4));

        auto bigScene = std::make_unique<Scene>();
        bigScene->addObject(move(bsh1));
        bigScene->addObject(move(bsh2));
        bigScene->addObject(move(bsh3));
        bigScene->addObject(move(bsh4));
        bigScene->addObject(move(gft));
        bigScene->addObject(move(plain));
        bigScene->addObject(move(sphr));
        bigScene->addObject(move(suziFlat));
        bigScene->addObject(move(suziSmooth));
        bigScene->addObject(move(tree1));
        bigScene->addObject(move(tree2));
        bigScene->addObject(move(bsh5));
        bigScene->addObject(move(bsh6));
        bigScene->addObject(move(home1));
        bigScene->addObject(move(bsh7));
        bigScene->addObject(move(bsh8));
        bigScene->addObject(move(sphr1));
        bigScene->addObject(move(sphr2));
        bigScene->addObject(move(sphr3));
        bigScene->addObject(move(grnd));

        app.addScene(std::move(scene1));
        app.addScene(std::move(scene2));
        app.addScene(std::move(sceneSphere));
        app.addScene(std::move(bigScene));


        app.run();
    }
    catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

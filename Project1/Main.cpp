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

using namespace std;

int main() {
    try {
        Application app(2000, 1000, "ZPG");

        float triangle[] = {
             0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        Shader vs(GL_VERTEX_SHADER,
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;\n"
            "layout(location=1) in vec3 color;\n"
            "out vec3 fragColor;\n"
            "uniform mat4 model;\n"
            "void main(){fragColor=color;gl_Position=model*vec4(vp,1.0);}");

        Shader fs(GL_FRAGMENT_SHADER,
            "#version 330 core\n"
            "in vec3 fragColor;\n"
            "out vec4 outColor;\n"
            "void main(){outColor=vec4(fragColor,1.0);}");


        auto program = make_shared<ShaderProgram>(vs, fs);

        // Triangle 1 – posunutý doleva
        auto model1 = make_unique<Model>(triangle, sizeof(triangle), 6);
        auto obj1 = make_unique<DrawableObject>(move(model1), program);
        obj1->getTransform().addTransformation(make_unique<Translate>(-0.5f, 0.0f, 0.0f));

        // Triangle 2 – rotující
        auto model2 = make_unique<Model>(triangle, sizeof(triangle), 6);
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

        // Pøidání do scény
       /* app.getScene().addObject(move(obj1));
        app.getScene().addObject(move(obj2));*/
        // app.getScene().addObject(move(obj3));
       //app.getScene().addObject(move(obj4));
       app.getScene().addObject(move(obj5));
       //app.getScene().addObject(move(obj6));

        app.run();
    }
    catch (const exception& e) {
        //cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

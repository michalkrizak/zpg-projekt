#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include <iostream>

int main() {
    try {
        Window window(2000, 1000, "ZPG");

        float triangle[] = {
            // x,    y,    z,    r,   g,   b
             0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // red
             1.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // green
             0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // blue
        };

        float triangle2[] = {
             -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f 
        };


        float quad[] = {
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f
        };

        Mesh triangleMesh(triangle, sizeof(triangle), 6);

        Mesh triangleMesh2(triangle2, sizeof(triangle), 6);

        Mesh quadMesh(quad, sizeof(quad), 6);

        const char* vs = "#version 330 core\nlayout(location=0) in vec3 vp;\nlayout(location=1) in vec3 color;\nout vec3 fragColor;\nvoid main(){fragColor = color;gl_Position=vec4(vp,1.0);}";
        const char* fs = "#version 330 core\nin vec3 fragColor;\nout vec4 outColor;\nvoid main() {\noutColor = vec4(fragColor, 1.0);}";


        const char* fsYellow = R"(
            #version 330 core
            out vec4 outColor;
            void main() {
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
            }
            )";

        Shader shader1(vs, fs);
        Shader shader2(vs, fsYellow);

        while (!window.shouldClose()) {
            glClear(GL_COLOR_BUFFER_BIT);

            shader1.use();
            triangleMesh.draw();

            shader1.use();
            quadMesh.draw();

            shader2.use();
            triangleMesh2.draw();

            window.swapBuffers();
            window.pollEvents();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

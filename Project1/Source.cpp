/*#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

// Error callback for GLFW
void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main(void)
{
    // Inicializace GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        exit(EXIT_FAILURE);
    }

    // Inicializace verze OpenGL (3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Vytvoření okna
    GLFWwindow* window = glfwCreateWindow(800, 600, "ZPG", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Zapnutí vertikální synchronizace

    // Inicializace GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: could not start GLEW\n");
        exit(EXIT_FAILURE);
    }

    // Získání informací o verzi OpenGL
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("Vendor %s\n", glGetString(GL_VENDOR));
    printf("Renderer %s\n", glGetString(GL_RENDERER));
    printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("Using GLFW %i.%i.%i\n", major, minor, revision);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float)height;
    glViewport(0, 0, width, height);



    float points[] = {
        1.0f,  -0.5f, 0.0f,
        1.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f
    };

    float points_quad[] = {
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f
    };

    // Shadery
    const char* vertex_shader =
        "#version 330\n"
        "layout(location=0) in vec3 vp;\n"
        "void main() {\n"
        "    gl_Position = vec4(vp, 1.0);\n"
        "}";

    const char* fragment_shader =
        "#version 330\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    fragColor = vec4(0.5, 0.0, 0.5, 1.0);\n"
        "}";

    const char* vertex_shader_quad =
        "#version 330\n"
        "layout(location=0) in vec3 vp;\n"
        "void main() {\n"
        "    gl_Position = vec4(vp, 1.0);\n"
        "}";

    const char* fragment_shader_quad =
        "#version 330\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    fragColor = vec4(0.5, 0.0, 0.5, 1.0);\n"
        "}";

    // Vytvoření a inicializace bufferů
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    GLuint VBO2, VAO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points_quad), points_quad, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0); //enable vertex attributes 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(1); //enable vertex attributes 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

    // Kompilace shaderů
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint infoLogLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = new char[infoLogLength];
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, infoLog);
        fprintf(stderr, "Vertex Shader compilation failed: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint infoLogLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = new char[infoLogLength];
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, infoLog);
        fprintf(stderr, "Fragment Shader compilation failed: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }

    GLuint vertexShader_quad1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader_quad1, 1, &vertex_shader_quad, NULL);
    glCompileShader(vertexShader_quad1);

    GLuint fragmentShader_quad1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_quad1, 1, &fragment_shader_quad, NULL);
    glCompileShader(fragmentShader_quad1);
    glGetShaderiv(fragmentShader_quad1, GL_COMPILE_STATUS, &success);

    // Vytvoření shaderového programu
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLuint shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader_quad1);
    glAttachShader(shaderProgram2, fragmentShader_quad1);
    glLinkProgram(shaderProgram2);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = new char[infoLogLength];
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }

    // Vykreslovací smyčka
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);// trojúhelník

        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);// trojúhelník

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
*/
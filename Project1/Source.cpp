//Include GLFW 
/*#include <GL/glew.h> 
#include <GLFW/glfw3.h>  
#include <GL/gl.h>


//Include GLM  
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr



//Include the standard C++ headers  
#include <stdlib.h>
#include <stdio.h>

float directory = 1.0f;

static void error_callback(int error, const char* description) { fputs(description, stderr); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);

	if (directory == 1) {
		directory = -1.0f;
	}
	else {
		directory = 1.0f;
	}
}

static void window_focus_callback(GLFWwindow* window, int focused) { printf("window_focus_callback \n"); }

static void window_iconify_callback(GLFWwindow* window, int iconified) { printf("window_iconify_callback \n"); }

static void window_size_callback(GLFWwindow* window, int width, int height) {
	printf("resize %d, %d \n", width, height);
	glViewport(0, 0, width, height);
}

static void cursor_callback(GLFWwindow* window, double x, double y) { printf("cursor_callback \n"); }

static void button_callback(GLFWwindow* window, int button, int action, int mode) {
	if (action == GLFW_PRESS) printf("button_callback [%d,%d,%d]\n", button, action, mode);
}



//GLM test

// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 100.0f);

// Camera matrix
glm::mat4 View = glm::lookAt(
	glm::vec3(10, 10, 10), // Camera is at (4,3,-3), in World Space
	glm::vec3(0, 0, 0), // and looks at the origin
	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
);
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);



int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "ZPG", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Sets the key callback
	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_callback);

	glfwSetMouseButtonCallback(window, button_callback);

	glfwSetWindowFocusCallback(window, window_focus_callback);

	glfwSetWindowIconifyCallback(window, window_iconify_callback);

	glfwSetWindowSizeCallback(window, window_size_callback);


	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float ratio = width / (float)height;
	glViewport(0, 0, width, height);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);



	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, directory);

		glBegin(GL_QUADS);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.6f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(-0.6f, 0.6f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.6f, 0.6f, 0.f);
		glColor3f(1.f, 1.f, 0.f);
		glVertex3f(0.6f, -0.6f, 0.f);

		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.0f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(-0.6f, 0.6f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.0f, 0.6f, 0.f);
		glColor3f(1.f, 1.f, 0.f);
		glVertex3f(0.0f, -0.0f, 0.f);
		glEnd();
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}*/


#include <GL/glew.h>
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
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

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

    /*float points[] = {
       -3.0f,  0.5f, 0.0f,
        -2.5f, -0.5f, 0.0f,
       -3.5f, -0.5f, 0.0f
    };*/

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

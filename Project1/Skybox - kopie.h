#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Skybox {
private:
    GLuint VAO;
    GLuint VBO;
    GLuint cubemapTexture;
    std::shared_ptr<ShaderProgram> shaderProgram;
    
    void setupMesh();
    GLuint loadCubemap(const std::vector<std::string>& faces);

public:
    Skybox(const std::vector<std::string>& faces, std::shared_ptr<ShaderProgram> program);
    ~Skybox();
    
    void draw(const glm::mat4& view, const glm::mat4& projection);
};

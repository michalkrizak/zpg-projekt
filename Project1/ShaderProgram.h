#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include <glm/glm.hpp>

class ShaderProgram {
public:
    ShaderProgram(const Shader& vs, const Shader& fs);
    ~ShaderProgram();

    void useProgram() const;
    GLuint getId() const;

    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

private:
    GLuint programId;
};

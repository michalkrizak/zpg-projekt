#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include "CameraObserver.h"
#include "LightObserver.h"

class Shader;  // P�ed deklarac� Shaderu

class ShaderProgram : public ICameraObserver, public ILightObserver {
    friend class Shader;  // Shader m� p��stup k priv�tn�m �len�m

public:
    ShaderProgram(const Shader& vs, const Shader& fs);
    ~ShaderProgram();

    void useProgram() const;

    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

    // ICameraObserver
    void onCameraChanged(const glm::mat4& view, const glm::mat4& projection) override;

    // ILightObserver
    void onLightChanged(const glm::vec3& position, const glm::vec3& color) override;

    // Helper: connect to a camera (registration will be done outside)
    void setInitialViewProj(const glm::mat4& view, const glm::mat4& projection);
private:
    GLuint programId;
    glm::mat4 cachedView{1.0f};
    glm::mat4 cachedProj{1.0f};
};
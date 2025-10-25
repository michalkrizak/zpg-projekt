#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "CameraObserver.h"
#include "LightObserver.h"

class ShaderProgram : public ICameraObserver, public ILightObserver {
public:
    ShaderProgram(const std::vector<Shader*>& shaders);
    ~ShaderProgram();

    void useProgram() const;

    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

    // ICameraObserver
    void onCameraChanged(const glm::mat4& view, const glm::mat4& projection) override;

    // ILightObserver
    void onLightChanged(const glm::vec3& position, const glm::vec3& color) override;

    // Helper: connect to a camera (registration will be done outside)
    void setInitialViewProj(const glm::mat4& view, const glm::mat4& projection);
    
    // Multiple lights support
    void updateLightsUniforms();
    void clearLights();
    
private:
    static constexpr int MAX_LIGHTS = 16;
    
    struct LightData {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    GLuint programId;
    glm::mat4 cachedView{1.0f};
    glm::mat4 cachedProj{1.0f};
    std::vector<LightData> lights;
    std::vector<Shader*> shaders;
};
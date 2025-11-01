#include "ShaderProgram.h"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

ShaderProgram::ShaderProgram(const std::vector<Shader*>& shaders) : shaders(shaders) {
    programId = glCreateProgram();

    // Attach all shaders
    for (const auto& shader : shaders) {
        shader->attachShader(programId);
    }

    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(programId, logLen, nullptr, log.data());
        throw std::runtime_error("Shader linking failed: " + std::string(log.data()));
    }

    // Detach all shaders after linking
    for (const auto& shader : shaders) {
        shader->detachShader(programId);
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programId);
}

void ShaderProgram::useProgram() const {
    glUseProgram(programId);
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform4fv(location, 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) const {
    int location = glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::onCameraChanged(const glm::mat4& view, const glm::mat4& projection) {
    cachedView = view;
    cachedProj = projection;
    useProgram();
    setUniform("view", cachedView);
    setUniform("projection", cachedProj);
    // Reset program binding
    glUseProgram(0);
}

void ShaderProgram::setInitialViewProj(const glm::mat4& view, const glm::mat4& projection) {
    cachedView = view;
    cachedProj = projection;
}

void ShaderProgram::onLightChanged(const glm::vec3& position, const glm::vec3& color) {
    // Add or update light
    bool found = false;
    for (auto& light : lights) {
        if (glm::all(glm::epsilonEqual(light.position, position, 0.001f))) {
            light.color = color;
            found = true;
            break;
        }
    }
    
    if (!found && lights.size() < MAX_LIGHTS) {
        lights.push_back(SimpleLightData{position, color});
    }
    
    updateLightsUniforms();
}

void ShaderProgram::updateLightsUniforms() {
    useProgram();
    
    // Set number of lights
    setUniform("numLights", static_cast<int>(lights.size()));
    
    // Set each light's data
    for (size_t i = 0; i < lights.size() && i < MAX_LIGHTS; ++i) {
        std::string posName = "lights[" + std::to_string(i) + "].position";
        std::string colName = "lights[" + std::to_string(i) + "].color";
        
        setUniform(posName, lights[i].position);
        setUniform(colName, lights[i].color);
    }

    // Also support legacy single-light shaders (e.g., ground.frag)
    if (!lights.empty()) {
        setUniform("lightPosition", lights[0].position);
        setUniform("lightColor", lights[0].color);
    }
    // Reset program binding
    glUseProgram(0);
}

void ShaderProgram::clearLights() {
    lights.clear();
    updateLightsUniforms();
}

void ShaderProgram::setAdvancedLights(const std::vector<::LightData>& allLights) {
    useProgram();
    
    int enabledCount = 0;
    for (size_t i = 0; i < allLights.size() && i < MAX_LIGHTS; ++i) {
        const auto& light = allLights[i];
        
        std::string prefix = "lights[" + std::to_string(i) + "].";
        
        setUniform(prefix + "type", static_cast<int>(light.type));
        setUniform(prefix + "position", light.position);
        setUniform(prefix + "direction", light.direction);
        setUniform(prefix + "color", light.color);
        setUniform(prefix + "intensity", light.intensity);
        setUniform(prefix + "cutOff", std::cos(light.cutOff));  // Předpočítáme cos pro shader
        setUniform(prefix + "outerCutOff", std::cos(light.outerCutOff));
        setUniform(prefix + "constant", light.constant);
        setUniform(prefix + "linear", light.linear);
        setUniform(prefix + "quadratic", light.quadratic);
        setUniform(prefix + "enabled", light.enabled ? 1 : 0);
        
        if (light.enabled) {
            enabledCount++;
        }
    }
    
    setUniform("numLights", static_cast<int>(allLights.size()));
    
    // Backward compatibility: pro starší shadery nastavíme první bodové světlo
    for (const auto& light : allLights) {
        if (light.enabled && light.type == LightType::POINT) {
            setUniform("lightPosition", light.position);
            setUniform("lightColor", light.color * light.intensity);
            break;
        }
    }
    // Reset program binding
    glUseProgram(0);
}

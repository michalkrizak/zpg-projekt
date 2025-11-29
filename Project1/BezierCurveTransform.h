#pragma once
#include "TransformComponent.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Kubická Bézierova křivka s 4 kontrolními body
class BezierCurveTransform : public TransformComponent {
public:
    BezierCurveTransform(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                         float duration, bool loop = true, bool orientToDirection = true)
        : p0(p0), p1(p1), p2(p2), p3(p3), 
          duration(duration), loop(loop), orientToDirection(orientToDirection),
          startTime(static_cast<float>(glfwGetTime())) {}

    glm::mat4 getMatrix() const override {
        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        
        float t = elapsed / duration;
        
        if (loop) {
            t = t - std::floor(t);
        } else {
            if (t > 1.0f) t = 1.0f;
        }
        
        glm::vec3 position = calculateBezierPoint(t);
        
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        
        if (orientToDirection) {
            glm::vec3 tangent = calculateBezierTangent(t);
            if (glm::length(tangent) > 0.001f) {
                tangent = glm::normalize(tangent);

                glm::vec3 defaultForward = glm::vec3(0.0f, 0.0f, 1.0f);
                
                glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent));
                if (glm::length(right) < 0.001f) {
                    right = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), tangent));
                }
                glm::vec3 up = glm::normalize(glm::cross(tangent, right));
                
                // Vytvoříme rotační matici z bázových vektorů
                glm::mat4 rotation = glm::mat4(1.0f);
                rotation[0] = glm::vec4(right, 0.0f);
                rotation[1] = glm::vec4(up, 0.0f);
                rotation[2] = glm::vec4(tangent, 0.0f);
                
                matrix = matrix * rotation;
            }
        }
        
        return matrix;
    }

    void reset() {
        startTime = static_cast<float>(glfwGetTime());
    }

    // Veřejné gettery pro debugging
    glm::vec3 getCurrentPosition() const {
        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        float t = elapsed / duration;
        if (loop) t = t - std::floor(t);
        else if (t > 1.0f) t = 1.0f;
        return calculateBezierPoint(t);
    }

    glm::vec3 getCurrentTangent() const {
        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        float t = elapsed / duration;
        if (loop) t = t - std::floor(t);
        else if (t > 1.0f) t = 1.0f;
        return calculateBezierTangent(t);
    }

private:
    glm::vec3 p0, p1, p2, p3; // 4 kontrolní body
    float duration;
    bool loop;
    bool orientToDirection;
    mutable float startTime;

    // Výpočet bodu na kubické Bézierově křivce
    // B(t) = (1-t)³*P0 + 3(1-t)²t*P1 + 3(1-t)t²*P2 + t³*P3
    glm::vec3 calculateBezierPoint(float t) const {
        float oneMinusT = 1.0f - t;
        float oneMinusT2 = oneMinusT * oneMinusT;
        float oneMinusT3 = oneMinusT2 * oneMinusT;
        float t2 = t * t;
        float t3 = t2 * t;
        
        return oneMinusT3 * p0 +
               3.0f * oneMinusT2 * t * p1 +
               3.0f * oneMinusT * t2 * p2 +
               t3 * p3;
    }

    // Výpočet tečného vektoru (derivace Bézierovy křivky)
    // B'(t) = 3(1-t)²(P1-P0) + 6(1-t)t(P2-P1) + 3t²(P3-P2)
    glm::vec3 calculateBezierTangent(float t) const {
        float oneMinusT = 1.0f - t;
        float oneMinusT2 = oneMinusT * oneMinusT;
        float t2 = t * t;
        
        return 3.0f * oneMinusT2 * (p1 - p0) +
               6.0f * oneMinusT * t * (p2 - p1) +
               3.0f * t2 * (p3 - p2);
    }
};

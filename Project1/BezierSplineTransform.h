#pragma once
#include "TransformComponent.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Bézierova spline křivka složená z více kubických Bézierových segmentů
// Uživatel zadá n bodů, každé 4 body tvoří jeden segment
class BezierSplineTransform : public TransformComponent {
public:
    BezierSplineTransform(const std::vector<glm::vec3>& controlPoints, 
                          float duration, bool loop = true, bool orientToDirection = true)
        : controlPoints(controlPoints), duration(duration), loop(loop), 
          orientToDirection(orientToDirection),
          startTime(static_cast<float>(glfwGetTime())) {
        
        // Vypočítáme počet segmentů (každý segment má 4 body)
        if (controlPoints.size() >= 4) {
            numSegments = (controlPoints.size() - 1) / 3;
        } else {
            numSegments = 0;
        }
    }

    glm::mat4 getMatrix() const override {
        if (numSegments == 0 || controlPoints.size() < 4) {
            return glm::mat4(1.0f);
        }

        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        
        // Normalizovaný čas (0.0 - 1.0) pro celou spline
        float globalT = elapsed / duration;
        
        if (loop) {
            globalT = globalT - std::floor(globalT);
        } else {
            if (globalT > 1.0f) globalT = 1.0f;
        }
        
        // Pozice na spline křivce
        glm::vec3 position = calculateSplinePoint(globalT);
        
        // Základní translační matice
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        
        // Orientace podle směru pohybu
        if (orientToDirection) {
            glm::vec3 tangent = calculateSplineTangent(globalT);
            if (glm::length(tangent) > 0.001f) {
                tangent = glm::normalize(tangent);
                
                // Vytvoříme rotační matici
                glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent));
                if (glm::length(right) < 0.001f) {
                    right = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), tangent));
                }
                glm::vec3 up = glm::normalize(glm::cross(tangent, right));
                
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

    // Přidání dalších kontrolních bodů dynamicky
    void addControlPoint(const glm::vec3& point) {
        controlPoints.push_back(point);
        if (controlPoints.size() >= 4) {
            numSegments = (controlPoints.size() - 1) / 3;
        }
    }

    void clearControlPoints() {
        controlPoints.clear();
        numSegments = 0;
    }

    size_t getControlPointCount() const {
        return controlPoints.size();
    }

    int getSegmentCount() const {
        return numSegments;
    }

    glm::vec3 getCurrentPosition() const {
        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        float globalT = elapsed / duration;
        if (loop) globalT = globalT - std::floor(globalT);
        else if (globalT > 1.0f) globalT = 1.0f;
        return calculateSplinePoint(globalT);
    }

private:
    std::vector<glm::vec3> controlPoints;
    int numSegments;
    float duration;
    bool loop;
    bool orientToDirection;
    mutable float startTime;

    // Výpočet bodu na spline křivce
    glm::vec3 calculateSplinePoint(float globalT) const {
        if (numSegments == 0) return glm::vec3(0.0f);
        
        // Určíme, ve kterém segmentu se nacházíme
        float segmentFloat = globalT * numSegments;
        int segmentIndex = static_cast<int>(std::floor(segmentFloat));
        
        // Ošetření krajního případu
        if (segmentIndex >= numSegments) {
            segmentIndex = numSegments - 1;
            segmentFloat = static_cast<float>(numSegments);
        }
        
        // Lokální t v rámci daného segmentu (0.0 - 1.0)
        float localT = segmentFloat - segmentIndex;
        
        // Získáme 4 kontrolní body pro daný segment
        int baseIndex = segmentIndex * 3;
        glm::vec3 p0 = controlPoints[baseIndex];
        glm::vec3 p1 = controlPoints[baseIndex + 1];
        glm::vec3 p2 = controlPoints[baseIndex + 2];
        glm::vec3 p3 = controlPoints[baseIndex + 3];
        
        // Výpočet bodu na kubické Bézierově křivce
        return calculateBezierPoint(p0, p1, p2, p3, localT);
    }

    // Výpočet tečného vektoru na spline křivce
    glm::vec3 calculateSplineTangent(float globalT) const {
        if (numSegments == 0) return glm::vec3(0.0f, 0.0f, 1.0f);
        
        float segmentFloat = globalT * numSegments;
        int segmentIndex = static_cast<int>(std::floor(segmentFloat));
        
        if (segmentIndex >= numSegments) {
            segmentIndex = numSegments - 1;
            segmentFloat = static_cast<float>(numSegments);
        }
        
        float localT = segmentFloat - segmentIndex;
        
        int baseIndex = segmentIndex * 3;
        glm::vec3 p0 = controlPoints[baseIndex];
        glm::vec3 p1 = controlPoints[baseIndex + 1];
        glm::vec3 p2 = controlPoints[baseIndex + 2];
        glm::vec3 p3 = controlPoints[baseIndex + 3];
        
        return calculateBezierTangent(p0, p1, p2, p3, localT);
    }

    // Pomocné funkce pro výpočet Bézierovy křivky
    glm::vec3 calculateBezierPoint(const glm::vec3& p0, const glm::vec3& p1, 
                                   const glm::vec3& p2, const glm::vec3& p3, float t) const {
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

    glm::vec3 calculateBezierTangent(const glm::vec3& p0, const glm::vec3& p1,
                                     const glm::vec3& p2, const glm::vec3& p3, float t) const {
        float oneMinusT = 1.0f - t;
        float oneMinusT2 = oneMinusT * oneMinusT;
        float t2 = t * t;
        
        return 3.0f * oneMinusT2 * (p1 - p0) +
               6.0f * oneMinusT * t * (p2 - p1) +
               3.0f * t2 * (p3 - p2);
    }
};

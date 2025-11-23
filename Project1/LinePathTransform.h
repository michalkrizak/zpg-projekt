#pragma once
#include "TransformComponent.h"
#include <GLFW/glfw3.h>
#include <vector>


class LinePathTransform : public TransformComponent {
public:

    LinePathTransform(const std::vector<glm::vec3>& points, float duration, bool loop = true)
        : points(points), duration(duration), loop(loop), startTime(static_cast<float>(glfwGetTime())) {
        if (points.size() < 2) {
            // Fallback pro případ nedostatečného počtu bodů
            this->points = {glm::vec3(0.0f), glm::vec3(0.0f)};
        }
    }

    glm::mat4 getMatrix() const override {
        if (points.size() < 2) {
            return glm::mat4(1.0f);
        }

        float currentTime = static_cast<float>(glfwGetTime());
        float elapsed = currentTime - startTime;
        
        // Normalizovaný čas (0.0 - 1.0) pro celou cestu
        float t = elapsed / duration;
        
        if (loop) {
            // Cyklické opakování
            t = t - std::floor(t);
        } else {
            // Zastavení na konci
            if (t > 1.0f) t = 1.0f;
        }
        
        // Pozice na lomené čáře
        glm::vec3 position = interpolate(t);
        
        return glm::translate(glm::mat4(1.0f), position);
    }

    void reset() {
        startTime = static_cast<float>(glfwGetTime());
    }

private:
    std::vector<glm::vec3> points;
    float duration;
    bool loop;
    float startTime;


    glm::vec3 interpolate(float t) const {
        int numSegments = static_cast<int>(points.size()) - 1;
        if (numSegments < 1) return points[0];

        // Určíme, ve kterém segmentu se nacházíme
        float segmentLength = 1.0f / numSegments;
        int segmentIndex = static_cast<int>(t / segmentLength);
        
        // Ošetření krajního případu (t = 1.0)
        if (segmentIndex >= numSegments) {
            segmentIndex = numSegments - 1;
        }
        
        // Lokální t pro aktuální segment (0.0 - 1.0)
        float localT = (t - segmentIndex * segmentLength) / segmentLength;
        
        // Lineární interpolace mezi dvěma body
        const glm::vec3& p0 = points[segmentIndex];
        const glm::vec3& p1 = points[segmentIndex + 1];
        
        return p0 + localT * (p1 - p0);
    }
};

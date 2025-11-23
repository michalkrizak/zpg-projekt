#pragma once
#include <vector>
#include <memory>
#include <random>
#include "Target.h"


class ArcadeGame {
public:
    ArcadeGame() : score(0), rng(std::random_device{}()) {}


    void addTarget(Target* target) {
        targets.push_back(target);
    }


    int hitTarget(unsigned int targetID) {
        for (auto* target : targets) {
            if (target->getID() == targetID && target->isActive()) {
                int points = target->getPointValue();
                score += points;
                target->setActive(false);  // Deaktivace cíle po zásahu
                return points;
            }
        }
        return 0;
    }

    void reset() {
        score = 0;
        for (auto* target : targets) {
            target->setActive(true);
        }
    }

    int getScore() const { return score; }


    glm::vec3 getRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
        std::uniform_real_distribution<float> distX(minX, maxX);
        std::uniform_real_distribution<float> distY(minY, maxY);
        std::uniform_real_distribution<float> distZ(minZ, maxZ);
        return glm::vec3(distX(rng), distY(rng), distZ(rng));
    }


    std::vector<glm::vec3> generateRandomPath(int numPoints, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
        std::vector<glm::vec3> path;
        for (int i = 0; i < numPoints; ++i) {
            path.push_back(getRandomPosition(minX, maxX, minY, maxY, minZ, maxZ));
        }
        return path;
    }


    float getRandomDuration(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

private:
    std::vector<Target*> targets;
    int score;
    std::mt19937 rng;
};

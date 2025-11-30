#pragma once
#include "DrawableObject.h"


class Target : public DrawableObject {
public:
    enum class TargetType {
        SPHERE,   // 10 points
        CUBE,     // 20 points
        GIFT      // 50 points
    };

    Target(std::unique_ptr<Model> m, std::shared_ptr<ShaderProgram> sp, TargetType type)
        : DrawableObject(std::move(m), sp), targetType(type) {
        switch (type) {
            case TargetType::SPHERE:
                pointValue = 10;
                break;
            case TargetType::CUBE:
                pointValue = 20;
                break;
            case TargetType::GIFT:
                pointValue = 50;
                break;
        }
    }

    TargetType getTargetType() const { return targetType; }
    int getPointValue() const { return pointValue; }
    
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }

private:
    TargetType targetType;
    int pointValue;
    bool active = true;  // Whether target is active (can be hit)
};

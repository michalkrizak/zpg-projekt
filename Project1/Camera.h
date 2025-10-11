#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "CameraObserver.h"

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    void processKeyboard(char key, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);

    glm::vec3 getPosition() const;

    // Observer management
    void addObserver(const std::shared_ptr<ICameraObserver>& obs);
    void removeObserver(const std::shared_ptr<ICameraObserver>& obs);
    void notifyObservers(float aspect);

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    // Projection parameters
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    std::vector<std::weak_ptr<ICameraObserver>> observers;

    void updateCameraVectors();
};

#endif
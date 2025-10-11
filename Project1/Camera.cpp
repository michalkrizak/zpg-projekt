#include "Camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::processKeyboard(char key, float deltaTime) {
    float velocity = 2.5f * deltaTime;
    if (key == 'W') position += front * velocity;
    if (key == 'S') position -= front * velocity;
    if (key == 'A') position -= right * velocity;
    if (key == 'D') position += right * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset) {
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);

    right = glm::normalize(glm::cross(this->front, worldUp));
    up = glm::normalize(glm::cross(right, this->front));
}

glm::vec3 Camera::getPosition() const {
    return position;
}

void Camera::addObserver(const std::shared_ptr<ICameraObserver>& obs) {
    observers.emplace_back(obs);
}

void Camera::removeObserver(const std::shared_ptr<ICameraObserver>& obs) {
    observers.erase(std::remove_if(observers.begin(), observers.end(), [&](const std::weak_ptr<ICameraObserver>& w) {
        auto sp = w.lock();
        return !sp || sp == obs;
    }), observers.end());
}

void Camera::notifyObservers(float aspect) {
    glm::mat4 view = getViewMatrix();
    glm::mat4 proj = getProjectionMatrix(aspect);
    for (auto it = observers.begin(); it != observers.end(); ) {
        if (auto sp = it->lock()) {
            sp->onCameraChanged(view, proj);
            ++it;
        } else {
            it = observers.erase(it);
        }
    }
}
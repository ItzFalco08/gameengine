#include "Camera.hpp"
#include "SceneView.hpp"
#include <glm/gtc/matrix_transform.hpp>
extern SceneView sceneView;

void Camera::rotate(double dx, double dy) {
    _yaw += dx;
    _pitch += dy;

    if (_pitch > 89.0)
        _pitch = 89.0;

    if (_pitch < -89.0)
        _pitch = -89.0;

    front.x = glm::cos(glm::radians(_pitch)) * glm::cos(glm::radians(_yaw));
    front.y = glm::sin(glm::radians(_pitch));
    front.z = glm::cos(glm::radians(_pitch)) * glm::sin(glm::radians(_yaw));

    viewDirty = true;
}

void Camera::move(glm::vec3 deltaPos) {
    position += deltaPos;
    viewDirty = true;
}

glm::mat4 Camera::getProjMat() {
    if (projDirty) {
        calculateProj();
        projDirty = false;
    }

    return projectionMat;
}

glm::mat4 Camera::getViewMat() {
    if (viewDirty) {
        calculateView();
        viewDirty = false;
    }

    return viewMat;
}

void Camera::calculateProj() {
    projectionMat = glm::perspective(
        glm::radians(fov),
        sceneView.getAspect(),
        _near,
        _far
    );
}

void Camera::calculateView() {
    viewMat = glm::lookAt(position, position + front, up);
}
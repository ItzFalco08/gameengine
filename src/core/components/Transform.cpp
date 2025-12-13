#include "Transform.hpp"

void Transform::recalculateMatrix() {
    model = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
}

// Rotation
void Transform::Rotate(const glm::vec3& deltaGlobalEulars) {
    rotation = glm::normalize(glm::quat(glm::radians(deltaGlobalEulars)) * rotation);
    dirty = true;
}

void Transform::RotateLocal(const glm::vec3 localEulars) {
    rotation = glm::normalize(rotation * glm::quat(glm::radians(localEulars)));
    dirty = true;
}

void Transform::SetRotation(const glm::vec3& globalEulars) {
    rotation = glm::normalize(glm::quat(glm::radians(globalEulars)));
    dirty = true;
}

// Translation
void Transform::Translate(const glm::vec3& offset) {
    position += offset;
    dirty = true;
}
// Scale
void Transform::Scale(const glm::vec3& factor) {
    scale *= factor;
    dirty = true;
}

glm::mat4 Transform::getModel() {
    if(dirty) {
        recalculateMatrix();
        dirty = false;
    };
    return model;
}

void Transform::reset() {
    position = glm::vec3(0.0f);
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    dirty = true;
}
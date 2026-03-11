#include "Transform.hpp"
#include "../GameObject.hpp"

void Transform::MakeDirty() {
    if (!dirty) {
        dirty = true;
    }
}

void Transform::recalculateMatrix() {
    // POSSIBLE OPTIMIZATION
    model = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
}

// Rotation
void Transform::Rotate(const glm::vec3& deltaGlobalEulars) {
    rotation = glm::normalize(glm::quat(glm::radians(deltaGlobalEulars)) * rotation);
    dirty = true;
}

void Transform::RotateLocal(const glm::vec3& localEulars) {
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

glm::mat4 Transform::getNormalMat()
{
    if (dirty) { // recalculate only when model changes
        normalMat = glm::transpose(glm::inverse(glm::mat3(getModel())));
        dirty = false;
    }

    return normalMat;
}

Transform::~Transform() noexcept {
    LOG::Info("Transform Destroyed");
}


Transform::Transform() {
    LOG::Info("Transfrom Created");
}

void Transform::reset() {
    position = glm::vec3(0.0f);
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    dirty = true;
}


void Transform::Serialize(nlohmann::json& json) {
    json["position"] = {position.x, position.y, position.z};
    json["rotation"] = {rotation.w, rotation.x, rotation.y, rotation.z};
    json["scale"] = {scale.x, scale.y, scale.z};
}

void Transform::Deserialize(nlohmann::json& json) {
    position = glm::vec3(json.at("position")[0], json.at("position")[1], json.at("position")[2]);
    rotation = glm::quat(json.at("rotation")[0], json.at("rotation")[1], json.at("rotation")[2], json.at("rotation")[3]);
    scale = glm::vec3(json.at("scale")[0], json.at("scale")[1], json.at("scale")[2]);

    MakeDirty();
}

glm::vec3 Transform::getAbsolutePosition() {
    return parent->transform->getAbsolutePosition() + position;
}
#include "Transform.hpp"
#include "../GameObject.hpp"
#include "ImGuizmo/imGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

void Transform::recalculateMatrix() {
    // POSSIBLE OPTIMIZATION
    model = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
}

// Rotation
void Transform::Rotate(const glm::vec3& deltaGlobalEulars) {
    rotation = glm::normalize(glm::quat(glm::radians(deltaGlobalEulars)) * rotation);
    modelDirty = true;
    normalDirty= true;
}

void Transform::RotateLocal(const glm::vec3& localEulars) {
    rotation = glm::normalize(rotation * glm::quat(glm::radians(localEulars)));
    modelDirty = true;
    normalDirty = true;}

void Transform::SetRotationGlobal(const glm::vec3& globalEulars) {
    gEulars = globalEulars;
    rotation = glm::normalize(glm::quat(glm::radians(gEulars)));
    modelDirty = true;
    normalDirty = true;
}

// Translation
void Transform::Translate(const glm::vec3& offset) {
    position += offset;
    modelDirty = true;
}
// Scale
void Transform::Scale(const glm::vec3& factor) {
    scale *= factor;
    modelDirty = true;
    normalDirty = true;
}

void Transform::DecomposeModel() {
    glm::vec3 extractedEuler;
    
    // Decompose into temp variables
    ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(model), 
        glm::value_ptr(position), 
        glm::value_ptr(extractedEuler), // DO NOT pass &rotation[0]
        glm::value_ptr(scale)
    );

    // Sync our internal state
    gEulars = extractedEuler; 
    rotation = glm::normalize(glm::quat(glm::radians(gEulars)));
    
    normalDirty = true;
}

glm::mat4& Transform::getModel() {
    if(modelDirty) {
        recalculateMatrix();
        modelDirty = false;
    };
    return model;
}

glm::mat4 Transform::getNormalMat()
{
    if (normalDirty) { // recalculate only when model changes
        normalMat = glm::transpose(glm::inverse(glm::mat3(getModel())));
    }

    return normalMat;
}


void Transform::reset() {
    position = glm::vec3(0.0f);
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    normalDirty = true;
    modelDirty = true;
}


void Transform::Serialize(nlohmann::json& json) {
    json["position"] = {position.x, position.y, position.z};
    json["rotation"] = {rotation.w, rotation.x, rotation.y, rotation.z};
    json["scale"] = {scale.x, scale.y, scale.z};
    json["gEulars"] = {gEulars.x, gEulars.y, gEulars.x};
}

void Transform::Deserialize(nlohmann::json& json) {
    position = glm::vec3(json.at("position")[0], json.at("position")[1], json.at("position")[2]);
    rotation = glm::quat(json.at("rotation")[0], json.at("rotation")[1], json.at("rotation")[2], json.at("rotation")[3]);
    scale = glm::vec3(json.at("scale")[0], json.at("scale")[1], json.at("scale")[2]);
    gEulars = glm::vec3(json.at("gEulars")[0], json.at("gEulars")[1], json.at("gEulars")[2]);

    normalDirty = true;
    modelDirty = true;
}

glm::vec3 Transform::getAbsolutePosition() {
    return parent ? (parent->transform->getAbsolutePosition()  + position) : position;
}
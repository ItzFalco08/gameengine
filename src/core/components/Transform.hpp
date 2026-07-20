#pragma once
#include <glm/glm.hpp>
#include "Component.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "json/json.hpp"

struct Transform : public Component  {
private:
    void recalculateMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 normalMat = glm::mat4(1.0f);

public:
    bool modelDirty = false;
    bool normalDirty = false;
    
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion (w, x, y, z)
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 gEulars = glm::vec3(0.0f); // Store persistent Euler angles for UI

    static const char* StaticType() { return "Transform"; };
    std::string GetType() override { return StaticType(); }
    void Rotate(const glm::vec3& deltaGlobalEulars);
    void SetRotationGlobal(const glm::vec3& globalEulars);
    void RotateLocal(const glm::vec3& localEulars);
    void Translate(const glm::vec3& offset);
    void Scale(const glm::vec3& factor);
    void DecomposeModel();
    glm::mat4& getModel();
    glm::mat4 getNormalMat();
    glm::vec3 getAbsolutePosition();
    ~Transform() = default;
    Transform() = default;

    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;

    void reset();
};
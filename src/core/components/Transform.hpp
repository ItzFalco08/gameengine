#pragma once
#include "glm/glm.hpp"
#include "Component.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform : public Component  {
private:
    void recalculateMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    bool dirty = false;

public:
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion (w, x, y, z)
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    size_t GetId() override;
    void Rotate(const glm::vec3& deltaGlobalEulars);
    void SetRotation(const glm::vec3& globalEulars);
    void RotateLocal(const glm::vec3 localEulars);
    void Translate(const glm::vec3& offset);
    void Scale(const glm::vec3& factor);
    glm::mat4 getModel();

    void reset();
};
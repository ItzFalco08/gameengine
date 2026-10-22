#pragma once
#include "Component.hpp"
#include "glm/glm.hpp"
#include "json/json.hpp"

class SceneManager;

enum class LightType {
    POINT,
    DIRECTIONAL,
};

class LightComponent : public Component {
public:
    LightType type = LightType::POINT;
    glm::vec3 color = { 1, 1, 1 };
    float intensity = 10.0f;
    float range = 30.0f;

    ComponentType GetType() override { return ComponentType::Light; }
    void Serialize(nlohmann::json& json) override;
    void Deserialize(const nlohmann::json& json) override;
};
// no idont

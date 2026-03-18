#pragma once
#include "Component.hpp"
#include "glm/glm.hpp"
#include "json/json.hpp"

class SceneManager;
extern SceneManager sceneManager;

enum class LightType {
    POINT,
    DIRECTIONAL,
};

struct LightProps {
    virtual void Serialize(nlohmann::json& json) = 0;
    virtual void Deserialize(nlohmann::json& json) = 0;
};

struct DirLight : public LightProps {
    glm::vec3 lightColor = {1, 1, 1};

    DirLight() = default;
    DirLight(nlohmann::json& json);

    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;
};

struct PointLight : public LightProps{
    glm::vec3 lightColor = {1, 1, 1};
    float range = 30.0f;
    float intensity = 10.0f;

    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;
};

class Light : public Component {
public:
    LightType lightType = LightType::POINT;
    std::unique_ptr<LightProps> lightProps = std::make_unique<PointLight>();

    LightType getLightType();
    void setLight(LightType type);
    void Serialize(nlohmann::json& json);
    void Deserialize(nlohmann::json& json);

    Light();
    ~Light();
    static const char* StaticType() { return "Light"; }
    std::string GetType() override { return StaticType(); }
};
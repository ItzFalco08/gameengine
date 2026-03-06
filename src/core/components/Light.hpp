#pragma once
#include "Component.hpp"
#include "glm/glm.hpp"

enum LightType {
    DIRECTIONAL,
    POINT
};

struct LightProps {
    virtual void Serialize(nlohmann::json& json) = 0;
    virtual void Deserialize(nlohmann::json& json) = 0;
};

struct DirLight : public LightProps {
    glm::vec3 lightColor = {1, 1, 1};

    DirLight() = default;
    DirLight(nlohmann::json& json) { Deserialize(json); }

    void Serialize(nlohmann::json& json) override {
        json["lightColor"] = {lightColor.x, lightColor.y, lightColor.z};
    };
    void Deserialize(nlohmann::json& json) override {
        lightColor = {json["lightColor"][0], json["lightColor"][1], json["lightColor"][2]};
    };
};

struct PointLight : public LightProps{
    glm::vec3 lightColor = {1, 1, 1};
    float range = 30.0f;
    float intensity = 10.0f;

    void Serialize(nlohmann::json& json) override {
        json["lightColor"] = {lightColor.x, lightColor.y, lightColor.z};
        json["range"] = range;
        json["intensity"] = intensity;
    };
    void Deserialize(nlohmann::json& json) override {
        lightColor = {json["lightColor"][0], json["lightColor"][1], json["lightColor"][2]};
        range = json["range"];
        intensity = json["intensity"];
    };
};

class Light : public Component {
public:
    LightType lightType = POINT;
    std::unique_ptr<LightProps> lightProps = std::make_unique<PointLight>();

    void Serialize(nlohmann::json& json) {
        json["lightType"] = lightType;
        lightProps->Serialize(json);
    };

    void Deserialize(nlohmann::json& json) {
        LightType type = static_cast<LightType>(json["lightType"]);
        switch (type)
        {
        case LightType::DIRECTIONAL:
            lightProps = std::make_unique<DirLight>();
            lightProps->Deserialize(json);
            break;
        
        case LightType::POINT:
            lightProps = std::make_unique<PointLight>();
            lightProps->Deserialize(json);
            break;

        default:
            break;
        }
    };

    Light() = default;
    std::string GetType() override { return "Light"; };
};

#pragma once
#include "Component.hpp"
#include "glm/glm.hpp"

enum LightType {
    DIRECTIONAL,
    POINT
};

struct LightProps {
    virtual void Serialize(nlohmann::json& json);
    virtual void Deserialize(nlohmann::json& json);
};

struct DirLight : public LightProps {
    glm::vec3 lightColor = {1, 1, 1};

    DirLight() = default;
    DirLight(nlohmann::json& json) { Deserialize(json); }

    void Serialize(nlohmann::json& json) override {

    };
    void Deserialize(nlohmann::json& json) override {

    };
};

struct PointLight : public LightProps{
    glm::vec3 lightColor = {1, 1, 1};
    int range = 30;

    // attenution
    float constant = 1.0f;
    float linear =  0.07f;
    float quadratic = 0.017f;
    

    void Serialize(nlohmann::json& json) override {

    };
    void Deserialize(nlohmann::json& json) override {
        
    };
};

class Light : public Component {
public:
    LightType lightType = POINT;
    LightProps lightProps = PointLight();

    void Serialize(nlohmann::json& json) {
        json["lightType"] = lightType;
        lightProps.Serialize(json);
    };

    void Deserialize(nlohmann::json& json) {
        LightType type = static_cast<LightType>(json["lightType"]);
        switch (type)
        {
        case LightType::DIRECTIONAL:
            lightProps = DirLight();
            lightProps.Deserialize(json);
            break;
        
        case LightType::POINT:
            lightProps = PointLight();
            lightProps.Deserialize(json);
            break;

        default:
            break;
        }
    };

    Light() = default;
    std::string GetType() override { return "Light"; };
};

#include "Light.hpp"
#include "../Scene.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

// DirLight

DirLight::DirLight(nlohmann::json& json) { Deserialize(json); }

void DirLight::Serialize(nlohmann::json& json) {
    json["lightColor"] = {lightColor.x, lightColor.y, lightColor.z};
}

void DirLight::Deserialize(nlohmann::json& json) {
    lightColor = {json["lightColor"][0], json["lightColor"][1], json["lightColor"][2]};
}

// PointLight

void PointLight::Serialize(nlohmann::json& json) {
    json["lightColor"] = {lightColor.x, lightColor.y, lightColor.z};
    json["range"] = range;
    json["intensity"] = intensity;
}

void PointLight::Deserialize(nlohmann::json& json) {
    lightColor = {json["lightColor"][0], json["lightColor"][1], json["lightColor"][2]};
    range = json["range"];
    intensity = json["intensity"];
}

// Light

LightType Light::getLightType() {
    return lightType;
}

void Light::setLight(LightType type) {
    if (lightType == type) return;

    switch (type)
    {
    case LightType::POINT:
        lightProps = std::make_unique<PointLight>();
        break;
    case LightType::DIRECTIONAL:
        lightProps = std::make_unique<DirLight>();
        break;
    default:
        break;
    }
}

void Light::Serialize(nlohmann::json& json) {
    json["lightType"] = lightType;
    lightProps->Serialize(json);
}

void Light::Deserialize(nlohmann::json& json) {
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

    editorScene->lights.push_back(this);
}


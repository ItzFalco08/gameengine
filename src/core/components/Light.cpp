#include "Light.hpp"
#include "../SceneManager.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

extern SceneManager sceneManager;

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
    if (lightType == type) {
        LOG::Warning("Change of light to the same type | (Light::setLight)");
        return;
    };

    switch (type)
    {
    case LightType::POINT:
        lightProps = std::make_unique<PointLight>();
        lightType = LightType::POINT;
        break;
    case LightType::DIRECTIONAL:
        lightProps = std::make_unique<DirLight>();
        lightType = LightType::DIRECTIONAL;
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

    sceneManager.activeScene->lights.push_back(this);
}

Light::Light(){
    Light* ptr = this;

    if (sceneManager.activeScene) {
        sceneManager.activeScene->lights.push_back(ptr);

    } else {
        LOG::Warning("trying to add a Light component when active Scene is null");
    }
}

Light::~Light() {
    Light* ptr = this;
    auto itr = std::find(sceneManager.activeScene->lights.begin(), sceneManager.activeScene->lights.end(), ptr);
    if (itr != sceneManager.activeScene->lights.end()) {
        sceneManager.activeScene->lights.erase(itr);
    } else {
        LOG::Warning("destructuring a light that doesn't exists");
    }
}

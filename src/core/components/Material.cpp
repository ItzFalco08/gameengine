#include "Material.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>


Material::Material() {
    matprops = std::make_unique<LitMaterial>();
    LOG::Info("Material Created");
}

void Material::Serialize(nlohmann::json& json) {
    json["materialType"] = static_cast<int>(matprops->GetMatType());
    matprops->Serialize(json);
}

void Material::Deserialize(nlohmann::json& json) {
    int type = json["materialType"];
    switch (static_cast<MaterialType>(type))
    {
    case MaterialType::LIT:
        matprops = std::make_unique<LitMaterial>();
        break;
    case MaterialType::UNLIT:
        matprops = std::make_unique<UnlitMaterial>();
        break;
    
    default:
        LOG::Error("Material Type Not Found");
        break;
    }

    matprops->Deserialize(json);
}

void Material::SetMaterialType(MaterialType matType) {
    if (matprops->GetMatType() == matType) {
        LOG::Warning("Failed to set MaterialType (Material Already Exists)");
        return;
    }

    switch (matType)
    {
    case MaterialType::LIT:
        matprops = std::make_unique<LitMaterial>();
        break;

    case MaterialType::UNLIT:
        matprops = std::make_unique<UnlitMaterial>();
        break;
    
    default:
        break;
    }
};

std::string Material::GetType() {
    return "Material";
}

// L I T _ M A T E R I A L
void LitMaterial::Serialize(nlohmann::json& json) {
    json["ambientColor"] = {ambientColor.x, ambientColor.y, ambientColor.z};
    json["ambientStrength"] = ambientStrength;
    json["diffuseStrength"] = diffuseStrength;
    json["specularStrength"] = specularStrength;
    json["shininess"] = shininess;
}

void LitMaterial::Deserialize(nlohmann::json& json) {
    ambientColor = {json["ambientColor"][0], json["ambientColor"][1], json["ambientColor"][2]};
    ambientStrength = json["ambientStrength"]; 
    diffuseStrength = json["diffuseStrength"]; 
    specularStrength = json["specularStrength"]; 
    shininess = json["shininess"]; 
}

void LitMaterial::applyToShader(Shader* shader) {
    // future implementation
}

MaterialType LitMaterial::GetMatType() { return MaterialType::LIT; }

// U N L I T _ M A T E R I A L
void UnlitMaterial::Serialize(nlohmann::json& json) {
    json["ambientColor"] = {ambientColor.x, ambientColor.y, ambientColor.z};
}

void UnlitMaterial::Deserialize(nlohmann::json& json) {
    ambientColor = {json["ambientColor"][0], json["ambientColor"][1], json["ambientColor"][2]};
}

void UnlitMaterial::applyToShader(Shader* shader) {
    // future implementation
}

MaterialType UnlitMaterial::GetMatType() { return MaterialType::UNLIT; }


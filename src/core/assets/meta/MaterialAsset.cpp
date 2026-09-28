#include "MaterialAsset.hpp"
#include <json/json.hpp>
#include <fstream>

bool MaterialAsset::Serialize(const fs::path& assetFile) {
    if (!fs::exists(assetFile) || !fs::is_regular_file(assetFile))
        return false;

    try {
        std::ifstream file(assetFile);
        if (!file)
            return false;

        nlohmann::json j;
        file >> j;

        m_uuid = UUID::fromString(j.at("uuid").get<std::string>());

        ambientColor = glm::vec3(
            j.at("ambientColor").at(0),
            j.at("ambientColor").at(1),
            j.at("ambientColor").at(2)
        );

        diffuseColor = glm::vec3(
            j.at("diffuseColor").at(0),
            j.at("diffuseColor").at(1),
            j.at("diffuseColor").at(2)
        );

        specularColor = glm::vec3(
            j.at("specularColor").at(0),
            j.at("specularColor").at(1),
            j.at("specularColor").at(2)
        );

        shininess = j.at("shininess").get<float>();

        ambientMapUUID = UUID::fromString(j.at("ambientMap").get<std::string>());
        diffuseMapUUID = UUID::fromString(j.at("diffuseMap").get<std::string>());
        specularMapUUID = UUID::fromString(j.at("specularMap").get<std::string>());
        normalMapUUID = UUID::fromString(j.at("normalMap").get<std::string>());

        return true;
    }
    catch (const nlohmann::json::exception&) {
        return false;
    }
}
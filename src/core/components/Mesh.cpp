#include "Mesh.hpp"

void MeshComponent::Serialize(nlohmann::json& json) {
    json["meshUUID"] = UUID::toString(meshUUID);
}

void MeshComponent::Deserialize(const nlohmann::json& json) {
	meshUUID = UUID::fromString(json.at("meshUUID").get<std::string>());
}

MeshComponent::~MeshComponent() = default;

#include "Mesh.hpp"

inline void MeshComponent::Serialize(nlohmann::json& json) {
    json["meshUUID"] = UUID::toString(meshUUID);
}

inline void MeshComponent::Deserialize(const nlohmann::json& json) {
	meshUUID = UUID::fromString(json.at("meshUUID").get<std::string>());
}

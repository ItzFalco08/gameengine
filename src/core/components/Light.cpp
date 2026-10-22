#include "Light.hpp"
#include "../SceneManager.hpp"

void LightComponent::Serialize(nlohmann::json& json)
{
	json["lightType"] = static_cast<int>(type);
	json["color"] = { color.r, color.g, color.b };
	json["intensity"] = intensity;
	json["range"] = range;
}

void LightComponent::Deserialize(const nlohmann::json& json)
{
	// Older scene files stored the light kind in "type", overwriting the
	// component kind. Preserve those scenes while writing the unambiguous key.
	const auto lightType = json.contains("lightType") ? json["lightType"].get<int>() : json["type"].get<int>();
	type = static_cast<LightType>(lightType);
	color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	intensity = json["intensity"].get<float>();
	range = json["range"].get<float>();
}

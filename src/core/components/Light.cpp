#include "Light.hpp"
#include "../SceneManager.hpp"

extern SceneManager sceneManager;

void LightComponent::Serialize(nlohmann::json& json)
{
	json["type"] = static_cast<int>(type);
	json["color"] = { color.r, color.g, color.b };
	json["intensity"] = intensity;
	json["range"] = range;
}

void LightComponent::Deserialize(const nlohmann::json& json)
{
	type = static_cast<LightType>(json["type"].get<int>());
	color = glm::vec3(json["color"][0].get<float>(), json["color"][1].get<float>(), json["color"][2].get<float>());
	intensity = json["intensity"].get<float>();
	range = json["range"].get<float>();
}

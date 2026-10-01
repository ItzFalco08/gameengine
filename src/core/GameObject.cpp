#include "GameObject.hpp"
#include "components/Light.hpp"
#include "components/Mesh.hpp"

inline void GameObject::Serialize(nlohmann::json& json) {
    json["name"] = name;

    nlohmann::json transformJson;
    nlohmann::json componentsJson = nlohmann::json::array();

    transform->Serialize(transformJson);

    for (auto& [name, component] : components) {
        nlohmann::json componentJson;
        componentJson["type"] = component->GetType();
        component->Serialize(componentJson);
        componentsJson.push_back(std::move(componentJson));
    }

    json["transform"] = transformJson;
    json["components"] = componentsJson;
}

inline void GameObject::Deserialize(nlohmann::json& json) {
    transform = std::make_unique<TransformComponent>();
    transform->Deserialize(json["transform"]);

    struct ComponentInfo {
        std::type_index typeidx;
        std::function<std::unique_ptr<Component>()> create;
    };

    std::unordered_map<ComponentType, ComponentInfo> comp_info = {
        { ComponentType::MeshRenderer,{ std::type_index(typeid(MeshComponent)), []() { return std::make_unique<MeshComponent>(); } } },
        { ComponentType::Light,{ std::type_index(typeid(LightComponent)), []() { return std::make_unique<LightComponent>(); } } },
    };

    for (auto& componentJson : json["components"]) {
        ComponentType type = componentJson.at("type").get<ComponentType>();

        auto it = comp_info.find(type);
        if (it == comp_info.end())
            continue; // or throw

        auto& info = it->second;

        auto c = info.create();
        c->Deserialize(componentJson);
        c->parent = this;

        components[info.typeidx] = std::move(c);
    }
}

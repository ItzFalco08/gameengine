#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "../utils/Logger.hpp"
#include "memory"
#include "vector"
#include <typeindex>
#include <functional>
#include <unordered_map>
#include "components/Transform.hpp"
#include "components/Component.hpp"
#include "json/json.hpp"

class Scene; // forward declaration 

struct ComponentRegistryEntry {
    std::string name;
    std::unique_ptr<Component> (*create)();
};

// Factory map defined in a .cpp to avoid include cycles
extern std::map<std::string, std::function<std::unique_ptr<Component>()>> componentRegistry;

class GameObject {
public:
    std::string name;
    std::unique_ptr<Transform> transform = std::make_unique<Transform>();
    std::map<std::string, std::unique_ptr<Component>> components;
    Scene* parentScene;
    GameObject* parent;
    std::vector<GameObject*> childs;

    GameObject(Scene* parentScene, const std::string& goName) 
    : parentScene(parentScene)
    , name(goName) { };

    ~GameObject() {
        LOG::Debug("GameObject Deleted: ", name);
    }
    
    template<typename T, typename... Args>
    void AddComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        const char* compTypeStr = T::StaticType();

        if (components.find(compTypeStr) != components.end()) {
            LOG::Error("AddComponent Failed! Component Already Exists.");
            return;
        }
        components[compTypeStr] = std::make_unique<T>(std::forward<Args>(args)...);
        components[compTypeStr]->parent = this;
    }

    template<typename T>
    void RemoveComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        const char* typeStr = T::StaticType();
        if(components.find(typeStr) != components.end()) {
            components.erase(typeStr);
            return;
        }

        LOG::Error("No Component found of type: ", typeStr);
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        const char* key = T::StaticType();

        auto comp = components.find(key);
        if (comp != components.end()) {
            return static_cast<T*>(comp->second.get());
        }

        return nullptr;
    }

    template<typename T>
    bool hasComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from 'Component'");

        const char* key = T::StaticType();
        return components.find(key) != components.end();
    }
    
    // Game Object -> JSON 
    void Serialize(nlohmann::json& json) {
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
    
    void Deserialize(nlohmann::json& json) {        
        transform = std::make_unique<Transform>();
        transform->Deserialize(json["transform"]);

        for(nlohmann::json& componentJson : json["components"]) {
            std::unique_ptr<Component> c = componentRegistry[componentJson["type"]]();
            c->Deserialize(componentJson);
            c->parent = this;
            components[componentJson.at("type")] = std::move(c);
        }
    }
};


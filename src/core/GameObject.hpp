#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define TypeToStr(x) #x
#include "glm/glm.hpp"
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
extern std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> componentRegistry;

class GameObject {
public:
    std::string name;
    std::unique_ptr<Transform> transform;
    std::unordered_map<std::string, std::unique_ptr<Component>> components;
    Scene* parentScene;
    std::vector<GameObject*> childs;

    GameObject(Scene* parentScene, const std::string& goName) 
    : transform(std::make_unique<Transform>())
    , parentScene(parentScene)
    , name(goName)
    {
        components.reserve(4);
    }

    ~GameObject() {
        LOG::Debug("GameObject Deleted: ", name);
    }
    
    template<typename T, typename... Args>
    void AddComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        char* compTypeStr = TypeToStr(T);

        if (components.find(compTypeStr) != components.end()) {
            LOG::Error("AddComponent Failed! Component Already Exists.");
            return;
        }

        components[compTypeStr] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        const char* typeStr = TypeToStr(T);
        if(components.find(typeStr) != components.end()) {
            components.erase(typeStr);
            return;
        }

        LOG::Error("No Component found of type: ", typeStr);
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        char* key = TypeToStr(T);

        auto& comp = components.find(key);
        if (comp != components.end()) {
            return static_cast<T*>((*comp).second.get());
        }

        return nullptr;
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
            components[componentJson.at("name")] = std::move(c);
        }
    }
};


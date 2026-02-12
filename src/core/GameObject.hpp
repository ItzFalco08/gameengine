#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "../utils/Logger.hpp"
#include "memory"
#include "vector"
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include "components/Transform.hpp"
#include "components/Component.hpp"
#include "json/json.hpp"

class Scene; // forward declaration 

// Factory map defined in a .cpp to avoid include cycles
extern std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> TypeToComponent;
class GameObject {
public:
    std::string name;
    std::unique_ptr<Transform> transform;
    std::vector<std::unique_ptr<Component>> components;
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

        size_t compId = typeid(T).hash_code();

        for (const auto& c : components) {
            if (compId == c->GetId()) {
                LOG::Error("AddComponent Failed! Component Already Exists.");
                return;
            }
        }

        components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template<typename T>
    void RemoveComponent() {
        size_t compId = typeid(T).hash_code();

        for (auto itr = components.begin(); itr != components.end(); ++itr) {
            if ((*itr)->GetId() == compId) {
                components.erase(itr);
                LOG::Success("Component Deleted of type: ", typeid(T).name());
                return;
            }
        }

        LOG::Error("No Component found of type: ", typeid(T).name());
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        size_t compId = typeid(T).hash_code();

        for (const auto& c : components) {
            if (c->GetId() == compId) {
                return static_cast<T*>(c.get());
            }
        }

        return nullptr;
    }
    
    // Game Object -> JSON 
    void Serialize(nlohmann::json& json) {
        json["name"] = name;
       
        nlohmann::json transformJson;
        nlohmann::json componentsJson = nlohmann::json::array();

        transform->Serialize(transformJson);

        for (std::unique_ptr<Component>& component : components) {
            nlohmann::json componentJson;
            componentJson["type"] = component->GetType();
            component->Serialize(componentJson);
            componentsJson.push_back(std::move(componentJson));
        }

        json["transform"] = transformJson;
        json["components"] = componentsJson;
    }
    
    void Deserialize(nlohmann::json& json) {
        name = json.at("name");
        
        transform = std::make_unique<Transform>();
        transform->Deserialize(json["transform"]);

        for(nlohmann::json& component : json["components"]) {
            std::unique_ptr<Component> c = TypeToComponent[component["type"]]();
            c->Deserialize(component);
            components.push_back(std::move(c));
        }
    }
};


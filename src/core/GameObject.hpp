#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "../utils/Logger.hpp"
#include "memory"
#include "vector"
#include <typeinfo>
#include "components/Transform.hpp"
#include "components/Material.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "components/Component.hpp"
#include "json/json.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"

class GameObject {
public:
    std::string name;
    std::unique_ptr<Transform> transform;
    std::vector<std::unique_ptr<Component>> components;

    GameObject() : transform(std::make_unique<Transform>()) {
        components.reserve(4);
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
    
    void Serialize(nlohmann::json& json) {
        SerializerJSON serializer(json);

        serializer.Write("name", name);
        serializer.Write("gameObjects")
    }
};
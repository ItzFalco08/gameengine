#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "../utils/Logger.hpp"
#include "memory"
#include "vector"
#include <typeindex>
#include <unordered_map>
#include "components/Transform.hpp"
#include "components/Component.hpp"
#include "json/json.hpp"

class Scene;

class GameObject {
public:
    std::string name;
    std::unique_ptr<TransformComponent> transform = std::make_unique<TransformComponent>();
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    Scene* parentScene;
    GameObject* parent;
    std::vector<GameObject*> childs;

    GameObject(Scene* parentScene, const std::string& goName)  : parentScene(parentScene) , name(goName) { };

    template<typename T, typename... Args>
    void AddComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        if (components.find(std::type_index(typeid(T))) != components.end()) {
            LOG::Error("AddComponent Failed! Component Already Exists.");
            return;
        }

        components[std::type_index(typeid(T))] = std::make_unique<T>(std::forward<Args>(args)...);
        components[std::type_index(typeid(T))]->parent = this;
    }

    template<typename T>
    void RemoveComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        if constexpr (std::is_same_v<T, TransformComponent>)
            return components.erase(std::type_index(typeid(T)));
        else
			components.erase(std::type_index(typeid(T)));
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        if constexpr (std::is_same_v<T, TransformComponent>)
            return &transform;
        else
            return static_cast<const T*>(components[std::type_index(typeid(T))].get());

        return nullptr;
    }

    template<typename T>
    bool hasComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from 'Component'");

        return components.find(std::type_index(typeid(T))) != components.end();
    }
    
    void Serialize(nlohmann::json& json);
    void Deserialize(nlohmann::json& json);
};


#include "glm/glm.hpp"
#include "../utils/Logger.hpp"
#include "memory"
#include "vector"
#include "components/Transform.hpp"
#include "components/Material.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "components/Component.hpp"

class GameObject {
public:
    std::unique_ptr<Transform> transform;
    std::vector<std::unique_ptr<Component>> components;

    GameObject() : transform(std::make_unique<Transform>()) {
        components.reserve(4);
    };

    // gameObject.AddComponent<Mesh>(int a);
    template<typename T, typename... Args>
    void AddComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        for(const auto& c : components) {
            T* ptr = dynamic_cast<T*>(c.get());
            if(ptr) {
                LOG::Error("AddComponent Failed! Component Already Exists.");
                return; // Don't add duplicate component
            }
        }

        components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }


    template <typename T>
    void RemoveComponent() {
        for(auto itr = components.begin(); itr != components.end(); ++itr) {
            T* ptr = dynamic_cast<T*>(itr->get());
            if(ptr){
                components.erase(itr);
                return;
            }
        }

        LOG::Error("No Component found of type: ", typeid(T).name());
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        for (const auto& c : components ){
            T* ptr = dynamic_cast<T*>(c.get());
            if(ptr) {
                return ptr;                 
            }
        }

        // if component dont exists
        return nullptr;
    }
};
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstddef>
#include "../../utils/Logger.hpp"
#include "json/json.hpp"

class GameObject; // forward declaration to avoid circular include

class Component {
public:
    virtual ~Component()  = default;
    GameObject* owner = nullptr;
    virtual std::string GetType() = 0;
    virtual void Serialize(nlohmann::json& json) {};
    virtual void Deserialize(nlohmann::json& json) {};

};
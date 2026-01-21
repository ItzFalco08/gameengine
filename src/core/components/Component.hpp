#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstddef>
#include "../../utils/Logger.hpp"
#include "json/json.hpp"

class GameObject; // forward declaration to avoid circular include

class Component {
public:
    virtual ~Component() noexcept = default;
    GameObject* owner = nullptr;
    virtual size_t GetId() { return 0; }
    virtual std::string GetType() {return "";};
    virtual void Serialize(nlohmann::json& json) {};
    virtual void Deserialize(nlohmann::json& json) {};

};
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstddef>
#include "../../utils/Logger.hpp"
#include "json/json.hpp"

class GameObject;

enum class ComponentType {
	Transform,
	MeshRenderer,
	Camera,
	Light,
	Script
};

class Component {
public:
    virtual ~Component()  = default;
    GameObject* parent = nullptr;
    virtual ComponentType GetType() = 0;
    virtual void Serialize(nlohmann::json& json)=0;
    virtual void Deserialize(const nlohmann::json& json)=0;
};
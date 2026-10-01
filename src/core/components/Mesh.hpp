#pragma once
#include "Component.hpp"
#include "../../utils/Utils.hpp"
#include "json/json.hpp"

class MeshComponent : public Component {
public:
    UUID meshUUID = 0;

    void Serialize(nlohmann::json& json) override;
    void Deserialize(const nlohmann::json& json) override;

	ComponentType GetType() override { return ComponentType::MeshRenderer; }

    ~MeshComponent() override;
};
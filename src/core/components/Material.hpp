#pragma once
#include "Component.hpp"
#include "../../utils/globals.hpp"
#include "../Texture.hpp"
#include <memory>

enum class MaterialType {
    LIT,
    UNLIT
};

struct MaterialProps {
    virtual ~MaterialProps() = default;
    virtual void Serialize(nlohmann::json& json) {};
    virtual void Deserialize(nlohmann::json& json) {};
    virtual MaterialType GetMatType() = 0;
};

struct LitMaterial : public MaterialProps {

    glm::vec3 ambientColor = glm::vec3(1.0f,1.0f,1.0f);
    float ambientStrength = 0.2f;
    float diffuseStrength = 0.4f;
    float specularStrength = 0.4f;
    float shininess = 32.0f;

    std::string texturePath = "";

    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;

    void applyToShader(Shader* shader);

    MaterialType GetMatType() override;
};

struct UnlitMaterial : public MaterialProps {
    glm::vec3 ambientColor = glm::vec3(1.0f,1.0f,1.0f);
    
    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;

    void applyToShader(Shader* shader);

    MaterialType GetMatType() override;
};

class Material : public Component {
public:
    std::unique_ptr<MaterialProps> matprops;

    Material();

    void Serialize(nlohmann::json& json) override;
    void Deserialize(nlohmann::json& json) override;
    void SetMaterialType(MaterialType matType);

    std::string GetType() override;
};
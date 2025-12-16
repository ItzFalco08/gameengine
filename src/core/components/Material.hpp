#pragma once
#include "Component.hpp"
#include "../../utils/globals.hpp"
#include "../Texture.hpp"

class Material : public Component {
public:
    Shader* shader = &litShader;
    Texture* texture = nullptr;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientStrength  = 0.1f;
    float diffuseStrength  = 1.0f;
    float specularStrength = 0.5f;
    float shininess        = 32.0f;

    size_t GetId() override;
    ~Material() noexcept override;
    Material();
};
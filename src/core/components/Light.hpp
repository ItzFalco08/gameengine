#pragma once
#include "Component.hpp"
#include "glm/glm.hpp"

enum LightType {
    DIRECTIONAL,
    POINT
};

class Light : public Component {
public:
    LightType lightType = LightType::DIRECTIONAL;
    glm::vec3 lightColor = {1, 1, 1};
    int range = 30;

    // attenution
    float constant = 1.0f;
    float linear =  0.07f;
    float quadratic = 0.017f;
    
    size_t GetId() override;
    ~Light() noexcept override;
};

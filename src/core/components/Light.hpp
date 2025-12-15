#pragma once
#include "Component.hpp"

enum LightType {
    DIRECTIONAL,
    POINT
};

class Light : public Component{
public:
    LightType lightType = LightType::DIRECTIONAL;
    
    size_t GetId() override {
        return typeid(Light).hash_code();
    }

    ~Light() noexcept override {
        LOG::Info("Light Destroyed");
    };

    Light() {
        LOG::Info("Light Created");
    }
};
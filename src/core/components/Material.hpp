#pragma once
#include "Component.hpp"

class Material : public Component{
public:
    size_t GetId() override {
        return typeid(Material).hash_code();
    }

    ~Material() noexcept override {
        LOG::Info("Material Destroyed");
    };

    Material() {
        LOG::Info("Material Created");
    }
};
#pragma once
#include "../GameObject.hpp"

class Component {
    virtual ~Component() = default;
public:
    GameObject* owner = nullptr;
    virtual size_t GetId() {
        return 0;
    };

};
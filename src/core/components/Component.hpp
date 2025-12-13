#pragma once
#include "../GameObject.hpp"

class Component {
    virtual ~Component() = default;
public:
    GameObject* owner = nullptr;
};
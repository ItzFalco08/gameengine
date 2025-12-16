#include "Light.hpp"
#include "../utils/Logger.hpp"
#include <typeinfo>

size_t Light::GetId() {
    return typeid(Light).hash_code();
}

Light::~Light() noexcept {
    LOG::Info("Light Destroyed");
}

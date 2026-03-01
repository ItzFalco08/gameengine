#include "Light.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

Light::~Light() noexcept {
    LOG::Info("Light Destroyed");
}

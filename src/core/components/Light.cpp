#include "Light.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

Light::~Light() {
    LOG::Info("Light Destroyed");
}
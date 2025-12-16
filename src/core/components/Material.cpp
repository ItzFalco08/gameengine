#include "Material.hpp"
#include "../../utils/Logger.hpp"
#include <typeinfo>

size_t Material::GetId() {
    return typeid(Material).hash_code();
}

Material::~Material() noexcept {
    LOG::Info("Material Destroyed");
}

Material::Material() {
    LOG::Info("Material Created");
}

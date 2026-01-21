#include "GameObject.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "components/Material.hpp"

// Concrete factory map; placed in a .cpp to ensure full definitions are available
std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> TypeToComponent = {
    { "Mesh",     [] { return std::make_unique<Mesh>(); } },
    { "Light",    [] { return std::make_unique<Light>(); } },
    { "Material", [] { return std::make_unique<Material>(); } },
};

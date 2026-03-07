#include "GameObject.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "components/Material.hpp"

// Concrete factory map; placed in a .cpp to ensure full definitions are available
std::map<std::string, std::function<std::unique_ptr<Component>()>> componentRegistry = {
    { "Mesh",     [] { return std::make_unique<Mesh>(); } },
    { "Light",    [] { return std::make_unique<Light>(); } },
    { "Material", [] { return std::make_unique<Material>(); } },
};

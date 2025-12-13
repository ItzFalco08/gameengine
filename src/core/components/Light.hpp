#include "Component.hpp"

enum LightType {
    DIRECTIONAL,
    POINT
};

class Light : public Component{
    LightType lightType = LightType::DIRECTIONAL;
};
#include "Component.hpp"

class Material : public Component{
public:
    size_t GetId() override {
        return typeid(Material).hash_code();
    }
};
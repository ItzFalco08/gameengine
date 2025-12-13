#include "Component.hpp"

class Mesh : public Component {
public:
    size_t GetId() override {
        return typeid(Mesh).hash_code();
    }
};
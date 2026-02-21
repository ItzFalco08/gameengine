#include <iostream>
#include <typeindex>

class Something {
    int _int;
};

int main() {
    std::type_index a = typeid(Something);
    std::cout << std::string(a.name());
    return 0;
}
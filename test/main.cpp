#include "iostream"
#define print(x) std::cout << x << std::endl

class A { 

};

class B {

};

int main() {
    const std::type_info& typeInfoA = typeid(A);
    const std::type_info& typeInfoB = typeid(B);
    print(typeInfoA.raw_name());
    print(typeInfoB.name());
    return 0;
}
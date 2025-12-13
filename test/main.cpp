#include "iostream" 
#include <typeinfo> // Required header

class A {
public:
    A() = default;
    A (int&& a) {
        data = &a;
    };


private:
    int* data = nullptr; 
};


class Bakwas {

};

int main() {
    int a = 10;
    int b = 20;

    const std::type_info& typeInfoA = typeid(a);
    const std::type_info& typeInfoB = typeid(b);
    

    std::cout << typeInfoA.name() << ", " << typeInfoB.name() << std::endl;
    std::cout << std::endl;
    std::cout << sizeof(typeInfoA.hash_code()) << ", " << typeInfoB.hash_code() << std::endl;
    return 0;
}
#include "iostream"
#include "vector"
#include "memory"

struct Component {

};

class GameObject { 
    std::vector<std::unique_ptr<Component>> components;
};

int main() {
    std::vector<int> v1;
    std::vector<GameObject> v2;
    std::cout << "Size of std::vector<int>: " << sizeof(v1) << "size of std::vector<GameObject>: " << sizeof(v2) << std::endl;
    return 0;
}
#include "iostream"

template<typename... Ts>
void func(Ts... args) {
    ((std::cout << "Type: " << typeid(Ts).name() << ", Value: " << args << "\n"), ...);
}

int main() {
      func<int,float>(1, 2.5, "aaa"); 
}
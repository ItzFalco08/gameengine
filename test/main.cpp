#include <iostream>
// L A M B D A and S T D :: F U N C T I O N

int main() {
    auto lambda = []() {
        std::cout << "Gay" << std::endl;
    };
    lambda();
    return 0;
}
#include <iostream>
#include <cstring>
#define print(x) std::cout << x << std::endl;

class String {
public:
    String(): data(nullptr), length(0) {};

    String(const char* str) {
        print("Constructor");
        length = strlen(str);
        data = new char[length];
        memcpy(data, str, length + 1);
    }

    // copy constructor
    String(const String& other) {
        print("Copy constructor");
        length = other.length;
        data = new char[length];
        memcpy(data, other.data, length + 1);
    };

    // move constructor
    String(String&& other) {
        print("Move Constructor");
        length = other.length;
        data = new char[length];
        memcpy(data, other.data, length + 1);
        other.data = nullptr;
        other.length = 0;
    }

    // copy assignment operator
    void operator=(const String& other) {
        print("Copy");
        delete[] data;
        length = other.length;
        data = new char[length];
        memcpy(data, other.data, length + 1);
    };

    // move assignment operator
    void operator=(String&& other) {
        print("Move");
        data = other.data;
        length = other.length;

        other.data = nullptr;
        other.length = 0;
    }

    // raw pointer
    char* get() {
        static char null[] = "";
        return data ? data : null;
    };

    ~String() {
        print("Delete");
        delete[] data;
    }

    char* data;
    int length;
};

int main() {
    return 0;
}
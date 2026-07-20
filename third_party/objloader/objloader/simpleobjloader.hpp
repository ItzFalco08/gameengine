#pragma once
#include <vector>

struct vec2 {
    float x = 0, y = 0;

    vec2() = default;  
    vec2(float x, float y): x(x), y(y) {};
};

struct vec3 {
    float x = 0, y = 0, z = 0;
    
    vec3() = default;
    vec3(float x, float y, float z) : x(x), y(y), z(z) { };
};

struct Vertex {
    vec3 v;
    vec3 vn;
    vec2 vt;
};

struct Key {
    long v, vt, vn;
    
    bool operator==(const Key& other) const {
        return v == other.v && vt == other.vt && vn == other.vn;
    }
};

void loadObj(const char* objPath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
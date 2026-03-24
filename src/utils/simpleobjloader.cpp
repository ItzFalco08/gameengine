#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <charconv>
#include "objloader/simpleobjloader.hpp"
#define _CRT_SECURE_NO_WARNINGS

struct KeyHash {
    size_t operator()(const Key& k) const {
        // A simple, fast hash combine (similar to boost::hash_combine)
        size_t h = std::hash<long>{}(k.v);
        h ^= std::hash<long>{}(k.vt) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<long>{}(k.vn) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

inline float fastParseFloat(char*& ptr, const char* end) {
    // additional check (space skip)
    while (ptr < end && *ptr == ' ') ptr++;
    float value = 0;
    auto res = std::from_chars(ptr, end, value);
    if (res.ec == std::errc()) {
        ptr = const_cast<char*>(res.ptr);
    }

    return value;
};


void loadObj(const char* objPath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    FILE* file = std::fopen(objPath, "rb");
    if(file == nullptr) return;
    // get size
    std::fseek(file, 0, SEEK_END);
    size_t size = std::ftell(file);
    std::rewind(file);
    // get buffer
    char* fileBuffer = (char*)malloc(size + 1);
    const char* end = fileBuffer + size;
    std::fread(fileBuffer, sizeof(char), size, file);
    fileBuffer[size] = '\0';

    char* ptr = fileBuffer;

    std::cout << "File Loaded into RAM" << std::endl;

    // some cache
    std::vector<vec3> vs;
    std::vector<vec3> vns;
    std::vector<vec2> vts;
    std::unordered_map<Key, unsigned int, KeyHash> cacheMap;

    // Rough estimate: assume 1 vertex/face per 50-100 bytes of file
    vs.reserve(size / 64);
    vns.reserve(size / 64);
    vts.reserve(size / 64);
    vertices.reserve(size / 32);
    indices.reserve(size / 16);
    cacheMap.reserve(size / 32);


    while(*ptr) {

        // 1. skip comment, skip space and handle new line
        if (*ptr == '#') while(*ptr && *ptr != '\n') ptr++;
        if (*ptr == ' ') while(*ptr == ' ') ptr++;
        if (*ptr == '\n') {
            ptr++;
            continue;
        };
        
        // 2. read line
        if (ptr[0] == 'v') {
            Vertex vertex;

            if(ptr[1] == 't') {
                // vt
                ptr += 2;

                vec2 vt = {fastParseFloat(ptr, end), fastParseFloat(ptr, end)};
                vts.push_back(vt);

            } else if (ptr[1] == 'n') {
                // vn
                ptr += 2;

                // parse floats
                vec3 vn = {fastParseFloat(ptr, end), fastParseFloat(ptr, end), fastParseFloat(ptr, end)};
                vns.push_back(vn);
            } else {
                // v
                ptr += 2;

                // parse floats
                vec3 v = {fastParseFloat(ptr, end), fastParseFloat(ptr, end), fastParseFloat(ptr, end)};
                vs.push_back(v);
            }
        } else if(ptr[0] == 'f'){
            // face
            ptr++;
            // const char* startPos = ptr; // 1. Remember where we started
            int vCount = 0;
            
            while (*ptr && *ptr != '\n' && *ptr != '\r') {
                vCount++;

                // process v,vt,vn
                Key key = {0, 0, 0};

                key.v = strtol(ptr, &ptr, 10);

                if (*ptr == '/') {
                    ptr++;
                    key.vt = strtol(ptr, &ptr, 10);

                    if (*ptr == '/') {
                        ptr++;
                        key.vn = strtol(ptr, &ptr, 10);
                    }
                }

                // Process Vertex
                unsigned int idx = (unsigned int)vertices.size();
                auto [itr, inserted] = cacheMap.try_emplace(key, idx);
                
                if(inserted) {
                    // if doesnt exists, push to data (already added to cache)
                    Vertex vert;
                    vert.v = (key.v != 0) ? (key.v > 0 ? vs[key.v - 1] : vs[vs.size() + key.v]) : vec3(0,0,0);
                    vert.vt = (key.vt != 0) ? (key.vt > 0 ? vts[key.vt - 1] : vts[vts.size() + key.vt]) : vec2(0,0);
                    vert.vn = (key.vn != 0) ? (key.vn > 0 ? vns[key.vn - 1] : vns[vns.size() + key.vn]) : vec3(0,0,0);
                    
                    vertices.push_back(vert);
                    indices.push_back(idx);
                } else {
                    indices.push_back(itr->second);
                }

                // Triangularization
                if(vCount == 4) {
                    // rearange the indices;
                    size_t  lastIdx = indices.size() - 1;
                    unsigned int FourthVertex = indices[lastIdx];
                    indices[lastIdx - 1] = FourthVertex;
                    indices.push_back(indices[lastIdx - 3]);
                    indices.push_back(indices[lastIdx - 2]);
                    indices.push_back(FourthVertex);
                }

                while(*ptr == ' ') ptr++;
            }

        } else if(ptr[0] == 'm') {
            // mat
            while(*ptr && *ptr != '\n') ptr++;
        } else {
            // undefined 
            // printf("undefined obj entry: %c", *ptr);
            while(*ptr && *ptr != '\n') ptr++;
        }
    }

    std::cout << "Successfully reached end of buffer." << std::endl;
    std::cout << "Final Vertices: " << vertices.size() << std::endl;
    std::cout << "Final Indices: " << indices.size() << std::endl;
    
    free(fileBuffer);
    fclose(file);
}

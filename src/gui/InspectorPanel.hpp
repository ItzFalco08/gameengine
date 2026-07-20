#pragma once
#include "imgui.h"
#include "../core/Texture.hpp"

class InspectorPanel {
public:
    void Render();
    
private:
    int faceCullingIdx = 0;
    int shaderTypeIdx = 0;
    int lightTypeIdx = 0;
    TexDets dets{};

    int wrapSIdx;
    int wrapTIdx;
    int minFilterIdx;
    int magFilterIdx;

    int GetIdx(const unsigned int* arr, int size, unsigned int value);

};
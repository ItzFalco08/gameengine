#pragma once
#include "imgui/imgui.h"

class InspectorPanel {
public:
    void Render();
    
private:
    int faceCullingIdx = 0;
    int shaderTypeIdx = 0;
};
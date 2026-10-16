#pragma once
#include "../../core/assets/runtime/Texture.hpp"
#include <typeindex>

class InspectorPanel {
public:
    void Render();

private:
    void renderPanels();

    bool showDeleteComponentPopup = false;
    bool showAddComponentPopup = false;
    std::type_index typeToDelete{typeid(void)};

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

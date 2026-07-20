#pragma once
#include "imgui.h"
#include "../utils/globals.hpp"
#include "glfw/glfw3.h"
#include "../utils/Utils.hpp"
#include "ImGuizmo/ImGuizmo.h"

class ScenePanel {
public:
    void Render();
    void initTextures();

private:
    void updateDimentions(ImVec2& dimensions);
    void renderFrameBuffer();
    void gizmoSelectorGui(ImVec2& windowPos);
    void statsGui(ImVec2& windowPos, ImVec2& dimensions);
    void renderGuizmos();
    void handleCameraMovement();

    Texture moveTex;
    Texture rotateTex;
    Texture gizmoTex;
    Texture scaleTex;
    double cursorX = 0, cursorY = 0;
    // variables for scene panel class
    ImGuizmo::OPERATION gizmoState = ImGuizmo::TRANSLATE;
};
#pragma once
#include "../../core/assets/runtime/Texture.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include "imgui.h"

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
    void updateFBODimensions();

    Texture2D moveTex;
    Texture2D rotateTex;
    Texture2D gizmoTex;
    Texture2D scaleTex;
    double cursorX = 0, cursorY = 0;

    // variables for scene panel class
    float camera_sensitivity = 1.0f;
    float camera_speed = 10.0f;
    bool isFocused = false;
    bool isVSync = false;
    ImGuizmo::OPERATION gizmoState = ImGuizmo::TRANSLATE;
};
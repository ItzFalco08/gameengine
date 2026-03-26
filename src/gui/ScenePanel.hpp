#pragma once
#include "imgui/imgui.h"
#include "../utils/globals.hpp"
#include "glfw/glfw3.h"
#include "../utils/Utils.hpp"
#include "ImGuizmo/imGuizmo.h"

class ScenePanel {
public:
    void Render();
    void initTextures();

private:
    Texture moveTex;
    Texture rotateTex;
    Texture gizmoTex;
    Texture scaleTex;
    // variables for scene panel class
    ImGuizmo::OPERATION gizmoState = ImGuizmo::TRANSLATE;
};
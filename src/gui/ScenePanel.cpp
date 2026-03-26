#include "ScenePanel.hpp"
#include "ImGuizmo/imGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

void ScenePanel::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();

    // U P D A T E _ S C E N E _ D I M E N S I O N S
    int p_w = sceneView.SCENEVIEW_WIDTH;
    int p_h = sceneView.SCENEVIEW_HEIGHT;
    static double lastchange = 0;
    ImVec2 dimensions = ImGui::GetContentRegionAvail();

    bool changed = false;

    if (dimensions.x != p_w) {
        sceneView.SCENEVIEW_WIDTH = static_cast<int>(dimensions.x);
        changed = true;
    }
    if (dimensions.y != p_h) {
        sceneView.SCENEVIEW_HEIGHT = static_cast<int>(dimensions.y);
        changed = true;
    }

    if (changed) {
        lastchange = glfwGetTime();
        editorCamera->projDirty = true; // aspect ratio changed, recalculate projection
    }

    if (glfwGetTime() - lastchange > 0.15) { // 150ms debounce
        Utils::updateFBODimensions();
    }

    // NOTE: UV flipped vertically, OpenGL FBO is bottom-left origin, ImGui is top-left
    ImGui::Image((ImTextureID)(uintptr_t)sceneView.textureObj,
                 ImVec2((float)sceneView.SCENEVIEW_WIDTH, (float)sceneView.SCENEVIEW_HEIGHT),
                 ImVec2(0, 1), ImVec2(1, 0));
    
    
    //  G I Z M O _ S E L E C T O R
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 overlayPos = ImVec2(windowPos.x + 10, windowPos.y + 30); 
    ImGui::SetNextWindowPos(overlayPos, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));

    unsigned int gizmo_window_flags = ImGuiWindowFlags_NoDecoration | 
                                    ImGuiWindowFlags_NoDocking | 
                                    ImGuiWindowFlags_AlwaysAutoResize | 
                                    ImGuiWindowFlags_NoSavedSettings | 
                                    ImGuiWindowFlags_NoFocusOnAppearing | 
                                    ImGuiWindowFlags_NoNav |
                                    ImGuiWindowFlags_NoMove;

    ImVec4 activeColor  = ImVec4(0.16f, 0.16f, 0.16f, 1.0f); // highlight tint
    ImVec4 defaultColor = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); // transparent (normal)

    auto PushButtonState = [&](bool active) {
        ImVec4 col = active ? activeColor : defaultColor;
        ImGui::PushStyleColor(ImGuiCol_Button,        col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, active ? activeColor : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  activeColor);
    };

    ImGui::Begin("##Toolbar", nullptr, gizmo_window_flags);

        PushButtonState(gizmoState == ImGuizmo::TRANSLATE);
        if (ImGui::ImageButton("##T", (ImTextureID)moveTex.TexId, ImVec2(20,20), ImVec2(0,1), ImVec2(1,0))) gizmoState = ImGuizmo::TRANSLATE;
        ImGui::PopStyleColor(3);
        ImGui::Separator();

        PushButtonState(gizmoState == ImGuizmo::ROTATE);
        if (ImGui::ImageButton("##R", (ImTextureID)rotateTex.TexId, ImVec2(20,20), ImVec2(0,1), ImVec2(1,0))) gizmoState = ImGuizmo::ROTATE;
        ImGui::PopStyleColor(3);
        ImGui::Separator();

        PushButtonState(gizmoState == ImGuizmo::SCALE);
        if (ImGui::ImageButton("##S", (ImTextureID)scaleTex.TexId, ImVec2(20,20), ImVec2(0,1), ImVec2(1,0))) gizmoState = ImGuizmo::SCALE;
        ImGui::PopStyleColor(3);

        ImGui::Separator();

        PushButtonState(gizmoState == ImGuizmo::UNIVERSAL);
        if (ImGui::ImageButton("##G", (ImTextureID)gizmoTex.TexId, ImVec2(20,20), ImVec2(0,1), ImVec2(1,0))) gizmoState = ImGuizmo::UNIVERSAL;
        ImGui::PopStyleColor(3);

    ImGui::PopStyleColor(); // WindowBg
    ImGui::End();

    // S T A T S
    ImVec2 pos = ImVec2(windowPos.x + dimensions.x - 60, windowPos.y + 20);
    ImGui::SetNextWindowPos(pos);
    ImGui::Begin("##Stats", nullptr, gizmo_window_flags | ImGuiWindowFlags_NoBackground );
    if(ImGui::Button("Stats")) {
        ImGui::OpenPopup("StatsPanel");
    }

    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + 30.0f));
    if(ImGui::BeginPopup("StatsPanel", gizmo_window_flags)) {
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::Text("frame time: %fms", ImGui::GetIO().Framerate / 1000.0f);
        ImGui::EndChild();
    };

    ImGui::End();

    ImGuizmo::SetDrawlist();
    // get image rect (IMPORTANT)
    ImVec2 imagePos = ImGui::GetItemRectMin();
    ImVec2 imageSize = ImGui::GetItemRectSize();

    ImGuizmo::SetRect(
        imagePos.x,
        imagePos.y,
        imageSize.x,
        imageSize.y
    );

    // render Gizmos;
    ImGuizmo::Enable(true);
    if (selectedGameObject) {
        ImGuizmo::Manipulate(
            glm::value_ptr(editorCamera->viewMat),
            glm::value_ptr(editorCamera->projectionMat),
            gizmoState,   // later: switch modes
            ImGuizmo::LOCAL,
            const_cast<float*>(glm::value_ptr(selectedGameObject->transform->getModel()))
        );

        if (ImGuizmo::IsUsing()) {
            selectedGameObject->transform->DecomposeModel();
        }
    }
    ImGui::End();
}


void ScenePanel::initTextures() {
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    moveTex = Texture((rootDir + "/src/textures/move.png").c_str(), texDets);
    rotateTex = Texture((rootDir + "/src/textures/rotate.png").c_str(), texDets);
    scaleTex = Texture((rootDir + "/src/textures/scale.png").c_str(), texDets);
    gizmoTex = Texture((rootDir + "/src/textures/gizmo.png").c_str(), texDets);
}
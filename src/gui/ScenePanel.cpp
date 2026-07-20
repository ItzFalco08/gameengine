#include "ScenePanel.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

extern GLFWwindow* gMainWindow;

void ScenePanel::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();

    // variables
    ImVec2 dimensions = ImGui::GetContentRegionAvail();
    ImVec2 windowPos = ImGui::GetWindowPos();

    updateDimentions(dimensions);
    renderFrameBuffer();
    gizmoSelectorGui(windowPos);
    statsGui(windowPos, dimensions);
    renderGuizmos();
    handleCameraMovement();

    ImGui::End();
}

void ScenePanel::updateDimentions(ImVec2& dimensions) {
 int p_w = sceneView.SCENEVIEW_WIDTH;
    int p_h = sceneView.SCENEVIEW_HEIGHT;
    static double lastchange = 0;

    bool changed = false;
    //[02614] [imgui - error] In window 'Scene': Incorrect parameter.Did you swap 'thickness' and 'flags' ?
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
}

void ScenePanel::renderFrameBuffer() {
        ImGui::Image((ImTextureID)(uintptr_t)sceneView.textureObj,
            ImVec2((float)sceneView.SCENEVIEW_WIDTH, (float)sceneView.SCENEVIEW_HEIGHT),
            ImVec2(0, 1), ImVec2(1, 0));
}

void ScenePanel::gizmoSelectorGui(ImVec2& windowPos) {
    ImVec2 overlayPos = ImVec2(windowPos.x + 10, windowPos.y + 30); 
    ImGui::SetNextWindowPos(overlayPos, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));

    ImVec4 activeColor  = ImVec4(0.16f, 0.16f, 0.16f, 1.0f); // highlight tint
    ImVec4 defaultColor = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); // transparent (normal)

    auto PushButtonState = [&](bool active) {
        ImVec4 col = active ? activeColor : defaultColor;
        ImGui::PushStyleColor(ImGuiCol_Button,        col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, active ? activeColor : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  activeColor);
    };

    ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration |  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

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
}

void ScenePanel::statsGui(ImVec2& windowPos, ImVec2& dimensions) {
    // S T A T S
    ImVec2 pos = ImVec2(windowPos.x + dimensions.x - 60, windowPos.y + 20);
    ImGui::SetNextWindowPos(pos);
    ImGui::Begin("##Stats", nullptr, ImGuiWindowFlags_NoDecoration |  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground );
    if(ImGui::Button("Stats")) {
        ImGui::OpenPopup("StatsPanel");
    }

    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + 30.0f));
    if(ImGui::BeginPopup("StatsPanel", ImGuiWindowFlags_NoDecoration |  ImGuiWindowFlags_NoMove)) {
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::Text("frame time: %fms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::EndPopup();
    };

    ImGui::End();
}

void ScenePanel::renderGuizmos() {
    
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
            sceneManager.activeScene->MakeDirty();
        }
    }
};

void ScenePanel::handleCameraMovement() {
    double xpos, ypos = 0;
    glfwGetCursorPos(gMainWindow, &xpos, &ypos); // new pos
    
    if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(1)) {
        double dx = xpos - cursorX; // rotation around y (local/camera)
        double dy = ypos - cursorY; // rotation around x (local/camera)

        std::cout << dx << ", " << dy << std::endl;
    };

    // update cursor position
    cursorX = xpos;
    cursorY = ypos;
}

void ScenePanel::initTextures() {
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    moveTex = Texture((rootDir + "/src/textures/move.png").c_str(), texDets);
    rotateTex = Texture((rootDir + "/src/textures/rotate.png").c_str(), texDets);
    scaleTex = Texture((rootDir + "/src/textures/scale.png").c_str(), texDets);
    gizmoTex = Texture((rootDir + "/src/textures/gizmo.png").c_str(), texDets);
}
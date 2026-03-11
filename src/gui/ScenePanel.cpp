#include "ScenePanel.hpp"

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

    // NOTE: UV flipped vertically — OpenGL FBO is bottom-left origin, ImGui is top-left
    ImGui::Image((ImTextureID)(uintptr_t)sceneView.textureObj,
                 ImVec2((float)sceneView.SCENEVIEW_WIDTH, (float)sceneView.SCENEVIEW_HEIGHT),
                 ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

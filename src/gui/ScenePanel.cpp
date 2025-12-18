#include "ScenePanel.hpp"

void ScenePanel::Render() {
    ImGui::Begin("Scene");

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

    if (changed)
        lastchange = glfwGetTime();

    if (glfwGetTime() - lastchange > 0.15) { // 150ms debounce
        Utils::updateFBODimensions();
    }

    ImGui::Image((ImTextureID)(uintptr_t)sceneView.textureObj,
                 ImVec2((float)sceneView.SCENEVIEW_WIDTH, (float)sceneView.SCENEVIEW_HEIGHT),
                 ImVec2(0, 0), ImVec2(1, 1));

    ImGui::End();
}

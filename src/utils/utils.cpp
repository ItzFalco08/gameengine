#include "imgui.h"

void centerWindow(){
    ImGui::SetNextWindowPos(
        ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Appearing,
        ImVec2(0.5f, 0.5f)
    );
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
}
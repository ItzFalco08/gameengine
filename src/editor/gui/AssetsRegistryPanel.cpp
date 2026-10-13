#include "AssetsRegistryPanel.hpp"
#include "imgui.h"
#include "../../utils/globals.hpp"
#include "../../utils/Logger.hpp"

void AssetsRegistryPanel::Render() {
    if (openRequested) {
        ImGui::OpenPopup("AssetsRegistry");
        openRequested = false;
    }

    if (ImGui::BeginPopupModal("AssetsRegistry", &active)) {
        // Render the assets registry content
        if (ImGui::BeginTable(
            "AssetsTable",
            3,
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_ScrollX | 
            ImGuiTableFlags_SizingFixedFit,
            ImVec2(0, 300)))
        {
            // Column sizing
            ImGui::TableSetupColumn("UUID", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed, 200.0f);

            ImGui::TableHeadersRow();

            for (auto& [uuid, runtimeIdx] : engine.assetsRegistry.m_assetsMeta) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text(UUID::toString(uuid).c_str());

                ImGui::TableSetColumnIndex(1);

                AssetMeta* asset = engine.assetsRegistry.getAsset<AssetMeta>(uuid);

                if (asset)
                switch (asset->getType()) {
                case AssetType::MESH:
                    ImGui::Text("Mesh");
                    break;
                case AssetType::MATERIAL:
                    ImGui::Text("Material");
                    break;
                case AssetType::TEXTURE:
                    ImGui::Text("Texture");
                    break;
                default:
                    ImGui::Text("Unknown");
                    continue;
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::Text(asset->m_path.string().c_str());
            };

            ImGui::EndTable();
        }

        ImGui::EndPopup();
    }
}

void AssetsRegistryPanel::Activate() {
    active = true;
    openRequested = true;
}

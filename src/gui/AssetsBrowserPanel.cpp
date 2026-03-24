#include "AssetsBrowserPanel.hpp"
#include "../utils/globals.hpp"
#include "../utils/Utils.hpp"
#include <vector>
#include <functional>
#include "../utils/Logger.hpp"
#include <windows.h>
#include <shlobj.h>
#include <cstring>
#include "../utils/WinMsg.hpp"

void AssetsBrowser::InitIcons() {
    currentPath = rootDir + "/Assets";
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    fileTex = Texture((rootDir + "/src/textures/File.png").c_str(), texDets);
    folderTex = Texture((rootDir + "/src/textures/Folder.png").c_str(), texDets);
    folderEmptyTex = Texture((rootDir + "/src/textures/FolderEmpty.png").c_str(), texDets);
}

void AssetsBrowser::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("AssetManager");
    ImGui::PopStyleVar();
    
    // Custom toolbar panel at top
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
    
    float toolbarHeight = 28;

        ImGui::BeginChild("##toolbar", ImVec2(0, toolbarHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        
        
        if(currentPath.string() != (rootDir + "/Assets")) {
            if(ImGui::Button("<-", ImVec2(20, 20))) {
                
                currentPath = currentPath.parent_path();
            }
        } 

        ImGui::SameLine();
        ImGui::Text(currentPath.filename().string().c_str());

        ImGui::SameLine(ImGui::GetWindowWidth() - 270);
        ImGui::Text("icon size");
        ImGui::SameLine();

        ImGui::PushItemWidth(180);
        ImGui::SliderFloat("##iconSize", &iconSize, 50.0f, 200.0f);
        ImGui::PopItemWidth();
        
        ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

        ImGui::BeginChild("##content", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::Indent(8);
        ImGui::Dummy(ImVec2(0, 8));


        float panelWidth = ImGui::GetContentRegionAvail().x;
        DrawBreadcrumbBar();


        // right click
        if(ImGui::BeginPopupContextWindow()) {
            if(ImGui::BeginMenu("Create")) {
                if(ImGui::MenuItem("Folder")) {
                    isCreateFolderPanelActive = true;
                }
                if(ImGui::MenuItem("Scene")) {
                    isCreateScenePanelActive = true;
                }
                if(ImGui::BeginMenu("Mesh")) {
                    if(ImGui::MenuItem("Cube")) {
                        AssetsManager::CreateMesh(MeshType::CUBE, currentPath);
                    } else if(ImGui::MenuItem("Cylinder")) {
                        AssetsManager::CreateMesh(MeshType::CYLINDER, currentPath);
                    } else if(ImGui::MenuItem("Capsule")) {
                        AssetsManager::CreateMesh(MeshType::CAPSULE, currentPath);
                    } else if(ImGui::MenuItem("Sphere")) {
                        AssetsManager::CreateMesh(MeshType::SPHERE, currentPath);
                    }
                    ImGui::EndMenu();
                }
                ImGui::MenuItem("Script");
                ImGui::MenuItem("Material");
                ImGui::MenuItem("Texture");
                ImGui::EndMenu();
            }
            
            std::string openTxt = std::string("Open ") + currentPath.filename().string() + std::string(" Folder");

            if(ImGui::MenuItem(openTxt.c_str())) {
                ShowOpenWithDialog(fs::absolute(currentPath).string());
            }
            ImGui::EndPopup();
        }

        std::vector<AssetItem> items = assetsManager.List(currentPath);

        float cursorW = 0;
        float itemWidthWithSpacing = iconSize + ImGui::GetStyle().ItemSpacing.x;
        
        // assets
        for (auto& item : items) {
            std::error_code ec;
            ImTextureID icon;
            
            if (item.isDir) {
                // Check if folder is empty (non-throwing)
                bool isEmpty = fs::is_empty(item.itemPath, ec);
                icon = (ImTextureID)(intptr_t)(isEmpty && !ec ? folderEmptyTex.TexId : folderTex.TexId);
            } else {
                icon = (ImTextureID)(intptr_t)fileTex.TexId;
            }
            
            // Check if THIS item would overflow - if so, wrap to new line
            if (cursorW > 0 && cursorW + itemWidthWithSpacing > panelWidth) {
                cursorW = 0; // Start new line
            }
            
            // If not first item on line, call SameLine
            if (cursorW > 0) {
                ImGui::SameLine();
            }
            
            DrawAssetItem(icon, item.itemPath.filename().string().c_str(), {iconSize, iconSize}, item.itemPath);

            cursorW += itemWidthWithSpacing;
        }

        // P A N E L S
        if(isRenamePanelActive) {
            Utils::GUI::ShowTextInputDialoge(
                "Rename File",
                toRename.stem().string().c_str(),
                isRenamePanelActive,
                [this](std::string input) { onRename(std::move(input)); }
            );
        };

        if(isCreateFolderPanelActive) {
            Utils::GUI::ShowTextInputDialoge(
                "Create Folder",
                "NewFolder",
                isCreateFolderPanelActive,
                [this](std::string input) { onCreateFolder(std::move(input)); }
            );
        };

        if(isCreateScenePanelActive) {
            Utils::GUI::ShowTextInputDialoge(
                "Create Scene",
                "NewScene",
                isCreateScenePanelActive,
                [this](std::string input) { onCreateScene(currentPath, std::move(input)); }
            );
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selectedItem = ""; // deselect on empty area click
        }
        
        ImGui::Unindent(8);
        ImGui::EndChild();


    
    ImGui::End();
}

void AssetsBrowser::DrawBreadcrumbBar() {

};

void AssetsBrowser::onCreateScene(fs::path dir, std::string input) {
    assetsManager.CreateScene(std::move(dir), std::move(input));
}

void AssetsBrowser::onOpenScene(fs::path& path) {
    LOG::Info("Opening Scene: ", path.filename().string());
    sceneManager.SetScene(path);
}

void AssetsBrowser::onRename(std::string input) {
    // Build the new path using the user-provided name and original parent directory
    assetsManager.Rename(toRename, toRename.parent_path() / (input + toRename.extension().string()));
}

void AssetsBrowser::onCreateFolder(std::string input) {
    // Build the new path using the user-provided name and original parent directory
    assetsManager.CreateFolder(currentPath / input);
}

void AssetsBrowser::DrawAssetItem(
    ImTextureID icon,
    const char* label,
    ImVec2 iconSize,
    fs::path itemPath
)
{
    ImGui::BeginGroup(); // makes image + text behave as one item

    bool isActive = (selectedItem == itemPath);
    
    // Active style
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
    } else {
        // Transparent style you already use
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered , ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
    }

    bool clicked = ImGui::ImageButton(
        label,          // unique ID
        icon,
        iconSize, {0,1}, {1,0}, ImVec4(0, 0, 0, 0)
    );

    if (ImGui::BeginDragDropSource()) {
        const wchar_t* itemPathW = itemPath.c_str();
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathW, (wcslen(itemPathW) + 1) * sizeof(wchar_t));
        ImGui::Text("%s", label); 
        ImGui::EndDragDropSource();
    }

    ImGui::PopStyleColor(3);

    if (clicked) {
        if (!isActive) {
            selectedItem = itemPath;
        } else if (fs::is_directory(selectedItem)) {
            currentPath = selectedItem;
        } else if (selectedItem.extension() == ".scene") {
            onOpenScene(selectedItem);
        }
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open"))
        {
            ShowOpenWithDialog(fs::absolute(itemPath).string()); 
        }

        if (ImGui::MenuItem("Rename"))
        {
            isRenamePanelActive = true;
            toRename = itemPath;
        }

        if (ImGui::MenuItem("Delete"))
        {
            if(WinMsg::YesNo("Confirm", "This will permanently delete this item.\nContinue?"))
                assetsManager.Delete(itemPath);
        }

        ImGui::EndPopup();
    }


    // Center text under image
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float itemWidth = iconSize.x;

    ImGui::SetCursorPosX(
        ImGui::GetCursorPosX() + (itemWidth - textSize.x) * 0.5f
    );
    ImGui::TextUnformatted(label);

    ImGui::EndGroup();

}



void AssetsBrowser::ShowOpenWithDialog(std::string filePath)
{
    std::wstring wPath(filePath.begin(), filePath.end());
    OPENASINFO info{};
    info.pcszFile = wPath.c_str();
    info.pcszClass = nullptr;
    info.oaifInFlags = OAIF_ALLOW_REGISTRATION |
                       OAIF_REGISTER_EXT |
                       OAIF_EXEC;

    SHOpenWithDialog(nullptr, &info);
}

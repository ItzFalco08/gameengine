#include "AssetsBrowserPanel.hpp"
#include "../utils/globals.hpp"
#include <vector>
#include "../utils/Logger.hpp"
#include <windows.h>
#include <shlobj.h>
#include <cstring>

void AssetsBrowser::InitIcons() {
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    fileTex = Texture("../src/textures/File.png", texDets);
    folderTex = Texture("../src/textures/Folder.png", texDets);
    folderEmptyTex = Texture("../src/textures/FolderEmpty.png", texDets);
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
    
    
    if(ImGui::Button("<-", ImVec2(20, 20))) {
        currentPath = currentPath.parent_path();
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


    if(ImGui::BeginPopupContextWindow()) {
        if(ImGui::BeginMenu("Create")) {
            if(ImGui::MenuItem("Folder")) {
                isCreateFolderPanelActive = true;
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

    int cursorW = 0;
    int itemWidthWithSpacing = iconSize + ImGui::GetStyle().ItemSpacing.x;
    
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
        
        DrawAssetItem(icon, item.itemPath.filename().string().c_str(), {(float)iconSize, (float)iconSize}, item.itemPath);

        cursorW += itemWidthWithSpacing;
    }

    // rename panel
    if(isRenamePanelActive) ShowRenamePanel();
    if(isCreateFolderPanelActive) ShowCreateFolderPanel();

    ImGui::Unindent(8);
    ImGui::EndChild();
    
    ImGui::End();
}

void AssetsBrowser::DrawBreadcrumbBar() {

};

void AssetsBrowser::DrawAssetItem(
    ImTextureID icon,
    const char* label,
    ImVec2 iconSize,
    fs::path itemPath
)
{

    ImGui::BeginGroup(); // ⬅ makes image + text behave as one item

    // Image button
    if (ImGui::ImageButton(
        label,          // unique ID
        icon,
        iconSize, {0,0}, {1,1}
    ))
    {
        if(selectedItem == itemPath) {
            currentPath = selectedItem;
        } else {
            selectedItem = itemPath;
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

void AssetsBrowser::ShowRenamePanel() { 
    ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(
        ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Rename Asset", nullptr, flags);
    
    static char inputBuffer[256];
    if (inputBuffer[0] == '\0') {
        strncpy(inputBuffer, toRename.filename().string().c_str(), sizeof(inputBuffer) - 1);
        inputBuffer[sizeof(inputBuffer) - 1] = '\0';
    }
    ImGui::InputText("New Name", inputBuffer, sizeof(inputBuffer));
    
    if (ImGui::Button("OK"))
    {
        assetsManager.Rename(toRename, fs::path(toRename.parent_path() / inputBuffer));
        isRenamePanelActive = false;        
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        isRenamePanelActive = false;
    }

    ImGui::End();
}

void AssetsBrowser::ShowCreateFolderPanel() { 
    ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(
        ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Create Folder", nullptr, flags);
    
    static char inputBuffer[256] = {0};
    ImGui::InputText("Folder Name", inputBuffer, sizeof(inputBuffer));
    
    if (ImGui::Button("OK"))
    {
        if (inputBuffer[0] != '\0') {
            assetsManager.CreateFolder(currentPath / inputBuffer);
            isCreateFolderPanelActive = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        isCreateFolderPanelActive = false;
    }

    ImGui::End();
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

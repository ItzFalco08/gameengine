#pragma once
#include "imgui/imgui.h"
#include <filesystem>
#include <functional>
#include <string>
#include "../core/Texture.hpp"
namespace fs = std::filesystem;


class AssetsBrowser {
public:
    void Render();
    AssetsBrowser() = default;
    void InitIcons();

private:
    void DrawBreadcrumbBar();
    void DrawAssetItem( ImTextureID icon, const char* label, ImVec2 iconSize, fs::path itemPath);
    void ShowOpenWithDialog(std::string filePath);
    void onCreateFolder(std::string input);
    void onRename(std::string input);
    void onCreateScene(fs::path dir, std::string input);
    void onOpenScene(fs::path& path);
    fs::path currentPath = PROJECT_ROOT "/Assets";
    Texture fileTex;
    Texture folderTex;
    Texture folderEmptyTex;
    float iconSize = 112.0f;
    fs::path selectedItem;
    bool isRenamePanelActive = false;
    bool isCreateFolderPanelActive = false;
    bool isCreateMaterialPanelActive = false;
    bool isCreateTexturePanelActive = false;
    bool isCreateScenePanelActive = false;
    fs::path toRename = "";
};
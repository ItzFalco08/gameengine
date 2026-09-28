#pragma once
#include "imgui.h"
#include <filesystem>
#include <string>
#include <vector>
#include "../../core/Texture.hpp"
#include <mutex>

namespace fs = std::filesystem;

enum class AssetItemType {
    Folder,
    File,
    Texture,
    Material,
    Mesh,
    Scene
};

struct AssetItem {
    fs::path itemPath = "";
    AssetItemType assetType = AssetItemType::File;
};

class AssetsBrowser {
public:
    void Render();
    AssetsBrowser() = default;
    void InitIcons();
    void handleFileDrop(const char* filePath);

private:
    void DrawAssetItem( ImTextureID icon, const char* label, ImVec2 iconSize, fs::path itemPath);
    void OpenFile(std::string filePath);
    void OpenScene(fs::path& path);
    void renderPanels();
    void createScene(const fs::path& path);
    void createFolder(const fs::path& dir);
	void deleteAsset(const fs::path& path);
    std::vector<AssetItem> getAssetItems(const fs::path& path);
    void rename(std::string input);

    fs::path currentPath;
    Texture2D fileTex;
    Texture2D folderTex;
    Texture2D folderEmptyTex;
    float iconSize = 112.0f;
    fs::path selectedItem;
    fs::path toRename = "";
	fs::path importModelPath = "";
    std::mutex impProgressMtx;
    int progress = 0;
};
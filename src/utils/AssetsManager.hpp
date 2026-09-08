#pragma once
#include <vector>
#include <filesystem>
#include "efsw/efsw.hpp"
#include "Logger.hpp"

namespace fs = std::filesystem;

struct AssetItem {
    fs::path itemPath;
    bool isDir;
};

enum MeshType {
    CUBE,
    SPHERE,
    CAPSULE,
    CYLINDER
};

static void handleAssetAction() {

}

// listener
class AssetsListener : public efsw::FileWatchListener {
public:
    void handleFileAction(
        efsw::WatchID watchid, const std::string& dir,
        const std::string& filename, efsw::Action action,
        const std::string& oldFilename
    ) override {
        if(fs::is_directory(std::filesystem::path(dir) / filename)) return;

        switch (action) {
        case efsw::Actions::Add:
            LOG::Info("File Added: ", filename);
            break;
        case efsw::Actions::Moved:
            LOG::Info("File Moved: ", filename);
            break;
        case efsw::Actions::Delete:
            LOG::Info("File Deleted: ", filename);
            break;
        case efsw::Actions::Modified:
            LOG::Info("File Modified: ", filename);
            break;
        }
    }
};

// manual asset manipulation
class AssetsManager {
public:
    static std::vector<AssetItem> List(const fs::path& directory);

    static void CreateFolder(const fs::path& path);
    static void CreateFile(const fs::path& path);
    static void Rename(const fs::path& from, const fs::path& to);
    static void Delete(const fs::path& path);
    static void CreateScene(fs::path dir, std::string sceneName);
    static void CreateMesh(MeshType meshType, const fs::path& writeTo);
};
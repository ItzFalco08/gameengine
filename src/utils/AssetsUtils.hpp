#pragma once
#include <vector>
#include <filesystem>
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
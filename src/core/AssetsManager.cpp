#include "AssetsManager.hpp"
#include "../utils/Logger.hpp"
#include "../utils/WinMsg.hpp"
#include "json/json.hpp"
#include <fstream>

std::vector<AssetItem> AssetsManager::List(const fs::path& directory) {
    if (!fs::exists(directory) || !fs::is_directory(directory))
    {
        LOG::Error("directory doesn't exist ", directory.string());
        return {};
    }

    std::vector<AssetItem> items;
    for (const auto& directoryItem : fs::directory_iterator(directory)) {
        AssetItem item;
        item.itemPath = directoryItem.path();
        item.isDir = directoryItem.is_directory();

        items.push_back(item);
    }

    return items;
};

void AssetsManager::CreateFolder(const fs::path& path) {
    if(fs::exists(path)) {
        WinMsg::Warning("Warning", "Folder with this name already exists!");
    }
    fs::create_directory(path);
};

void AssetsManager::CreateFile(const fs::path& path) {
    
};

void AssetsManager::Rename(const fs::path& from, const fs::path& to) {
    fs::rename(from, to);
};

void AssetsManager::Delete(const fs::path& path) {
    std::error_code ec;
    
    if (!fs::exists(path, ec))
    {
        LOG::Error("Delete failed: path does not exist: ",
                   path.filename().string());
        return;
    }
    
    if (fs::is_directory(path, ec))
        fs::remove_all(path, ec);
    else
        fs::remove(path, ec);

    if(ec) {
        LOG::Error("Error While Deleting: ", path.filename().string());
    }
};

void AssetsManager::CreateScene(fs::path dir, std::string sceneName) {
    fs::path scenePath = dir / (sceneName + ".scene");
    if (fs::exists(scenePath)) {
        WinMsg::Warning("Warning", "A scene with this name already exists.");
        return;

    }
    std::ofstream sceneFile(scenePath);

    nlohmann::json defaultSceneJson;
    defaultSceneJson["gameObjects"] = nlohmann::json::array();
    sceneFile << defaultSceneJson;
};

void AssetsManager::CreateMesh(MeshType meshType, const fs::path& dir) {
    std::string fileName;

    switch (meshType) {
        case CUBE: fileName = "Cube.obj"; break;
        case CYLINDER: fileName = "Cylinder.obj"; break;
        case SPHERE: fileName = "Sphere.obj"; break;
        case CAPSULE: fileName = "Capsule.obj"; break;
        default: fileName = "Mesh.obj"; break;
    }

    fs::path writeTo = dir / fileName;

    // Optional: Avoid overwriting existing files
    int counter = 1;
    while (fs::exists(writeTo)) {
        writeTo = dir / (fileName.substr(0, fileName.find_last_of('.')) + " (" + std::to_string(counter++) + ").obj");
    }

    std::ofstream file(writeTo);
    if(!file) {
        LOG::Warning("Unable to create Mesh File at path:", writeTo.string());
        return;
    }

    switch (meshType)
    {
    case CUBE:
        file << R"(# Blender v2.76 (sub 0) OBJ File: ''
# www.blender.org
mtllib cube.mtl
o Cube
v 1.000000 -1.000000 -1.000000
v 1.000000 -1.000000 1.000000
v -1.000000 -1.000000 1.000000
v -1.000000 -1.000000 -1.000000
v 1.000000 1.000000 -0.999999
v 0.999999 1.000000 1.000001
v -1.000000 1.000000 1.000000
v -1.000000 1.000000 -1.000000
vt 1.000000 0.333333
vt 1.000000 0.666667
vt 0.666667 0.666667
vt 0.666667 0.333333
vt 0.666667 0.000000
vt 0.000000 0.333333
vt 0.000000 0.000000
vt 0.333333 0.000000
vt 0.333333 1.000000
vt 0.000000 1.000000
vt 0.000000 0.666667
vt 0.333333 0.333333
vt 0.333333 0.666667
vt 1.000000 0.000000
vn 0.000000 -1.000000 0.000000
vn 0.000000 1.000000 0.000000
vn 1.000000 0.000000 0.000000
vn -0.000000 0.000000 1.000000
vn -1.000000 -0.000000 -0.000000
vn 0.000000 0.000000 -1.000000
usemtl Material
s off
f 2/1/1 3/2/1 4/3/1
f 8/1/2 7/4/2 6/5/2
f 5/6/3 6/7/3 2/8/3
f 6/8/4 7/5/4 3/4/4
f 3/9/5 7/10/5 8/11/5
f 1/12/6 4/13/6 8/11/6
f 1/4/1 2/1/1 4/3/1
f 5/14/2 8/1/2 6/5/2
f 1/12/3 5/6/3 2/8/3
f 2/12/4 6/8/4 3/4/4
f 4/13/5 3/9/5 8/11/5
f 5/6/6 1/12/6 8/11/6
)";
        break;
    
    case CYLINDER:
        file << "Undefined Mesh";
    case SPHERE:
        file << "Undefined Mesh";
    case CAPSULE:
        file << "Undefined Mesh";

    default:
        LOG::Warning("Specified MeshType does not exist!");
        break;
    }
};
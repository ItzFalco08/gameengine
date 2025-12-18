#include "AssetsManager.hpp"
#include "../utils/Logger.hpp"

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
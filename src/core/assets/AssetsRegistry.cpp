#include "AssetsRegistry.hpp"
#include "meta/TextureAsset.hpp"
#include "meta/MaterialAsset.hpp"
#include "meta/MeshAsset.hpp"
#include "../../utils/Logger.hpp"
#include "../../utils/globals.hpp"

void AssetsRegistry::syncAssets() {
    syncAssets(ASSETS_DIR);
}

void AssetsRegistry::syncAssets(const fs::path& dir) {
    m_syncFuture = std::async(std::launch::async, [this, dir]() {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                addAsset(entry.path());
            }
        }
    });
    SDL_SetCursor(appState.loadingCursor);
}

void AssetsRegistry::addAsset(const fs::path& path) {
    std::string ext = path.extension().string();
    if (ext == ".mat") {
        MaterialAsset materialAsset;
        if(!materialAsset.Serialize(path)) {
            LOG::Error("Failed to serialize material asset: ", path.string());
            return;
        }
        m_assetsMeta.insert_or_assign(materialAsset.m_uuid, std::move(materialAsset));
    }
    else if (ext == ".tex") {
        TextureAsset textureAsset;
        if (!textureAsset.Serialize(path)) {
			LOG::Error("Failed to serialize texture asset: ", path.string());
            return;
        }
        m_assetsMeta.insert_or_assign(textureAsset.m_uuid, std::move(textureAsset));
    }
    else if (ext == ".mesh") {
        MeshAsset meshAsset;
        if (!meshAsset.Serialize(path)) {
            LOG::Error("Failed to serialize mesh asset: ", path.string());
            return;
        }
        m_assetsMeta.insert_or_assign(meshAsset.m_uuid, std::move(meshAsset));
    }
}

AssetMeta* AssetsRegistry::getAsset(const UUID& uuid) {
    auto it = m_assetsMeta.find(uuid);
    if (it != m_assetsMeta.end()) {
        return &it->second;
    }
    return nullptr;
}

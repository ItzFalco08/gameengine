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
        auto materialAsset = std::make_unique<MaterialAsset>();
        if(!materialAsset->Serialize(path)) {
            LOG::Error("Failed to serialize material asset: ", path.string());
            return;
        }
        m_assetsMeta.set(materialAsset->m_uuid, std::move(materialAsset));
    }
    else if (ext == ".tex") {
        auto textureAsset = std::make_unique<TextureAsset>();
        if (!textureAsset->Serialize(path)) {
			LOG::Error("Failed to serialize texture asset: ", path.string());
            return;
        }
        m_assetsMeta.set(textureAsset->m_uuid, std::move(textureAsset));
    }
    else if (ext == ".mesh") {
        auto meshAsset = std::make_unique<MeshAsset>();
        if (!meshAsset->Serialize(path)) {
            LOG::Error("Failed to serialize mesh asset: ", path.string());
            return;
        }
        m_assetsMeta.set(meshAsset->m_uuid, std::move(meshAsset));
    }
}


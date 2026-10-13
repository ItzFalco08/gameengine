#include "AssetsRegistry.hpp"
#include "meta/TextureAsset.hpp"
#include "meta/MaterialAsset.hpp"
#include "meta/MeshAsset.hpp"
#include "../../utils/Logger.hpp"

void AssetsRegistry::syncAssets() {
    syncAssets(ASSETS_DIR);
}

void AssetsRegistry::syncAssets(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            addAsset(entry.path());
        }
    }
}

 uint32_t AssetsRegistry::size() {
    return m_assetsMeta.size();
}

void AssetsRegistry::addAsset(const fs::path& path) {
    std::string ext = path.extension().string();

    if (ext == ".mat") {
        auto materialAsset = std::make_unique<MaterialAsset>();
        if(!materialAsset->Serialize(path)) {
            LOG::Error("Failed to serialize material asset: ", path.string());
            return;
        }
		UUID uuid = materialAsset->m_uuid;
        m_assetsMeta.set(uuid, std::move(materialAsset));
    }
    else if (ext == ".tex") {
        auto textureAsset = std::make_unique<TextureAsset>();
        if (!textureAsset->Serialize(path)) {
			LOG::Error("Failed to serialize texture asset: ", path.string());
            return;
        }
        UUID uuid = textureAsset->m_uuid;
        m_assetsMeta.set(uuid, std::move(textureAsset));
    }
    else if (ext == ".mesh") {
        auto meshAsset = std::make_unique<MeshAsset>();
        if (!meshAsset->Serialize(path)) {
            LOG::Error("Failed to serialize mesh asset: ", path.string());
            return;
        }
        UUID uuid = meshAsset->m_uuid;
        m_assetsMeta.set(uuid, std::move(meshAsset));
    }
}


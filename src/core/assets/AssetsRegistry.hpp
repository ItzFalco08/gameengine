#pragma once

#include <filesystem>
#include <unordered_map>
#include "meta/AssetMeta.hpp"
#include <future>

namespace fs = std::filesystem;

class AssetsRegistry {
private:
    // UUID -> metadata
    std::unordered_map<UUID, AssetMeta, UUIDHasher> m_assetsMeta;

public:
    std::future<void> m_syncFuture;

    void syncAssets();
    void syncAssets(const fs::path& dir);

    void addAsset(const fs::path& path);
    AssetMeta* getAsset(const UUID& uuid);
};
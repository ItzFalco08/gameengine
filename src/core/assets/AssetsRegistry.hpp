#pragma once

#include <filesystem>
#include "meta/AssetMeta.hpp"
#include <future>
#include "AssetPool.hpp"

namespace fs = std::filesystem;

class AssetsRegistry {
public:
    AssetsPool<AssetMeta> m_assetsMeta; // UUID -> runtimeId -> AssetHandle

    std::future<void> m_syncFuture;

    void syncAssets();
    void syncAssets(const fs::path& dir);

    void addAsset(const fs::path& path);
    template<typename T>
    T* getAsset(const UUID& uuid) {
        return dynamic_cast<T*>(m_assetsMeta.get(uuid));
    }
};
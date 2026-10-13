#pragma once

#include <filesystem>
#include <vector>
#include "meta/AssetMeta.hpp"
#include <future>
#include "AssetPool.hpp"

namespace fs = std::filesystem;

class AssetsRegistry {
public:
    AssetsPool<AssetMeta> m_assetsMeta; // UUID -> runtimeId -> AssetHandle

    void syncAssets();
    void syncAssets(const fs::path& dir);

    uint32_t size();

    void addAsset(const fs::path& path);
    template<typename T>
    T* getAsset(const UUID& uuid) {
        return dynamic_cast<T*>(m_assetsMeta.get(uuid));
    }
};
